#include "kernel.h"
#include "pm.h"
#include "mm.h"
#include "fm.h"
pid_t PM;

void create_new_process(int file_name)
{
	PCB *pcb;
	uint8_t buf[512];
	ProgramHeader *ph, *eph;
	uint8_t *pa, *i;
	ELFHeader *elf;
	/* read elfheader from ramdisk */
	do_read(file_name, buf, 0 , 512);
	elf = (ELFHeader *) buf;
	/* tell mm we need to run a user process */
	static Msg m;
	m.src = PM;
	m.type = MM_NEW_PROC;
	send(MM, &m);
	/* Copy process to memory */
	ph = (ProgramHeader*)((uint8_t *)elf + elf->phoff);
	eph = ph + elf->phnum;
	for (; ph < eph; ph++)
	{
		m.src = current->pid;
    		m.type = MM_NEW_PAGE;
    		m.i[0] = ph->vaddr;
    		m.i[1] = ph->memsz;
    		send(MM, &m);
    		receive(MM, &m);
		pa = (uint8_t*)m.ret;
		do_read(file_name, pa, ph->off, ph->filesz);
		for (i = pa + ph->filesz; i < pa + ph->memsz; *i ++ = 0);
	}
	pcb = create_kthread((void*) elf->entry);
	pcb->cr3= get_user_cr3();
	wakeup(pcb);
}

void pm_thread(void)
{
	static Msg m;
	while (true) {
		receive(ANY, &m);
		switch (m.type) {
			case NEW_PROC:
				create_new_process(m.dev_id);
				break;
			default: assert(0);
		}
	}
}


void create_process(int file_name)
{
	static Msg m;
	m.src = current->pid;
	m.dev_id = file_name;
	m.req_pid = current->pid;
	m.type = NEW_PROC;
	send(PM, &m);
}
void init_user_proc(void)
{
	create_process(1);	
}

void init_pm()
{
	PCB* p = create_kthread(pm_thread);
	PM = p->pid;
	wakeup(p);
}
