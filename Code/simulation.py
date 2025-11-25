import pandas as pd
import matplotlib.pyplot as plt
import os

# Đọc dữ liệu từ file CSV
df = pd.read_csv('csma_metrics.csv')

# Kiểm tra các cột trong dữ liệu
print("Columns in CSV file:", df.columns)

# Tạo cột 'Scenario' dựa trên tên file
scenario = 'ideal' if 'ideal' in os.path.basename('csma_metrics.csv') else 'congest'

# Lọc dữ liệu cho TCP và UDP trong các trường hợp ideal và congest
ideal_data = df[df['Scenario'] == 'ideal']
congest_data = df[df['Scenario'] == 'congest']

# Tạo figure với 4 subplots cho mỗi scenario
fig, axs = plt.subplots(2, 2, figsize=(9, 8))

# Đối với scenario 'ideal'
# 1. Throughput cho TCP và UDP trong scenario 'ideal'
axs[0, 0].plot(ideal_data[ideal_data['Protocol'] == 'TCP']['PacketSize'],
                ideal_data[ideal_data['Protocol'] == 'TCP']['ThroughputMbps'], label='TCP', marker='o')
axs[0, 0].plot(ideal_data[ideal_data['Protocol'] == 'UDP']['PacketSize'],
                ideal_data[ideal_data['Protocol'] == 'UDP']['ThroughputMbps'], label='UDP', marker='o')
axs[0, 0].set_title('Ideal Scenario-Throughput')
axs[0, 0].set_xlabel('Packet Size (Bytes)')
axs[0, 0].set_ylabel('Throughput (Mbps)')
axs[0, 0].legend()

# 2. Received Packets cho TCP và UDP trong scenario 'ideal'
axs[0, 1].plot(ideal_data[ideal_data['Protocol'] == 'TCP']['PacketSize'],
                ideal_data[ideal_data['Protocol'] == 'TCP']['RxPackets'], label='TCP', marker='o')
axs[0, 1].plot(ideal_data[ideal_data['Protocol'] == 'UDP']['PacketSize'], 
                ideal_data[ideal_data['Protocol'] == 'UDP']['RxPackets'], label='UDP', marker='o')
axs[0, 1].set_title('Ideal Scenario-Received Packets')
axs[0, 1].set_xlabel('Packet Size (Bytes)')
axs[0, 1].set_ylabel('Received Packets')
axs[0, 1].legend()

# 3. Transmitted Packets cho TCP và UDP trong scenario 'ideal'
axs[1, 0].plot(ideal_data[ideal_data['Protocol'] == 'TCP']['PacketSize'], 
                ideal_data[ideal_data['Protocol'] == 'TCP']['TxPackets'],
                label='TCP', marker='o')
axs[1, 0].plot(ideal_data[ideal_data['Protocol'] == 'UDP']['PacketSize'], 
                ideal_data[ideal_data['Protocol'] == 'UDP']['TxPackets'], 
                label='UDP', marker='o')
axs[1, 0].set_title('Ideal Scenario-Transmitted Packets')
axs[1, 0].set_xlabel('Packet Size (Bytes)')
axs[1, 0].set_ylabel('Transmitted Packets')
axs[1, 0].legend()

# 4. PDR cho TCP và UDP trong scenario 'ideal'
axs[1, 1].plot(ideal_data[ideal_data['Protocol'] == 'TCP']['PacketSize'], 
                ideal_data[ideal_data['Protocol'] == 'TCP']['PDR'], 
                label='TCP', marker='o')
axs[1, 1].plot(ideal_data[ideal_data['Protocol'] == 'UDP']['PacketSize'], 
                ideal_data[ideal_data['Protocol'] == 'UDP']['PDR'], 
                label='UDP', marker='o')
axs[1, 1].set_title('Ideal Scenario-Packet Delivery Ratio')
axs[1, 1].set_xlabel('Packet Size (Bytes)')
axs[1, 1].set_ylabel('PDR (%)')
axs[1, 1].legend()

# Điều chỉnh khoảng cách giữa các subplots cho scenario 'ideal'
plt.tight_layout()

# Tạo figure mới cho scenario 'congest'
fig, axs = plt.subplots(2, 2, figsize=(9, 8))

# Đối với scenario 'congest'
# 1. Throughput cho TCP và UDP trong scenario 'congest'
axs[0, 0].plot(congest_data[congest_data['Protocol'] == 'TCP']['PacketSize'], 
                congest_data[congest_data['Protocol'] == 'TCP']['ThroughputMbps'], label='TCP', marker='o')
axs[0, 0].plot(congest_data[congest_data['Protocol'] == 'UDP']['PacketSize'], 
                congest_data[congest_data['Protocol'] == 'UDP']['ThroughputMbps'], label='UDP', marker='o')
axs[0, 0].set_title('Congest Scenario-Throughput')
axs[0, 0].set_xlabel('Packet Size (Bytes)')
axs[0, 0].set_ylabel('Throughput (Mbps)')
axs[0, 0].legend()

# 2. Received Packets cho TCP và UDP trong scenario 'congest'
axs[0, 1].plot(congest_data[congest_data['Protocol'] == 'TCP']['PacketSize'], 
                congest_data[congest_data['Protocol'] == 'TCP']['RxPackets'], label='TCP', marker='o')
axs[0, 1].plot(congest_data[congest_data['Protocol'] == 'UDP']['PacketSize'], 
                congest_data[congest_data['Protocol'] == 'UDP']['RxPackets'], label='UDP', marker='o')
axs[0, 1].set_title('Congest Scenario-Received Packets')
axs[0, 1].set_xlabel('Packet Size (Bytes)')
axs[0, 1].set_ylabel('Received Packets')
axs[0, 1].legend()

# 3. Transmitted Packets cho TCP và UDP trong scenario 'congest'
axs[1, 0].plot(congest_data[congest_data['Protocol'] == 'TCP']['PacketSize'],
                congest_data[congest_data['Protocol'] == 'TCP']['TxPackets'],label='TCP', marker='o')
axs[1, 0].plot(congest_data[congest_data['Protocol'] == 'UDP']['PacketSize'], 
                congest_data[congest_data['Protocol'] == 'UDP']['TxPackets'], label='UDP', marker='o')
axs[1, 0].set_title('Congest Scenario-Transmitted Packets')
axs[1, 0].set_xlabel('Packet Size (Bytes)')
axs[1, 0].set_ylabel('Transmitted Packets')
axs[1, 0].legend()

# 4. PDR cho TCP và UDP trong scenario 'congest'
axs[1, 1].plot(congest_data[congest_data['Protocol'] == 'TCP']['PacketSize'], 
                congest_data[congest_data['Protocol'] == 'TCP']['PDR'], label='TCP', marker='o')
axs[1, 1].plot(congest_data[congest_data['Protocol'] == 'UDP']['PacketSize'], 
                congest_data[congest_data['Protocol'] == 'UDP']['PDR'], label='UDP', marker='o')
axs[1, 1].set_title('Congest Scenario-Packet Delivery Ratio')
axs[1, 1].set_xlabel('Packet Size (Bytes)')
axs[1, 1].set_ylabel('PDR (%)')
axs[1, 1].legend()

# Điều chỉnh khoảng cách giữa các subplots cho scenario 'congest'
plt.tight_layout()


# Hiển thị tất cả đồ thị và bảng
plt.show()


