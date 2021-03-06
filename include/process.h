#ifndef __PROCESS_H__
#define __PROCESS_H__
#define KSTACK_SIZE 4096
#include "adt/list.h"

#define ANY -1
#define sys_getpid 100
typedef struct Semaphore {
	int taken;
	ListHead block;		/* blocking queue */
} Sem;


typedef struct PCB {
	void *tf;
	uint32_t kstack[KSTACK_SIZE];
	int depth;
	int sleep;
	pid_t pid;
	CR3 cr3;
	Sem msg_mutex,msg_num;
	ListHead list,listmsg;
} PCB;


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
extern Msg Msg_q[100000];
extern int depth;


PCB* create_kthread(void *fun);

void lock(void);
void unlock(void);

void sleep(Sem *s);
void wakeup(PCB *PCB_of_thread);

void send(pid_t dest, Msg *m);
void receive(pid_t src, Msg *m);

void create_sem(Sem *sem, int num);
PCB *fetch_pcb(pid_t pid);
void A();
void B();
void C();
void D();
void E();
void read_mbr();
void fm_test();
#endif
