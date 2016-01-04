#include "fm.h"
#include "hal.h"
pid_t FM;
static void fm_thread(void);
void
do_read(int file_name, uint8_t *buf, off_t offset, size_t len) 
{
	static Msg m;
	m.src = current->pid;
	m.dev_id = file_name;
	m.req_pid = current->pid;
	m.buf = buf;
	m.offset = offset;
	m.len = len;
	m.type = FILE_READ;
	send(FM, &m);
	receive(FM, &m);
}

void
do_write(int file_name, uint8_t *buf, off_t offset, size_t len) 
{
	static Msg m;
	m.src = current->pid;
	m.type = FILE_WRITE;
	m.dev_id = file_name;
	m.req_pid = current->pid;
	m.buf = buf;
	m.offset = offset;
	m.len = len;
	send(FM, &m);
	receive(FM, &m);
}

void init_fm(void)
{
	PCB *p = create_kthread(fm_thread);
	FM = p->pid;
	wakeup(p);
}
static void fm_thread(void)
{
	static Msg m;
	while (true) {
		receive(ANY, &m);
		switch (m.type) {
			case FILE_READ:
				dev_read("ramdisk", m.req_pid, m.buf, m.dev_id*NR_FILE_SIZE+m.offset, m.len);
				break;
			case FILE_WRITE:
				dev_write("ramdisk", m.req_pid, m.buf, m.dev_id*NR_FILE_SIZE+m.offset, m.len);
				break;
			default: assert(0);
		}
		m.dest=m.src;
		m.src=FM;
		send(m.dest,&m);
	}
}

