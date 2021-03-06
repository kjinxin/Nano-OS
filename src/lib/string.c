#include "common.h"

/* 注意！itoa只有一个缓冲，因此
 * char *p = itoa(100);
 * char *q = itoa(200);
 * 后p和q所指内容都是"200"。
 */
char *itoa(int a) {
	static char buf[30];
	char *p = buf + sizeof(buf) - 1;
	if (a>=0)
	do {
		*--p = '0' + a % 10;
	} while (a /= 10);
	else
	{
		do {
      			*--p = '0' - (a % 10);
    		} while (a /= 10);
    		*--p = '-';
	}
	return p;
}

char *itoah(int a){
	const char *digit = "0123456789abcdef";
	static char buf[30];
	char *p = buf + sizeof(buf) - 1;
	if (a>=0)
	do {
		*--p = digit[a % 16];
	} while (a /= 16);
	else
	{
		do {
      			*--p = digit[- (a % 10)];
    		} while (a /= 10);
    		*--p = '-';
	}
	return p;
}

void memcpy(void *dest, const void *src, size_t size) {
	asm volatile ("cld; rep movsb" : : "c"(size), "S"(src), "D"(dest));
}

void memset(void *dest, uint8_t data, size_t size) {
	asm volatile ("cld; rep stosb" : : "c"(size), "a"(data), "D"(dest));
}

size_t strlen(const char *str) {
	int len = 0;
	while (*str ++) len ++;
	return len;
}

void strcpy(char *d, const char *s) {
	memcpy(d, s, strlen(s) + 1);
}

size_t strcmp(const char *s1, const char *s2)
{
	while (*s1)
	{
		if ((*s1)!=(*s2))
		return 1;
		s1++; s2++;		
	}
	if (*s2) return 1;
	return 0;			
}
