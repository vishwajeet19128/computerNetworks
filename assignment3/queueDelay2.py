import codecs
import matplotlib.pyplot as plt

with codecs.open("tcp-example.tr", "r", "UTF8") as inputFile:
    inputFile=inputFile.readlines()

print(inputFile[0])
print(inputFile[1])

sz=len(inputFile)

enqueue=[]
dequeue=[]
for i in range(sz):
	data=inputFile[i].split(' ')
	if (data[0]=='+'):
		enqueue.append(data[1])
	elif (data[0]=='-'):
		dequeue.append(data[1])


print(len(enqueue))
print(len(dequeue))

queuingdelay=[]
time=[]
for i in range(len(enqueue)):
	queuingdelay.append(float(dequeue[i])-float(enqueue[i]))
	time.append(float(dequeue[i]))

# print(queuingdelay)
# print(time)

plt.figure(figsize=(20,20))
plt.plot(time,queuingdelay,label='queuingdelay')
plt.xlabel('time')
plt.ylabel('queuingdelay')
plt.legend()
plt.show()