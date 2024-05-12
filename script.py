user_bytes_Dl = {}
with open('DlRlcStats.txt', 'r') as file:
    next(file)
    first_start = None
    last_end = None
    for line in file:
        data = line.split('\t')

        start_time = float(data[1])
        end_time = float(data[2])
        
        if first_start is None or start_time < first_start:
            first_start = start_time
        if last_end is None or end_time > last_end:
            last_end = end_time

        imsi = data[3]
        tx_bytes = float(data[7])
        rx_bytes = float(data[9])

        if imsi in user_bytes_Dl:
            user_bytes_Dl[imsi][0] += tx_bytes
            user_bytes_Dl[imsi][1] += rx_bytes
        else:
            user_bytes_Dl[imsi] = [tx_bytes, rx_bytes]
time = last_end-first_start
user_bytes_Ul={}
with open('UlRlcStats.txt', 'r') as file:
    next(file)
    for line in file:
        data = line.split()
        imsi = data[3]
        tx_bytes = float(data[7])
        rx_bytes = float(data[9])

        if imsi in user_bytes_Ul:
            user_bytes_Ul[imsi][0] += tx_bytes
            user_bytes_Ul[imsi][1] += rx_bytes
        else:
            user_bytes_Ul[imsi] = [tx_bytes, rx_bytes]







throughput_Dl={}
for imsi in user_bytes_Dl:
    throughput_Dl[imsi]=user_bytes_Dl[imsi][0]+user_bytes_Dl[imsi][1] /time

throughput_Ul={}
for imsi in user_bytes_Ul:
    throughput_Ul[imsi]=user_bytes_Ul[imsi][0]+user_bytes_Ul[imsi][1]/time

print(throughput_Dl)
print(throughput_Ul)
    
