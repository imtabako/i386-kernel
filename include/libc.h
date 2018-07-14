#ifndef	__LIBC_H__
#define	__LIBC_H__

#include <defs.h>

size_t strlen(const char *s);

void *memcpy(void *dst, const void *src, size_t len);

void *memset(void *s, int c, size_t n);

void qsort(void *base, size_t nmemb, size_t size, int (*cmp)(const void *, const void *));

#endif	/* __LIBC_H__ */

