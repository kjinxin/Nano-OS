#ifndef __PROCESS_H__
#define __PROCESS_H__
#define KSTACK_SIZE 4096
typedef struct PCB {
	void *tf;
	uint32_t kstack[KSTACK_SIZE]; 
} PCB;

extern PCB *current;
extern PCB pcbx;
extern PCB pcby;
#endif
