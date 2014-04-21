#include "kernel.h"

void A(void);
void B(void);
void C(void);
void D(void);

//PCB pcbx;
//PCB pcby;
PCB PCB_of_thread_A;
PCB PCB_of_thread_B;
PCB PCB_of_thread_C;
PCB PCB_of_thread_D;
ListHead pcbwake;
ListHead pcbsleep;
int num=0;


PCB*
create_kthread(void *fun) {
	PCB *pcb;
	switch (num)
	{
		case 0:
			pcb=&PCB_of_thread_A;
			break;
		case 1:
			pcb=&PCB_of_thread_B;
			break;
		case 2:
			pcb=&PCB_of_thread_C;
			break;
		case 3:
			pcb=&PCB_of_thread_D;
			break;
		default: pcb=&PCB_of_thread_A;
	}
	TrapFrame *tf=(TrapFrame *)(pcb->kstack+KSTACK_SIZE)-1;
	pcb->tf = tf;
	tf->eip=(uint32_t) fun;
	tf->cs=0x8;
	tf->eflags=0x202; 
		
	//(*pcb).kstack[14]=(uint32_t) fun;  // eip
	//(*pcb).kstack[15]=0x8;      // cs
	//(*pcb).kstack[16]=0x202;   // eflags
        //其他寄存器等到下次要用的时候会统一赋值
	num++;
	return pcb;
}
void
init_proc() {
        list_init(&pcbwake);   // initialize the list of ready
        list_init(&pcbsleep);   // initialize the list of block
	/*create_kthread(A);      
	list_add_after(&pcbwake,&(PCB_of_thread_A.list));
	create_kthread(B);
	list_add_after(&pcbsleep,&(PCB_of_thread_B.list));
	create_kthread(C);
	list_add_after(&pcbsleep,&(PCB_of_thread_C.list));
	create_kthread(D);
	list_add_after(&pcbsleep,&(PCB_of_thread_D.list));*/
}

void sleep()
{
	list_del(&(current->list));
	list_add_after(&pcbsleep,&(current->list));
	//wait_intr();   // we can set the interapt, also we can wait for the interapt!
	asm("int $0x80");	
}

void wakeup(PCB *PCB_of_thread)
{
	list_del(&(PCB_of_thread->list));
	list_add_after(&pcbwake,&(PCB_of_thread->list));
}


#define NBUF 5
#define NR_PROD 3
#define NR_CONS 4
 
int buf[NBUF], f = 0, r = 0, g = 1;
int last = 0;
Sem empty, full, mutex;
 
void
test_producer(void) {
	while (1) {
		P(&mutex);
		P(&empty);
		if(g % 10000 == 0) {
			printk(".");	// tell us threads are really working
		}
		buf[f ++] = g ++;
		f %= NBUF;
		V(&full);
		V(&mutex);
	}
}
 
void
test_consumer(void) {
	int get;
	while (1) {
		P(&mutex);
		P(&full);
		get = buf[r ++];
		assert(last == get - 1);	// the products should be strictly increasing
		last = get;
		r %= NBUF;
		V(&empty);
		V(&mutex);
	}
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
/*void A () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("a");
            wakeup(&PCB_of_thread_B);
            sleep();
        }
        x ++;
    }
}
void B () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("b");
            wakeup(&PCB_of_thread_C);
            sleep();
        }
        x ++;
    }
}
void C () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("c");
            wakeup(&PCB_of_thread_D);
            sleep();
        }
        x ++;
    }
}
void D () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {
            printk("d");
            wakeup(&PCB_of_thread_A);
            sleep();
        }
        x ++;
    }
}
*/
