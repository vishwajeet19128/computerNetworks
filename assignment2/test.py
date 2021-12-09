import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("Data1.csv")
#print(data)
time = data['Time'].tolist()
print(len(time))
print(time[2] - time[1])
length1 = data['Length'].tolist()
print(length1)

x = []
y = []
sum1 = 0
startTime = 0
i = 0
while(i < len(time)):
    if(time[i] - startTime >= 2):
        throughput = sum1 / (time[i] - startTime)
        y.append(throughput)
        x.append(time[i])
        sum1 = 0
        startTime = time[i]
    else:
        sum1 += length1[i]
        i+=1;

plt.plot(x, y)
plt.xlabel('time(sec)')
plt.ylabel('throughput(kbps)')
plt.title('aggregate throughput for every 2 sec')
plt.show()