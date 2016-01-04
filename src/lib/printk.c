#include "common.h"
#include "string.h"
/* implement this function to support printk */
void vfprintf(void (*printer)(char), const char *ctl, void **args) {
void **argslist=args;
char *ch;
int value;
int t;
	for (t=0; *(ctl+t)!='\0'; t++)
	{
		if (*(ctl+t)!='%')
		{
			printer(*(ctl+t));
			continue;
		}
		t++;
		switch (*(ctl+t))
		{
			case 'd':
				value=*((int*)argslist);
				ch=itoa(value);
				for (; *ch!='\0'; ch++) 
				printer(*ch);
				argslist++;
				break;
			case 'x':
				value=*((int*)argslist);
				ch=itoah(value);
				for (; *ch!='\0'; ch++) 
				printer(*ch);
				argslist++;
				break;
			case 'c':
				printer(*((char*)argslist));
				argslist++;
				break;
			case 's':
				ch=*((char**)argslist);
				for (; *ch!='\0'; ch++)
				printer(*ch);
				argslist++;
				break;
			default:
				printer('\n');
		}	
	}
}

extern void serial_printc(char);

/* __attribute__((__noinline__))  here is to disable inlining for this function to avoid some optimization problems for gcc 4.7 */
void __attribute__((__noinline__)) 
printk(const char *ctl, ...) {
	void **args = (void **)&ctl + 1;
	vfprintf(serial_printc, ctl, args);
}
