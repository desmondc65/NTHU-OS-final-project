// alarm.cc
//	Routines to use a hardware timer device to provide a
//	software alarm clock.  For now, we just provide time-slicing.
//
//	Not completely implemented.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "alarm.h"
#include "main.h"

//----------------------------------------------------------------------
// Alarm::Alarm
//      Initialize a software alarm clock.  Start up a timer device
//
//      "doRandom" -- if true, arrange for the hardware interrupts to 
//		occur at random, instead of fixed, intervals.
//----------------------------------------------------------------------

Alarm::Alarm(bool doRandom)
{
    timer = new Timer(doRandom, this);
}

//----------------------------------------------------------------------
// Alarm::CallBack
//	Software interrupt handler for the timer device. The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	For now, just provide time-slicing.  Only need to time slice 
//      if we're currently running something (in other words, not idle).
//	Also, to keep from looping forever, we check if there's
//	nothing on the ready list, and there are no other pending
//	interrupts.  In this case, we can safely halt.
//----------------------------------------------------------------------

void 
Alarm::CallBack() 
{
    Interrupt *interrupt = kernel->interrupt;
    MachineStatus status = interrupt->getStatus();
    

    //<TODO/unsure: desmond>

    // In each 100 ticks, 
    
    // 1. Update Priority
    //An aging mechanism must be implemented, so that the priority of a process is increased by 10 after waiting for more than 400 ticks (Note: The operations of preemption and priority updating can be delayed until the next timer alarm interval).
    kernel->scheduler->UpdatePriority();

    // 2. Update RunTime & RRTime
    // Update RunTime & RRTime
    kernel->currentThread->setRunTime(kernel->currentThread->getRunTime() + TimerTicks);
    // DEBUG(dbgMLFQ, "call back: " << kernel->currentThread->getName() << " run time: " << kernel->currentThread->getRunTime());
    if(kernel->currentThread->getPriority() < 50) kernel->currentThread->setRRTime(kernel->currentThread->getRRTime() + TimerTicks);
    
    // 3. Check Round Robin
    // If the current thread has run for more than 200 ticks, it should be preempted.
    // check if in level3
    if (kernel->currentThread->getPriority() < 50 && kernel->currentThread->getRRTime() > 200) {
        //how to preempt?
        DEBUG(dbgMLFQ, "call back: RR preempt");
        kernel->interrupt->YieldOnReturn();
    }
    //<TODO>
    
     //    if (status == IdleMode) {    // is it time to quit?
 //        if (!interrupt->AnyFutureInterrupts()) {
    //        timer->Disable(); // turn off the timer
    // }
 //    } else {         // there's someone to preempt
    //    interrupt->YieldOnReturn();
 //    }
}

