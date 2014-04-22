#include "kernel.h"

#define NBUF 5
#define NR_PROD 3
#define NR_CONS 4
 
static int depth=0;
int buf[NBUF], f = 0, r = 0, g = 1;
int last = 0;
Sem empty, full, mutex;


void test_setup(void);


PCB PCB_thread[100];
ListHead pcbwake;
ListHead pcbsleep;
int num=0;


PCB*
create_kthread(void *fun) {
	PCB *pcb;
	pcb=&PCB_thread[num];
	TrapFrame *tf=(TrapFrame *)(pcb->kstack+KSTACK_SIZE)-1;
	pcb->tf = tf;
	tf->eip=(uint32_t) fun;
	tf->cs=0x8;
	tf->eflags=0x202; 	
        //其他寄存器等到下次要用的时候会统一赋值
	num++;
	return pcb;
}
void
init_proc() {
        list_init(&pcbwake);   // initialize the list of ready
        list_init(&pcbsleep);  // initialize the list of block
	test_setup();
}

void lock(){
	if (depth++==0) 
	cli();
}

void unlock(){
	if (--depth==0) 
	sti();
	depth++;
}

void sleep()
{
	unlock();
	//wait_intr();   // we can set the interapt, also we can wait for the interapt!
	lock();
	asm("int $0x80");	
}

void wakeup(PCB *PCB_of_thread)
{
	lock();
	list_del(&(PCB_of_thread->list));
	list_add_after(&pcbwake,&(PCB_of_thread->list));
	unlock();
}


void V(Sem *s){
	lock();
	if (list_empty(&(s->block)))
		(s->taken)++;
	else 
		wakeup(list_entry((s->block).prev, PCB, list));
	unlock();
}

void P(Sem *s){
	lock();
	if (s->taken>0)
		(s->taken)--;
	else {
		list_del(&(current->list));
		list_add_after(&(s->block),&(current->list));
		sleep();
		}
	unlock();
}

 
void
test_producer(void) {
	while (1) {
		P(&empty);
		P(&mutex);
		if(g % 10000 == 0) {
			printk(".");	// tell us threads are really working
		}
		buf[f ++] = g ++;
		f %= NBUF;
		V(&mutex);
		V(&full);
	}
}

 
void
test_consumer(void) {
	int get;
	while (1) {
		P(&full);
		P(&mutex);
		get = buf[r ++];
		assert(last == get - 1);	// the products should be strictly increasing
		last = get;
		r %= NBUF;
		V(&mutex);
		V(&empty);
	}
}

void create_sem(Sem *sem, int num)
{
	list_init(&(sem->block));
	sem->taken=num;
} 
void
test_setup(void) {
	create_sem(&full, 0);
	create_sem(&empty, NBUF);
	create_sem(&mutex, 1);
	int i;
	for(i = 0; i < NR_PROD; i ++) {
		wakeup(create_kthread(test_producer));
	}
	for(i = 0; i < NR_CONS; i ++) {
		wakeup(create_kthread(test_consumer));
	}
}

