#include "kernel.h"
PCB pcbx;
PCB pcby;
int num=0;
PCB*
create_kthread(void *fun) {
	PCB *pcb;
	if (num==0) pcb=&pcbx;
	else pcb=&pcby;
	(*pcb).tf = (*pcb).kstack;
	(*pcb).kstack[14]=(uint32_t) fun;  // eip
	(*pcb).kstack[15]=0x8;      // cs 
	(*pcb).kstack[16]=0x202;   // eflags
	num++;
	return pcb;
}
void A(void);
void B(void);
void
init_proc() {
	create_kthread(A);
	create_kthread(B);
}
void A () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {printk("a");}
        x ++;
    }
}
void B () { 
    int x = 0;
    while(1) {
        if(x % 100000 == 0) {printk("b");}
        x ++;
    }
}

