#include "kernel.h"

PCB idle, *current = &idle;

ListHead pcbwake;
ListHead pcbsleep;

void
schedule(void) {
	// implement process/thread schedule here
	if  (!list_empty(&pcbwake))
	{
		if (current->sleep==1)
		{
			list_del(&current->list);
			list_add_before(&pcbwake,&current->list);
		}
		current=list_entry(pcbwake.next, PCB, list);
		//printk("schedule %d\n",current->pid);
		//printk("%d  %x\n",current->pid,(current->cr3).val);
		write_cr3(&current->cr3);
	}
	else current=&idle;
} 

