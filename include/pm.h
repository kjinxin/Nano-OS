#ifndef __PM_H__
#define __PM_H__
void pm_thread(void);
void init_pm(void);

extern pid_t PM;
/* Structure of a ELF binary header */
typedef struct ELFHeader {
	unsigned int   magic;
	unsigned char  elf[12];
	unsigned short type;
	unsigned short machine;
	unsigned int   version;
	unsigned int   entry;
	unsigned int   phoff;
	unsigned int   shoff;
	unsigned int   flags;
	unsigned short ehsize;
	unsigned short phentsize;
	unsigned short phnum;
	unsigned short shentsize;
	unsigned short shnum;
	unsigned short shstrndx;
} ELFHeader;

/* Structure of program header inside ELF binary */
typedef struct ProgramHeader {
	unsigned int type;
	unsigned int off;
	unsigned int vaddr;
	unsigned int paddr;
	unsigned int filesz;
	unsigned int memsz;
	unsigned int flags;
	unsigned int align;
} ProgramHeader;

#define NEW_PROC 2
#endif
