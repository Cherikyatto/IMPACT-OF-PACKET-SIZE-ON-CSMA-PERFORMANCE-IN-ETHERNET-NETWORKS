#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"
#include <fstream>
#include <vector>
#include <iomanip>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("CsmaTcpUdpFlowMonitor");

int main(int argc, char *argv[])
{
    // Parse command line arguments
    CommandLine cmd;
    cmd.Parse(argc, argv);

    uint32_t nNodes = 5; // Số lượng nút mạng
    uint32_t simTime = 10; // Thời gian mô phỏng

    // Mở file CSV để ghi kết quả
    std::ofstream csvFile("csma_metrics.csv", std::ios::out);
    // Ghi tiêu đề cột vào CSV
    csvFile << "PacketSize,Scenario,Protocol,TxPackets,RxPackets,ThroughputMbps,PDR\n";

    // Cấu hình cho các trường hợp khác nhau
    struct Scenario {
        std::string name;
        Time delay;
        DataRate appRate;
    };

    // Các trường hợp mô phỏng: lý tưởng, trễ, nghẽn
    std::vector<Scenario> scenarios = {
        {"ideal", MicroSeconds(1), DataRate(100000000)}, // Lý tưởng: Trễ nhỏ và tốc độ truyền cao
        {"congest", MicroSeconds(20), DataRate(20000000)} // Nghẽn: Trễ cao và tốc độ rất thấp
    };

    // Kích thước gói tin
    std::vector<uint32_t> packetSizes = {64, 128, 256, 512, 1024, 1500}; // Kích thước gói tin

    // Lặp qua các trường hợp và kích thước gói tin
    for (auto &scenario : scenarios)
    {
        for (auto pktSize : packetSizes)
        {
            NS_LOG_INFO("Running simulation with PacketSize = " << pktSize << " bytes in scenario: " << scenario.name);

            // Tạo các node mạng
            NodeContainer nodes;
            nodes.Create(nNodes);

            // Thiết lập kênh CSMA với delay và app rate tùy chỉnh
            CsmaHelper csma;
            csma.SetChannelAttribute("DataRate", DataRateValue(scenario.appRate)); // Tốc độ truyền
            csma.SetChannelAttribute("Delay", TimeValue(scenario.delay)); // Độ trễ
            NetDeviceContainer devices = csma.Install(nodes);

            // Cài đặt Internet Stack cho các node
            InternetStackHelper internet;
            internet.Install(nodes);

            // Cấu hình địa chỉ IP
            Ipv4AddressHelper ipv4;
            ipv4.SetBase("10.1.1.0", "255.255.255.0");
            Ipv4InterfaceContainer interfaces = ipv4.Assign(devices);

            // Cấu hình ứng dụng TCP
            uint16_t tcpPort = 50000;
            BulkSendHelper tcpSender("ns3::TcpSocketFactory", InetSocketAddress(interfaces.GetAddress(nNodes - 1), tcpPort));
            tcpSender.SetAttribute("MaxBytes", UintegerValue(0)); // Gửi dữ liệu không giới hạn
            tcpSender.SetAttribute("SendSize", UintegerValue(pktSize)); // Kích thước gói giống nhau
            ApplicationContainer tcpApps = tcpSender.Install(nodes.Get(0));
            tcpApps.Start(Seconds(1.0));
            tcpApps.Stop(Seconds(simTime));

            // Cài đặt máy chủ TCP (sink)
            PacketSinkHelper tcpSink("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), tcpPort));
            ApplicationContainer tcpSinkApp = tcpSink.Install(nodes.Get(nNodes - 1));
            tcpSinkApp.Start(Seconds(0.0));
            tcpSinkApp.Stop(Seconds(simTime));

            // Cấu hình ứng dụng UDP
            uint16_t udpPort = 4000;
            OnOffHelper udpSender("ns3::UdpSocketFactory", InetSocketAddress(interfaces.GetAddress(nNodes - 1), udpPort));
            udpSender.SetAttribute("DataRate", StringValue("100Mbps")); // Tốc độ truyền dữ liệu giống TCP
            udpSender.SetAttribute("PacketSize", UintegerValue(pktSize)); // Kích thước gói giống nhau
            ApplicationContainer udpApps = udpSender.Install(nodes.Get(0));
            udpApps.Start(Seconds(1.0));
            udpApps.Stop(Seconds(simTime));

            // Cài đặt máy chủ UDP (sink)
            PacketSinkHelper udpSink("ns3::UdpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), udpPort));
            ApplicationContainer udpSinkApp = udpSink.Install(nodes.Get(nNodes - 1));
            udpSinkApp.Start(Seconds(0.0));
            udpSinkApp.Stop(Seconds(simTime));

            // Cài đặt FlowMonitor để theo dõi các luồng mạng
            FlowMonitorHelper flowHelper;
            Ptr<FlowMonitor> flowMonitor = flowHelper.InstallAll();

            // Chạy mô phỏng
            Simulator::Stop(Seconds(simTime));
            Simulator::Run();

            // Lấy các thống kê luồng
            Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());
            uint64_t tcpTx = 0, tcpRx = 0, udpTx = 0, udpRx = 0;
            double tcpThroughput = 0.0, udpThroughput = 0.0;

            // Duyệt qua các luồng và tính toán thống kê
            for (auto &flow : flowMonitor->GetFlowStats())
            {
                Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flow.first);
                double throughput = flow.second.rxBytes * 8.0 /
                                    (flow.second.timeLastRxPacket.GetSeconds() - flow.second.timeFirstTxPacket.GetSeconds());

                if (t.protocol == 6) // TCP
                {
                    tcpTx += flow.second.txPackets;
                    tcpRx += flow.second.rxPackets;
                    tcpThroughput += throughput;
                }
                else if (t.protocol == 17) // UDP
                {
                    udpTx += flow.second.txPackets;
                    udpRx += flow.second.rxPackets;
                    udpThroughput += throughput;
                }
            }

            // Tính toán tỷ lệ PDR
            double tcpPDR = tcpTx > 0 ? (double)tcpRx / tcpTx * 100 : 0.0;
            double udpPDR = udpTx > 0 ? (double)udpRx / udpTx * 100 : 0.0;

            // Ghi kết quả vào file CS
            csvFile << pktSize << "," << scenario.name << ",TCP," << tcpTx << "," << tcpRx << "," << tcpThroughput / 1e6 << "," << std::fixed << std::setprecision(2) << tcpPDR << "\n";
            csvFile << pktSize << "," << scenario.name << ",UDP," << udpTx << "," << udpRx << "," << udpThroughput / 1e6 << "," << std::fixed << std::setprecision(2) << udpPDR << "\n";

            // In kết quả lên console
            std::cout << "=============================================\n";
            std::cout << "Simulation Results for PacketSize = " << pktSize << " bytes\n";
            std::cout << "               Scenario = "  << scenario.name << "\n";
            std::cout << "=============================================\n";
            std::cout << "                    TCP:\n"
                      << "       Transmitted Packets:   " << tcpTx << "\n"
                      << "       Received Packets:      " << tcpRx << "\n"
                      << "       Throughput:            " << tcpThroughput / 1e6 << " Mbps\n"
                      << "       Packet Delivery Ratio: " << std::fixed << std::setprecision(2) << tcpPDR << " %\n";
            std::cout << "                    UDP:\n"
                      << "       Transmitted Packets:   " << udpTx << "\n"
                      << "       Received Packets:      " << udpRx << "\n"
                      << "       Throughput:            " << udpThroughput / 1e6 << " Mbps\n"
                      << "       Packet Delivery Ratio: " << std::fixed << std::setprecision(2) << udpPDR << " %\n";

            // Dọn dẹp bộ nhớ và kết thúc mô phỏng
            Simulator::Destroy();

            NS_LOG_INFO("Simulation completed for PacketSize = " << pktSize << " bytes, Scenario = " << scenario.name);
        }
    }
    csvFile.close();
    return 0;
}

