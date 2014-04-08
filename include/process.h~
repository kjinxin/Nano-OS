#ifndef __PROCESS_H__
#define __PROCESS_H__
#define KSTACK_SIZE 4096
#include "adt/list.h"
typedef struct PCB {
	void *tf;
	uint32_t kstack[KSTACK_SIZE];
	ListHead list; 
} PCB;

extern PCB *current;
extern ListHead pcbwake;
extern ListHead pcbsleep;
extern PCB pcbx;
extern PCB pcby;
extern PCB PCB_of_thread_A;
extern PCB PCB_of_thread_B;
extern PCB PCB_of_thread_C;
extern PCB PCB_of_thread_D;
#endif
