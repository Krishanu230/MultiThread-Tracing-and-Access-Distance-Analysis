import sys

#validation
if(len(sys.argv) != 2):
    print("invalid input")
    exit(0)
else:
    fileName = str(sys.argv[1])

counter = 0
AccessesSofar = {}
with open(fileName) as infile:
    for line in infile:
        line = line.rstrip().split(" ")
        if(line[0] == "#eof"):
            break

        memAddr = int(line[2])
        threadID = int(line[0].strip(":"))
        blockNumber = memAddr >> 6
        prevAccess = AccessesSofar.get(blockNumber, -1)
        if prevAccess == -1:
            AccessesSofar[blockNumber] = set([threadID])
        else:
            AccessesSofar[blockNumber].add(threadID)
        counter += 1

blocksSharedPerThread = [0, 0, 0, 0, 0, 0, 0, 0]
for block, threadsCount in AccessesSofar.items():
    blocksSharedPerThread[len(threadsCount)-1] += 1

print(blocksSharedPerThread)
