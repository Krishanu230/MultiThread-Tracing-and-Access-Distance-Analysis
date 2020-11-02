import sys

#validation
if(len(sys.argv) != 2):
    print("invalid input")
    exit(0)
else:
    fileName = str(sys.argv[1])

counter = 0
#We maintain a hashmap of block vs a set of threads that have accessed it so far
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
            #if accessing for the first time, add the block to the hashmap with a set of the current thread accessing it.
            AccessesSofar[blockNumber] = set([threadID])
        else:
            #else if it already exists add the thread to the set. Sets maintain the uniqueness of their elements.
            AccessesSofar[blockNumber].add(threadID)
        counter += 1

blocksSharedPerThread = [0, 0, 0, 0, 0, 0, 0, 0]
for block, threadsCount in AccessesSofar.items():
    blocksSharedPerThread[len(threadsCount)-1] += 1

print(blocksSharedPerThread)
