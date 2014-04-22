#include "kernel.h"

PCB idle, *current = &idle;

ListHead pcbwake;
ListHead pcbsleep;

void
schedule(void) {
	// implement process/thread schedule here
	assert(!list_empty(&pcbwake));
	if  (!list_empty(&pcbwake))
	{
		current=list_entry(pcbwake.prev, PCB, list);
	}
	else current=&idle;
} 

