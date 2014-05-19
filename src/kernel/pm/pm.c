#include "kernel.h"
#include "pm.h"
pid_t PM;


static void init_pm(void)
{
	PCB* p = create_kthread(pm_thread);
	PM = p->pid;
	wakeup(p);
}

void create_new_process(int file_name)
{
	
}

static void pm_thread(void)
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
