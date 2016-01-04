#ifndef __STRING_H__
#define __STRING_H__

#include "common.h"

char *itoa(int);
char *itoah(int);
void memcpy(void *, const void *, size_t);
void memset(void *, uint8_t, size_t);
size_t strlen(const char *);
void strcpy(char *d, const char *s);
size_t strcmp(const char *s1, const char *s2);
#endif
