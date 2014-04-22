#include "kernel.h"

#define NBUF 5
#define NR_PROD 3
#define NR_CONS 4
 
int depth=0;

int buf[NBUF], f = 0, r = 0, g = 1;
int last = 0;
Sem empty, full, mutex;


void test_setup(void);


PCB PCB_thread[100];
ListHead pcbwake;
//ListHead pcbsleep;
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
	tf->ds = 0x10;
	tf->es = 0x10;	
        //其他寄存器等到下次要用的时候会统一赋值
	num++;
	return pcb;
}
void
init_proc() {
        list_init(&pcbwake);   // initialize the list of ready
        //list_init(&pcbsleep);  // initialize the list of block
	
	test_setup();
}

void lock(){  
	if (depth++==0)   // to add the depth of lock
	cli();
}

void unlock(){
	if (--depth==0)   // to dec the depth of lock
	sti();
}

void sleep(Sem *s)    // the sleep process 
{
	lock();
	//list_del(&(current->list));
	list_add_after(&(s->block),&(current->list));
	//wait_intr();   // we can set the interapt, also we can wait for the interapt!
	unlock();
	asm volatile ("int $0x80");	
}

void wakeup(PCB *PCB_of_thread)
{
	lock();     // lock it 
	list_del(&(PCB_of_thread->list));                  // first we delete from the initial list
	list_add_after(&pcbwake,&(PCB_of_thread->list));   // add to the active list 
	unlock();   // unlock it 
}


void V(Sem *s){
	lock();     // lock it 
	if (list_empty(&(s->block)))    // if we the list that wants the resource do not empty
		(s->taken)++;           //  --
	else 
		wakeup(list_entry((s->block).prev, PCB, list));    // wake up the thread
	unlock();   // unlock it 
} 

void P(Sem *s){
	lock();    // lock it 
	if (s->taken>0)
		(s->taken)--;   // if we can enter the resource , then enter
	else {
		sleep(s);  // put the current thread to sleep 
		}
	unlock();   // unlock it
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

