import matplotlib.pyplot as plt
from pylab import *
flag=0
t=0
n=0
time1=[]
delay1=[]
for line in open("tcp-example.tr"):
    if line[0]=='+':
            tm= float(line[line.find(" ")+1: (line.find(" ", line.find(" ")+1))])
            time1.append(tm)
            t=tm
            flag+=1
            n+=1
    if line[0]=='-':
            tm=float(line[line.find(" ")+1: (line.find(" ", line.find(" ")+1))])
            t=tm-time1[n-flag]
            delay1.append(t)
            flag-=1
plt.plot(time1[0:len(delay1)], delay1, 'r')
plt.xlim([1,10])
plt.ylim([amin(delay1),amax(delay1)*1.1])
plt.title("Queing Delay vs. Time")
plt.xlabel("Time")
plt.ylabel("Delay")
plt.show()