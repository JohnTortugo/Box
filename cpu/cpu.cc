#define NEED_CPU_REG_SHORTCUTS 1
#include "bochs.h"
#include "cpu.h"
#include "debug.h"

int CacheSize;

void BX_CPU_C::cpu_loop(void)
{
 bxInstruction_c *i = new bxInstruction_c();
 int ret;

 while (CacheSize >= 1) 
 {
  printf("RIP: %lx CacheSize: %d\n", RIP,CacheSize);
  ret=fetchDecode32((Bit8u *) RIP, i, CacheSize);

  CacheSize -= i->ilen();
  RIP += i->ilen();
 }
}

#define BX_REPEAT_TIME_UPDATE_INTERVAL (BX_MAX_TRACE_LENGTH-1)

void BX_CPP_AttrRegparmN(2) BX_CPU_C::repeat(bxInstruction_c *i, BxRepIterationPtr_tR execute)
{
  // non repeated instruction
  if (! i->repUsedL()) {
    BX_CPU_CALL_REP_ITERATION(execute, (i));
    return;
  }

#if BX_X86_DEBUGGER
  BX_CPU_THIS_PTR in_repeat = 0;
#endif

#if BX_SUPPORT_X86_64
  if (i->as64L()) {
    while(1) {
      if (RCX != 0) {
        BX_CPU_CALL_REP_ITERATION(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX --;
      }
      if (RCX == 0) return;

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_CPU_THIS_PTR icount++;

     // BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
    }
  }
  else
#endif
  if (i->as32L()) {
    while(1) {
      if (ECX != 0) {
        BX_CPU_CALL_REP_ITERATION(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        RCX = ECX - 1;
      }
      if (ECX == 0) return;

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_CPU_THIS_PTR icount++;

      //BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
    }
  }
  else  // 16bit addrsize
  {
    while(1) {
      if (CX != 0) {
        BX_CPU_CALL_REP_ITERATION(execute, (i));
        BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
        CX --;
      }
      if (CX == 0) return;

#if BX_DEBUGGER == 0
      if (BX_CPU_THIS_PTR async_event)
#endif
        break; // exit always if debugger enabled

      BX_CPU_THIS_PTR icount++;

      //BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
    }
  }

#if BX_X86_DEBUGGER
  BX_CPU_THIS_PTR in_repeat = 1;
#endif

  RIP = BX_CPU_THIS_PTR prev_rip; // repeat loop not done, restore RIP

  // assert magic async_event to stop trace execution
  BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
}

void BX_CPP_AttrRegparmN(2) BX_CPU_C::repeat_ZF(bxInstruction_c *i, BxRepIterationPtr_tR execute)
{
  unsigned rep = i->repUsedValue();

  // non repeated instruction
  if (! rep) {
    BX_CPU_CALL_REP_ITERATION(execute, (i));
    return;
  }

#if BX_X86_DEBUGGER
  BX_CPU_THIS_PTR in_repeat = 0;
#endif

  if (rep == 3) { /* repeat prefix 0xF3 */
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      while(1) {
        if (RCX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX --;
        }
        if (! get_ZF() || RCX == 0) return;

#if BX_DEBUGGER == 0
        if (BX_CPU_THIS_PTR async_event)
#endif
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        //BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
    else
#endif
    if (i->as32L()) {
      while(1) {
        if (ECX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX = ECX - 1;
        }
        if (! get_ZF() || ECX == 0) return;

#if BX_DEBUGGER == 0
        if (BX_CPU_THIS_PTR async_event)
#endif
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        //BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
    else  // 16bit addrsize
    {
      while(1) {
        if (CX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          CX --;
        }
        if (! get_ZF() || CX == 0) return;

#if BX_DEBUGGER == 0
        if (BX_CPU_THIS_PTR async_event)
#endif
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        //BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
  }
  else {          /* repeat prefix 0xF2 */
#if BX_SUPPORT_X86_64
    if (i->as64L()) {
      while(1) {
        if (RCX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX --;
        }
        if (get_ZF() || RCX == 0) return;

#if BX_DEBUGGER == 0
        if (BX_CPU_THIS_PTR async_event)
#endif
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        //BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
    else
#endif
    if (i->as32L()) {
      while(1) {
        if (ECX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          RCX = ECX - 1;
        }
        if (get_ZF() || ECX == 0) return;

#if BX_DEBUGGER == 0
        if (BX_CPU_THIS_PTR async_event)
#endif
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        //BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
    else  // 16bit addrsize
    {
      while(1) {
        if (CX != 0) {
          BX_CPU_CALL_REP_ITERATION(execute, (i));
          BX_INSTR_REPEAT_ITERATION(BX_CPU_ID, i);
          CX --;
        }
        if (get_ZF() || CX == 0) return;

#if BX_DEBUGGER == 0
        if (BX_CPU_THIS_PTR async_event)
#endif
          break; // exit always if debugger enabled

        BX_CPU_THIS_PTR icount++;

        //BX_SYNC_TIME_IF_SINGLE_PROCESSOR(BX_REPEAT_TIME_UPDATE_INTERVAL);
      }
    }
  }

#if BX_X86_DEBUGGER
  BX_CPU_THIS_PTR in_repeat = 1;
#endif

  RIP = BX_CPU_THIS_PTR prev_rip; // repeat loop not done, restore RIP

  // assert magic async_event to stop trace execution
  BX_CPU_THIS_PTR async_event |= BX_ASYNC_EVENT_STOP_TRACE;
}
