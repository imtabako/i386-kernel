#include <kernel.h>
#include <ioport.h>

/* This helper reads a byte from I/O port. */
inline uint8_t
inb(uint16_t port)
{
	uint8_t c;

	__asm__ __volatile__ ("inb %%dx,%%al" : "=a"(c) : "d"(port));

	return c;
}

/* This helper reads a (2-byte) word from I/O port. */
inline uint16_t
inw(uint16_t port)
{
	uint16_t c;

	__asm__ __volatile__ ("inw %%dx,%%ax" : "=a"(c) : "d"(port));

	return c;
}

/* This helper writes a byte to I/O port. */
inline void
outb(uint16_t port, uint8_t val)
{
	__asm__ __volatile__ ("outb %b0,%w1;" :: "a"(val), "d"(port));
}

/* This helper writes a (2-byte) word to I/O port. */
inline void
outw(uint16_t port, uint16_t val)
{
	__asm__ __volatile__ ("outw %%ax,%%dx" :: "d"(port), "a"(val));
}

inline void
io_wait()
{
	__asm__ __volatile__ ("movb $0, %%al" :: "a"(0));
	__asm__ __volatile__ ("outb %%al, $0x80" :: "a"(0));
}

