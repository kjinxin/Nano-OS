#ifndef __MM_H__
#define __MM_H__
#include "kernel.h"
extern pid_t MM;
void mm_thread(void);
void init_mm(void);
inline CR3* get_user_cr3(); 
#define MM_NEW_PROC 3
#define MM_NEW_PAGE 4
#define USER_NUM 1
#endif
