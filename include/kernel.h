#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stdint.h>
#include <stddef.h>

#include <defs.h>
#include <libc.h>

#define	KERNEL_BASE	0xC0000000

#define KERNEL_CODE	0x8
#define KERNEL_DATA	0x10
#define USER_CODE	0x18
#define USER_DATA	0x20

#endif /* __KERNEL_H__ */

