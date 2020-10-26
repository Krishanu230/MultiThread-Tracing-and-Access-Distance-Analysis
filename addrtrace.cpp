#include <stdio.h>
#include "pin.H"

FILE * trace;
PIN_LOCK lock;

VOID partition(THREADID tid, VOID * addr,UINT32 size, char type){
  uint64_t start =  (uint64_t) addr;
  uint64_t end ;
  uint64_t blockCutInc = start | 0x000000000000003F;

  //we can not use start+size-1 as for very high 64bit value of start(ie addr) it overflows on addition.
  //uint64_t end = min(start+size-1,blockCutInc);

  //so instead well subtract it from the next block cut and compare
  if ((start)<(blockCutInc+1-size)){
    end = start+size-1;
  }else{
    end = blockCutInc;
  }

  while(size){
    while(start<=end){
      //even though the size of uint64 and long long unsigned int is equal (8 bytes, verified via sizeof)
      // but the compliler still throws an error thatuint64_t is long unsigned int notlong long unsigned int when used with llu format specifier.
      // so we will typecast uint64_t to long long unsigned
      int eightblockscount = (int)((end-start+1)/8);
      for (int i =0; i<eightblockscount; i++){
        fprintf(trace,"%d: %c %llu\n", tid, type, (long long unsigned)start);
        start += 8;
        size -=8;
      };
      int fourblockscount = (int)((end-start+1)/4);
      for (int i =0; i<fourblockscount; i++){
        fprintf(trace,"%d: %c %llu\n", tid, type, (long long unsigned)start);
        start += 4;
        size -=4;
      };
      int twoblockscount = (int)((end-start+1)/2);
      for (int i =0; i<twoblockscount; i++){
        fprintf(trace,"%d: %c %llu\n", tid, type, (long long unsigned)start);
        start += 2;
        size -=2;
      };
      int oneblockscount = (int)((end-start+1)/1);
      for (int i =0; i<oneblockscount; i++){
        fprintf(trace,"%d: %c %llu\n", tid, type, (long long unsigned)start);
        start += 1;
        size -=1;
      };
    };
    if(size>=64){
      //go to the next mem block
      end += 64;
    }
    else{
      //we are in the last mem block
      end += size;
    };
  };
  return;
};

// Print a memory read record
VOID RecordMemRead(THREADID tid, VOID * addr,UINT32 size)
{
    PIN_GetLock(&lock, tid+1);
    partition(tid, addr, size, 'R');
    //we'll use the buffered fprintf directly in partition function flushing every time leads to considerable increase in time.
    //If the programs exits properly there should be no issues.
    //fflush(trace);
    PIN_ReleaseLock(&lock);
}

// Print a memory write record
VOID RecordMemWrite(THREADID tid, VOID * addr,UINT32 size)
{
    PIN_GetLock(&lock, tid+1);
    partition(tid, addr, size,'W');
    //we'll use the buffered fprintf directly in partition function flushing every time leads to considerable increase in time.
    //If the programs exits properly there should be no issues.
    //fflush(trace);
    PIN_ReleaseLock(&lock);
}


// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
  // Instruments memory accesses using a predicated call, i.e.
 // the instrumentation is called iff the instruction will actually be executed.
 //
 // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
 // prefixed instructions appear as predicated instructions in Pin.
 UINT32 memOperands = INS_MemoryOperandCount(ins);

 // Iterate over each memory operand of the instruction.
 for (UINT32 memOp = 0; memOp < memOperands; memOp++)
 {
     int memOpSize = INS_MemoryOperandSize(ins,memOp);
     if (INS_MemoryOperandIsRead(ins, memOp))
     {
         INS_InsertPredicatedCall(
             ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
             IARG_THREAD_ID,
             IARG_MEMORYOP_EA, memOp,
             IARG_UINT32, memOpSize,
             IARG_END);
     }
     // Note that in some architectures a single memory operand can be
     // both read and written (for instance incl (%eax) on IA-32)
     // In that case we instrument it once for read and once for write.
     if (INS_MemoryOperandIsWritten(ins, memOp))
     {
         INS_InsertPredicatedCall(
             ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
             IARG_THREAD_ID,
             IARG_MEMORYOP_EA, memOp,
             IARG_UINT32, memOpSize,
             IARG_END);
     }
 }
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    fprintf(trace, "#eof\n");
    fclose(trace);
}



/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints the IPs of every instruction executed\n"
              + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}


int main(int argc, char * argv[]){
  trace = fopen("addrtrace.out", "w");

  // Initialize pin
  if (PIN_Init(argc, argv)) return Usage();

  // Register Instruction to be called to instrument instructions
  INS_AddInstrumentFunction(Instruction, 0);

  // Register Fini to be called when the application exits
  PIN_AddFiniFunction(Fini, 0);

  // Start the program, never returns
  PIN_StartProgram();

  return 0;
}
