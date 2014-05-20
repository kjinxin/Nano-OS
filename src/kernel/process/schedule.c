#include "kernel.h"

PCB idle, *current = &idle;

ListHead pcbwake;
ListHead pcbsleep;

void
schedule(void) {
	// implement process/thread schedule here
	if  (!list_empty(&pcbwake))
	{
		if (current->sleep==0)
		{
			list_del(&current->list);
			list_add_before(&pcbwake,&current->list);
		}
		current=list_entry(pcbwake.next, PCB, list);
		write_cr3(current->cr3);
	}
	else current=&idle;
} 

