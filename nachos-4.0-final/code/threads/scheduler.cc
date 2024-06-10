// scheduler.cc 
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would 
//	end up calling FindNextToRun(), and that would put us in an 
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "main.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------



//<TODO/done: desmond>
// Declare sorting rule of SortedList for L1 & L2 ReadyQueue
// Hint: Funtion Type should be "static int"
static int SRTN_Compare(Thread *t1, Thread *t2);
static int FCFS_Compare(Thread *t1, Thread *t2);
//<TODO>

Scheduler::Scheduler()
{
//	schedulerType = type;
    // readyList = new List<Thread *>; 
    //<TODO/done: desmond>
    // Initialize L1, L2, L3 ReadyQueue
    L1ReadyQueue = new SortedList<Thread *>(SRTN_Compare);
    L2ReadyQueue = new SortedList<Thread *>(FCFS_Compare);
    L3ReadyQueue = new List<Thread *>;
    //<TODO>
	toBeDestroyed = NULL;
} 

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

Scheduler::~Scheduler()
{ 
    //<TODO/done: desmond>
    // Remove L1, L2, L3 ReadyQueue
    delete L1ReadyQueue;
    delete L2ReadyQueue;
    delete L3ReadyQueue;
    //<TODO>
    // delete readyList; 
} 

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

void
Scheduler::ReadyToRun (Thread *thread)
{
    // DEBUG(dbgMLFQ, "to ready queue thread: "  << thread->getID()); //need to delete
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    // DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());

    Statistics* stats = kernel->stats;
    //<TODO/done: desmond>
    // According to priority of Thread, put them into corresponding ReadyQueue.
    // After inserting Thread into ReadyQueue, don't forget to reset some values.
    // Hint: L1 ReadyQueue is preemptive SRTN(Shortest Remaining Time Next).

    int q_level = 0;
    if (thread->getPriority() >= 0 && thread->getPriority() <= 49) {
        L3ReadyQueue->Append(thread);
        q_level = 3;
        
    } else if (thread->getPriority() >= 50 && thread->getPriority() <= 99) {
        L2ReadyQueue->Insert(thread);
        q_level = 2;
    } else if (thread->getPriority() >= 100 && thread->getPriority() <= 149) {
        L1ReadyQueue->Insert(thread);
        q_level = 1;
    }
    DEBUG(dbgMLFQ, "[InsertToQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thread->getID() << "] is inserted into queue L[" << q_level << "]");
    // DEBUG(dbgMLFQ, "to ready queue thread: "  << thread->getID() << "done");//need to delete
    //reset values
    thread->setStatus(READY);
    thread->setWaitTime(0);
    thread->setRunTime(0);
    thread->setRRTime(0);
    
    //<TODO>
    // readyList->Append(thread);
}

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

Thread *
Scheduler::FindNextToRun ()
{
    ASSERT(kernel->interrupt->getLevel() == IntOff);
    // DEBUG(dbgMLFQ, "Scheduler::FindNextToRun ()");
    /*if (readyList->IsEmpty()) {
    return NULL;
    } else {
        return readyList->RemoveFront();
    }*/

    //<TODO/done: desmond>
    // a.k.a. Find Next (Thread in ReadyQueue) to Run
    //There are 3 levels of queues: L1, L2 and L3. L1 is the highest level queue, and L3 is the lowest level queue.
    //The scheduler should select a thread to run from the highest level queue that is not empty.
    int q_level = 0;
    Thread* nextThread = NULL;
    if (!L1ReadyQueue->IsEmpty()) {
        q_level = 1;
        nextThread = L1ReadyQueue->RemoveFront();
    } else if (!L2ReadyQueue->IsEmpty()) {
        q_level = 2;
        nextThread = L2ReadyQueue->RemoveFront();
    } else if (!L3ReadyQueue->IsEmpty()) {
        q_level = 3;
        nextThread = L3ReadyQueue->RemoveFront();
    }
    if(nextThread != NULL) {
        DEBUG(dbgMLFQ, "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << nextThread->getID() << "] is removed from queue L[" << q_level << "]");
        
    }
    // DEBUG(dbgMLFQ, "Scheduler::FindNextToRun () done");
    Print();
    return nextThread;
    //<TODO>
}

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void
Scheduler::Run (Thread *nextThread, bool finishing)
{
    // DEBUG(dbgMLFQ, "Schduler Run: Thread [" << nextThread->getID() << "] is running.");
    Thread *oldThread = kernel->currentThread;
 
//	cout << "Current Thread" <<oldThread->getName() << "    Next Thread"<<nextThread->getName()<<endl;
   
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {	// mark that we need to delete current thread
         ASSERT(toBeDestroyed == NULL);
	     toBeDestroyed = oldThread;
    }
   
#ifdef USER_PROGRAM			// ignore until running user programs 
    if (oldThread->space != NULL) {	// if this thread is a user program,

        oldThread->SaveUserState(); 	// save the user's CPU registers
	    oldThread->space->SaveState();
    }
#endif
    
    oldThread->CheckOverflow();		    // check if the old thread
					    // had an undetected stack overflow

    kernel->currentThread = nextThread;  // switch to the next thread
    nextThread->setStatus(RUNNING);      // nextThread is now running
    
    // DEBUG(dbgThread, "Switching from: " << oldThread->getName() << " to: " << nextThread->getName());
    
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    cout << "Switching from: " << oldThread->getID() << " to: " << nextThread->getID() << endl;
    SWITCH(oldThread, nextThread);

    // we're back, running oldThread
      
    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << kernel->currentThread->getID());

    CheckToBeDestroyed();		// check if thread we were running
					// before this one has finished
					// and needs to be cleaned up
    
#ifdef USER_PROGRAM
    if (oldThread->space != NULL) {	    // if there is an address space
        oldThread->RestoreUserState();     // to restore, do it.
	    oldThread->space->RestoreState();
    }
#endif
}

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void
Scheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL) {
        DEBUG(dbgThread, "toBeDestroyed->getID(): " << toBeDestroyed->getID());
        delete toBeDestroyed;
	    toBeDestroyed = NULL;
    }
}
 
//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------
void
Scheduler::Print()
{
    cout << "Ready list contents:\n";
    // readyList->Apply(ThreadPrint);
    L1ReadyQueue->Apply(ThreadPrint);
    L2ReadyQueue->Apply(ThreadPrint);
    L3ReadyQueue->Apply(ThreadPrint);
}

// <TODO/done: desmond>

// Function 1. Function definition of sorting rule of L1 ReadyQueue
// L1 queue uses preemptive SRTN (shortest remaining time first) scheduling algorithm. If current thread has the lowest remaining burst time, it should not be preempted by the threads in the ready queue. The burst time (job execution time) is provided by user when execute the test case.
static int SRTN_Compare(Thread *t1, Thread *t2) {
    if (t1->getRemainingBurstTime() < t2->getRemainingBurstTime()) {
        return -1;
    } else if (t1->getRemainingBurstTime() > t2->getRemainingBurstTime()) {
        return 1;
    } else {
        return 0;
    }
}
// Function 2. Function definition of sorting rule of L2 ReadyQueue
// L2 queue uses a FCFS (First-Come First-Served) scheduling algorithm which means lower thread ID has higher priority.
static int FCFS_Compare(Thread *t1, Thread *t2) {
    if (t1->getID() < t2->getID()) {
        return -1;
    } else if (t1->getID() > t2->getID()) {
        return 1;
    } else {
        return 0;
    }
}
// Function 3. Scheduler::UpdatePriority()
// An aging mechanism must be implemented, so that the priority of a process is increased by 10 after waiting for more than 400 ticks (Note: The operations of preemption and priority updating can be delayed until the next timer alarm interval).
// Hint:
// 1. ListIterator can help.
// 2. Update WaitTime and priority in Aging situations
// 3. After aging, Thread may insert to different ReadyQueue

// unsure
void Scheduler::UpdatePriority() {
    // DEBUG(dbgMLFQ, "update priority");
    int agingThreshold = 400;  // Time after which priority is increased
    int priorityBoost = 10;    // Amount to increase priority after aging
    int tickIncrement = 100;   // Assuming this function is called every 100 ticks

    // Check L3 queue
    ListIterator<Thread *> itL3(L3ReadyQueue);
    while (!itL3.IsDone()) {
        Thread *thread = itL3.Item();
        thread->setWaitTime(thread->getWaitTime() + tickIncrement);

        if (thread->getWaitTime() > agingThreshold) {
            int newPriority = thread->getPriority() + priorityBoost;
            DEBUG(dbgMLFQ, "Thread [" << thread->getID() << "] priority is increased from " << thread->getPriority() << " to " << newPriority);
            thread->setPriority(newPriority);
            thread->setWaitTime(0); // Reset wait time after aging

            // Check if the thread needs to move from L3 to L2
            if (newPriority >= 50 && newPriority <= 99) {
                L3ReadyQueue->Remove(thread); // Remove from L3
                DEBUG(dbgMLFQ, "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread ["  \
                    << thread->getID() << "] is removed from queue L[3] in prior");
                L2ReadyQueue->Insert(thread); // Add to L2
                DEBUG(dbgMLFQ, "[InsertToQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" \
                    << thread->getID() << "] is inserted into queue L[2] in prior");
                itL3.Next(); // Move iterator forward
                continue; // Skip the Next() call at the end of the loop
            }
        }
        itL3.Next();
    }

    // Check L2 queue
    ListIterator<Thread *> itL2(L2ReadyQueue);
    while (!itL2.IsDone()) {
        Thread *thread = itL2.Item();
        thread->setWaitTime(thread->getWaitTime() + tickIncrement);

        if (thread->getWaitTime() > agingThreshold) {
            int newPriority = thread->getPriority() + priorityBoost;
            //print debug info of thread id and its new and old priority
            //[UpdatePriority] Tick [{current total tick}]: Thread [{thread ID}] changes its priority from [{old value}] to [{new value}]
            DEBUG(dbgMLFQ, "Thread [" << thread->getID() << "] priority is increased from " << thread->getPriority() << " to " << newPriority);
            thread->setPriority(newPriority);
            thread->setWaitTime(0); // Reset wait time after aging

            // Check if the thread needs to move from L2 to L1
            if (newPriority >= 100) {
                L2ReadyQueue->Remove(thread); // Remove from L2
                DEBUG(dbgMLFQ, "[RemoveFromQueue] Tick [" << kernel->stats->totalTicks << "]: Thread ["  << thread->getID() << "] is removed from queue L[2] in prior");
                L1ReadyQueue->Insert(thread); // Add to L1
                DEBUG(dbgMLFQ, "[InsertToQueue] Tick [" << kernel->stats->totalTicks << "]: Thread [" << thread->getID() << "] is inserted into queue L[1] in prior");
                itL2.Next(); // Move iterator forward
                continue; // Skip the Next() call at the end of the loop
            }
        }
        itL2.Next();
    }
    // DEBUG(dbgMLFQ, "update priority done");
}



// <TODO>