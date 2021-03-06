#include "kernel.h"
#include "string.h"
#include "hal.h"
#include "time.h"
#include "fm.h"
int depth=0;
PCB PCB_thread[100];
Msg Msg_q[100000];
int msgnum=0;
ListHead pcbwake;
int num=0;
int pidA, pidB, pidC, pidD, pidE;
int IF_flag;
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
	pcb->sleep=1;
	pcb->pid=num++;
	pcb->cr3= *get_kcr3();
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

	/*wakeup(create_kthread(read_mbr));
	
	PCB *pcb;
	pcb=create_kthread(A);
	pidA=pcb->pid;
	wakeup(pcb);
	pcb=create_kthread(B);
	pidB=pcb->pid;
	wakeup(pcb);
	pcb=create_kthread(C);
	pidC=pcb->pid;
	wakeup(pcb);
	pcb=create_kthread(D);
	pidD=pcb->pid;
	wakeup(pcb);
	pcb=create_kthread(E);
	pidE=pcb->pid;
	wakeup(pcb);  */	
	//wakeup(create_kthread(fm_test));
}

void lock(){  
	if (current->depth++==0)   // to add the depth of lock
	{
		IF_flag=((read_eflags()&0x200)==0)?0:1;
		cli();
	}
}

void unlock(){
	if (--current->depth==0&&IF_flag==1)   // to dec the depth of lock
	sti();
}

void sleep(Sem *s)    // the sleep process 
{
	lock();
	if (current->sleep==1) 
	{
		current->sleep=0;
	}
	list_del(&current->list);
	list_add_before(&(s->block),&(current->list));
	//wait_intr();   // we can set the interapt, also we can wait for the interapt!
	unlock();
	asm volatile ("int $0x80");	
}

void wakeup(PCB *PCB_of_thread)
{
	lock();     // lock it 
	list_del(&(PCB_of_thread->list));                  // first we delete from the initial list
	list_add_before(&pcbwake,&(PCB_of_thread->list));   // add to the active list 
	PCB_of_thread->sleep=1;
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
	lock();
	PCB *pcb=fetch_pcb(dest);
	//P(&pcb->msg_mutex);
	m->dest=dest;
	//printk("send %d  %d   %d\n",m->type, m->src, m->dest);
	Msg *mm=&Msg_q[msgnum++];
	memcpy(mm, m, sizeof(Msg));
	list_init(&mm->list);
	list_add_before(&pcb->listmsg,&mm->list);
	unlock();
	//V(&pcb->msg_num);
	//V(&pcb->msg_mutex);
}

void receive(pid_t src, Msg *m)
{
	//P(&current->msg_num);
	//P(&current->msg_mutex);
	ListHead *ptr;
	
	start1:
	asm volatile ("":::"memory");
	list_foreach(ptr, &current->listmsg)
	{
		lock();
		Msg *curm=list_entry(ptr, Msg, list);
		if (src == ANY || src == curm->src)
		{
			list_del(ptr);
			memcpy(m, curm, sizeof(Msg));
			//V(&current->msg_mutex);
			unlock();
			return;
		}
		unlock();
	}
	
	//V(&current->msg_mutex);
	//V(&current->msg_num);
	asm volatile ("int $0x80");
	goto start1;
}

PCB *fetch_pcb(pid_t pid)
{
	return &PCB_thread[pid];
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
		//assert((read_eflags()&0x200));
		if(x % 10000000 == 0) {
			printk("a"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void B () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	receive(pidE, &m2);
	while(1) {
		//assert((read_eflags()&0x200));
		if(x % 10000000 == 0) {
			printk("b"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void C () { 
	Msg m1, m2;
	m1.src = current->pid;
	int x = 0;
	receive(pidE, &m2);
	while(1) {
		//assert((read_eflags()&0x200));
		if(x % 10000000 == 0) {
			printk("c"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
void D () { 
	Msg m1, m2;
	m1.src = current->pid;
	receive(pidE, &m2);
	int x = 0;
	while(1) {
		//assert((read_eflags()&0x200));
		if(x % 10000000 == 0) {
			printk("d"); 
			send(pidE, &m1);
			receive(pidE, &m2);
		}
		x ++;
	}
}
 
void E () {
	Msg m1, m2;
	m2.src = current->pid;
	char c;
	while(1) {
		assert((read_eflags()&0x200));
		receive(ANY, &m1);
		if(m1.src == pidA) {c = '|'; m2.dest = pidB; }
		else if(m1.src == pidB) {c = '/'; m2.dest = pidC;}
		else if(m1.src == pidC) {c = '-'; m2.dest = pidD;}
		else if(m1.src == pidD) {c = '\\';m2.dest = pidA;}
		else assert(0);
 
		printk("\033[s\033[1000;1000H%c\033[u", c);
		send(m2.dest, &m2);
	}
 
}


void
read_mbr(void) 
{
	unsigned char buf[512];
	size_t num;
	dev_read("hda", current->pid, buf, 0, 512);
	lock();
	printk("Output MBR NUMBERS:\n");
	for (num=0; num < 512; num++) {
		if (buf[num]<16) printk("0");
		printk("0x%x    ", buf[num]&255);
	}
	unlock();
	assert((read_eflags()&0x200));
	while(1);
	//sleep(&current->msg_mutex);
}

void fm_test(void)
{
	unsigned char buf[100];
	do_read(0, buf, 0, 20);
	size_t num;
	for (num=0; num<20; num++)
	printk("%d",buf[num]);
	while(1);
	//do_write(int file_name, uint8_t *buf, off_t offset, size_t len);	
}


