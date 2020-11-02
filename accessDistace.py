import matplotlib.pyplot as plt
import sys
import matplotlib
matplotlib.use('Agg')

#validation
if(len(sys.argv) != 2):
    print("Invalid Arguments")
    exit(0)
else:
    fileName = str(sys.argv[1])

counter = 0
#we also count total acccess - the first access to each block
totalAccessCount = 0
#We maintain a hashmap of blocks vs their access number
AccessesSofar = {}
#We also maintain a hashmap of access distance vs its count
AccessesDist = {}
with open(fileName) as infile:
    for line in infile:
        line = line.rstrip().split(" ")
        if(line[0] == "#eof"):
            break

        memAddr = int(line[2])
        #for getting block number, we could use the starting address of the block as the unique identifier that would be setting the last 6 bits to 0
        #or we could also divide by 64 to get the block number ie if its the nth block what is the value of n.
        #dividing by 64 since its a power of two, can be done by right shifting by 6 bits.
        blockNumber = memAddr >> 6
        #print("%d %d %d" % (counter, memAddr, blockNumber))
        prevAccess = AccessesSofar.get(blockNumber, -1)
        if prevAccess == -1:
            #If we are accessing the block for the first time
            AccessesSofar[blockNumber] = counter
        else:
            dist = counter - prevAccess
            distCount = AccessesDist.setdefault(dist, 0)
            AccessesDist[dist] = distCount+1
            AccessesSofar[blockNumber] = counter
            #adding here sums up total number of access except the very first ones to each block.
            totalAccessCount += 1
        counter += 1

#plotting
CumulativeFnXaxis = []
CumulativeFnYaxis = []

countSofar = 0
#we need to sort the AccessesDistances by their value as hashmaps dont have an order.
for key, value in sorted(AccessesDist.items()):
    countSofar += value
    CumulativeFnXaxis.append(key)
    CumulativeFnYaxis.append(float(countSofar)/totalAccessCount)

plt.plot(CumulativeFnXaxis, CumulativeFnYaxis)
plt.grid()
plt.xscale('log', base=10)
plt.xlabel("Access Distance in log10")
plt.ylabel("cumulative density function")
graphfile = "graph_"+fileName.split(".")[0]
plt.savefig(graphfile)
