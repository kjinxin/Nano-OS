#include "kernel.h"

PCB idle, *current = &idle;
PCB pcbx;  // process 1
PCB pcby;  // process 2
void
schedule(void) {
	/* implement process/thread schedule here */
	if (current==&pcbx)
		current=&pcby;
	else
		current=&pcbx;	
}
