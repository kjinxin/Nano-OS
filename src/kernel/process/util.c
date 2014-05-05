#include "kernel.h"
 
int depth=0;
PCB PCB_thread[100];
ListHead pcbwake;
int num=0;
int ANY=-1;
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
	pcb->depth=0;
	pcb->pid=num++;
        //其他寄存器等到下次要用的时候会统一赋值
	list_init(&pcb->list);
	list_init(&pcb->listmsg);
	create_sem(&pcb->msg_mutex,1);
	create_sem(&pcb->msg_num,0);
	return pcb;
}
void
init_proc() {
        list_init(&pcbwake);   // initialize the list of ready
        //list_init(&pcbsleep);  // initialize the list of block
	wakeup(create_kthread(A));
	wakeup(create_kthread(B));
	wakeup(create_kthread(C));
	wakeup(create_kthread(D));
	wakeup(create_kthread(E));
}

void lock(){  
	if (current->depth++==0)   // to add the depth of lock
	cli();
}

void unlock(){
	if (--current->depth==0)   // to dec the depth of lock
	sti();
}

void sleep(Sem *s)    // the sleep process 
{
	lock();
	list_del(&(current->list));    // delete the current thread from it original list
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

void send(pid_t dest, Msg *m)
{
	PCB *pcb=&PCB_thread[dest];
	P(&pcb->msg_mutex);
	m->dest=dest;
	list_init(&m->list);
	list_add_after(&pcb->listmsg,&m->list);
	V(&pcb->msg_num);
	V(&pcb->msg_mutex);
}

void receive(pid_t src, Msg *m)
{
	P(&current->msg_num);
	P(&current->msg_mutex);
	ListHead *ptr;
	list_foreach(ptr, &current->listmsg)
	{
		Msg *curm=list_entry(ptr, Msg, list);
		if (src == ANY || src == curm->src)
		{
			list_del(ptr);
			*m=*curm;
			break;
		}
	}
	V(&current->msg_mutex);
}

void create_sem(Sem *sem, int num)
{
	list_init(&(sem->block));
	sem->taken=num;
} 

void A () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	while(1) {
		if(x % 10000000 == 0) {
			printk("a"); 
			send(4, &m1);
			receive(4, &m2);
		}
		x ++;
	}
}
void B () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	receive(4, &m2);
	while(1) {
		if(x % 10000000 == 0) {
			printk("b"); 
			send(4, &m1);
			receive(4, &m2);
		}
		x ++;
	}
}
void C () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	receive(4, &m2);
	while(1) {
		if(x % 10000000 == 0) {
			printk("c"); 
			send(4, &m1);
			receive(4, &m2);
		}
		x ++;
	}
}
void D () { 
	Msg m1, m2;
	m1.src = current->pid;
	receive(4, &m2);
	int x = 0;
	while(1) {
		if(x % 10000000 == 0) {
			printk("d"); 
			send(4, &m1);
			receive(4, &m2);
		}
		x ++;
	}
}
 
void E () {
	Msg m1, m2;
	m2.src = current->pid;
	char c;
	while(1) {
		receive(ANY, &m1);
		if(m1.src == 0) {c = '|'; m2.dest = 1; }
		else if(m1.src == 1) {c = '/'; m2.dest = 2;}
		else if(m1.src == 2) {c = '-'; m2.dest = 3;}
		else if(m1.src == 3) {c = '\\';m2.dest = 0;}
		else assert(0);
 
		printk("\033[s\033[1000;1000H%c\033[u", c);
		send(m2.dest, &m2);
	}
 
}
