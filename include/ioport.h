#ifndef	__IOPORT_H__
#define	__IOPORT_H__

#include <stdint.h>

uint8_t inb(uint16_t port);

uint16_t inw(uint16_t port);

void outb(uint16_t port, uint8_t val);

void outw(uint16_t port, uint16_t val);

void io_wait(void);

#endif	/* __IOPORT_H__ */

