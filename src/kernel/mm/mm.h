#ifndef __MM_H__
#define __MM_H__

extern pid_t MM;
static void mm_thread(void);
static void init_fm(void);
inline CR3* get_usr_cr3(); 
#define MM_NEW_PROC 3
#define MM_NEW_PAGE 4
#define USER_NUM 10
#endif
