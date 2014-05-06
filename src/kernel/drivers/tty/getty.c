#include "kernel.h"
#include "tty.h"
#include "hal.h"

static int tty_idx = 1;

static void
getty(void) {
	char name[] = "tty0", buf[256];
	lock();
	name[3] += (tty_idx ++);
	unlock();

	while(1) {
		/* Insert code here to do these:
		 * 1. read key input from ttyd to buf (use dev_read())
		 * 2. convert all small letters in buf into capitcal letters
		 * 3. write the result on screen (use dev_write())
		 */
		int len = dev_read(name, TTY , buf, 0, 255);
		int i;
		for (i=0; i<len; i++)
		{
			if (buf[i]>='a' && buf[i]<='z')
			buf[i]=buf[i]-'a'+'A';
		}
		buf[len++]='\n';
		len = dev_write(name, current->pid, buf , 0, len);
	}
}

void
init_getty(void) {
	int i;
	for(i = 0; i < NR_TTY; i ++) {
		wakeup(create_kthread(getty));
	}
}


