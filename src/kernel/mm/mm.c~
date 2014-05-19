#include "mm.h"
#include "kernel.h"


pid_t MM;

static void init_fm(void)
{
	PCB *p = create_kthread(mm_thread);
	MM = p->pid;
	wakeup(p);
}
static void mm_thread(void)
{
	static Msg m;
	while (true) {
		receive(ANY, &m);
		switch (m.type) {
			case MM_NEW_PROC:
				dev_read("ramdisk", m.req_pid, m.buf, m.dev_id*NR_FILE_SIZE+m.offset, m.len);
				break;
			case MM_NEW_PAGE:
				dev_write("ramdisk", m.req_pid, m.buf, m.dev_id*NR_FILE_SIZE+m.offset, m.len);
				break;
			default: assert(0);
		}
		m.dest=m.src;
		m.src=FM;
		send(m.dest,&m);
	}
}
