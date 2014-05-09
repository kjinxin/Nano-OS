#include "kernel.h"
#include "hal.h"
#include "string.h"

#define NR_MAX_FILE 8
#define NR_FILE_SIZE (128 * 1024)
pid_t RAMDISK;
static uint8_t file[NR_MAX_FILE][NR_FILE_SIZE];
static uint8_t *disk = (void*)file;
static void ramdisk_driver_thread(void);

void init_ramdisk()
{  
	
	PCB *p = create_kthread(ramdisk_driver_thread);
	RAMDISK = p->pid;
	hal_register("ramdisk", RAMDISK, 0);
	wakeup(p);
}

static void
ramdisk_driver_thread(void) {
	static Msg m;
	while (true) {
		receive(ANY, &m);
		switch (m.type) {
			case DEV_READ:
				copy_from_kernel(fetch_pcb(m.req_pid), m.buf, disk+m.offset, m.len);
				break;
			case DEV_WRITE:
				copy_to_kernel(fetch_pcb(m.req_pid), disk+m.offset, m.buf, m.len);
				break;
			default: assert(0);
		}
		m.dest=m.src;
		m.src=RAMDISK;
		send(m.dest,&m);
	}
}

