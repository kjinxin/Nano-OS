#include "kernel.h"

PCB idle, *current = &idle;
PCB pcbx;  // process 1
PCB pcby;  // process 2

PCB PCB_of_thread_A;
PCB PCB_of_thread_B;
PCB PCB_of_thread_C;
PCB PCB_of_thread_D;
ListHead pcbwake;
ListHead pcbsleep;

void
schedule(void) {
	// implement process/thread schedule here
	if  (!list_empty(&pcbwake))
	{
		current=list_entry(pcbwake.next, PCB, list);
	}
} 
/*void
schedule(void) {
	// implement process/thread schedule here 
	if (current==&pcbx)
		current=&pcby;
	else
		current=&pcbx;	
} */
