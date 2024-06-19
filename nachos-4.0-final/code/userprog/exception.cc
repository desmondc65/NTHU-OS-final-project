// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "synchconsole.h"

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
	int	type = kernel->machine->ReadRegister(2);
	int	val, status;


	Thread * cur = kernel->currentThread;
	int oldBurstTime = cur->getRemainingBurstTime();
	int oldRunTime = cur->getRunTime();
	int result = oldBurstTime - cur->getRunTime();
    switch (which) {
	case SyscallException:
	    switch(type) {
		case SC_Halt:
		    DEBUG(dbgAddr, "Shutdown, initiated by user program.\n");
   		    kernel->interrupt->Halt();
		    break;
		case SC_PrintInt:
			val=kernel->machine->ReadRegister(4);
			kernel->synchConsoleOut->PutInt(val);
			DEBUG(dbgMLFQ, "\n");
			return;
/*		case SC_Exec:
			DEBUG(dbgAddr, "Exec\n");
			val = kernel->machine->ReadRegister(4);
			kernel->StringCopy(tmpStr, retVal, 1024);
			cout << "Exec: " << val << endl;
			val = kernel->Exec(val);
			kernel->machine->WriteRegister(2, val);
			return;
*/		case SC_Exit:
			DEBUG(dbgAddr, "Program exit\n");
			val=kernel->machine->ReadRegister(4);

			cur->setRemainingBurstTime(oldBurstTime - cur->getRunTime());

			//print thread id and remaining burst time
			if(oldRunTime > oldBurstTime){
				result = 0;
				oldRunTime = oldBurstTime;
			}
			if(oldBurstTime != 0) DEBUG(dbgMLFQ, "[UpdateRemainingBurstTime] Tick [" 
				<< kernel->stats->totalTicks << "]: Thread [" 
				<< cur->getID() << "] update remaining burst time, from: [" 
				<< oldBurstTime << "] - [" << oldRunTime << "], to [" << result << "]");
			cout << "return value:" << val << endl;
			kernel->currentThread->Finish();
			break;

		case SC_Msg:
		{
			//DEBUG(dbgSys, "Message received.\n");
			val = kernel->machine->ReadRegister(4);
			{
				char *msg = &(kernel->machine->mainMemory[val]);
				cout << msg << endl;
			}
			kernel->interrupt->Halt();
			ASSERTNOTREACHED();
			break;
		}

		case SC_Create:
			val = kernel->machine->ReadRegister(4);
			{
				char *filename = &(kernel->machine->mainMemory[val]);
				status = kernel->fileSystem->Create(filename);	
				kernel->machine->WriteRegister(2, (int)status);
			}
			return;
			ASSERTNOTREACHED();
			break;
		//<hw1
		
		case SC_Open:
			val = kernel->machine->ReadRegister(4);
			{
				char *filename = &(kernel->machine->mainMemory[val]);
				status = kernel->fileSystem->OpenAFile(filename);	
				kernel->machine->WriteRegister(2, (int)status);
			}
			return;
			ASSERTNOTREACHED();
			break;

		case SC_Write:
			val = kernel->machine->ReadRegister(4);
			{
				char *buffer = &(kernel->machine->mainMemory[val]);
				int size = kernel->machine->ReadRegister(5);
				status = kernel->fileSystem->WriteFile(buffer, size);	
				kernel->machine->WriteRegister(2, (int)status);
			}
			return;
			ASSERTNOTREACHED();
			break;

		case SC_Read:
			val = kernel->machine->ReadRegister(4);
			{
				char *buffer = &(kernel->machine->mainMemory[val]);
				int size = kernel->machine->ReadRegister(5);
				status = kernel->fileSystem->ReadFile(buffer, size);	
				kernel->machine->WriteRegister(2, (int)status);
			}
			return;
			ASSERTNOTREACHED();
			break;

		case SC_Close:
			{
				status = kernel->fileSystem->CloseFile();	
				kernel->machine->WriteRegister(2, (int)status);
			}
			return;
			ASSERTNOTREACHED();
			break;
		//hw1>
		
		default:
		    cerr << "Unexpected system call " << type << "\n";
 		    break;
	    }
	    break;
	default:
	    cerr << "Unexpected user mode exception" << which << "\n";
	    break;
    }
    ASSERTNOTREACHED();
}
