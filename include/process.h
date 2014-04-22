#ifndef __PROCESS_H__
#define __PROCESS_H__
#define KSTACK_SIZE 4096
#include "adt/list.h"
typedef struct PCB {
	void *tf;
	uint32_t kstack[KSTACK_SIZE];
	ListHead list; 
} PCB;

typedef struct Semaphore {
	int taken;
	ListHead block;		/* blocking queue */
} Sem;

extern PCB *current;
extern ListHead pcbwake;
//extern ListHead pcbsleep;
extern PCB PCB_thread[100];
#endif
