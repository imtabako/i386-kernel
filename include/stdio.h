#ifndef	__STDIO_H__
#define	__STDIO_H__

#include <stdint.h>
#include <stddef.h>

int kprintf(const char *fmt, ...);

int snprintf(char *buf, size_t size, const char *fmt, ...);


#endif	/* __STDIO_H__ */

