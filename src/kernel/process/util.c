#include "kernel.h"

PCB*
create_kthread(void *fun) {
	printk("jinxin\n");
	return NULL;
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

