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

typedef struct Message {
	pid_t src, dest;
	union {
		int type;
		int ret;
	};
	union {
		int i[5];
		struct {
			pid_t req_pid;
			int dev_id;
			void *buf;
			off_t offset;
			size_t len;
		};
	};
	ListHead list;
} Msg;

extern PCB *current;
extern ListHead pcbwake;
extern PCB PCB_thread[100];
extern int depth;


PCB* create_kthread(void *fun);

void lock(void);
void unlock(void);

void sleep(Sem *s);
void wakeup(PCB *PCB_of_thread);

void send(pid_t dest, Msg *m);
void receive(pid_t src, Msg *m);

#endif
