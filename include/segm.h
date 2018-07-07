#ifndef	__SEGM_H__
#define	__SEGM_H__

#include <stdint.h>

#define	PRIVL_KERNEL	0x0
#define	PRIVL_USER	0x3

#define	SEGM_READ	(1 << 0)
#define	SEGM_WRITE	(1 << 1)
#define	SEGM_EXEC	(1 << 2)

#define	GDT_PRIVL_MASK	0x3

#define	GDT_ACCESS_PRESENT	(1 << 7)
#define	GDT_ACCESS_EXEC		(1 << 3)
#define	GDT_ACCESS_DC		(1 << 2)
#define	GDT_ACCESS_RW		(1 << 1)

#define	GDT_FLAG_INPAGES	(1 << 3)
#define	GDT_FLAG_MODE32 	(1 << 2)

struct gdtr {
	uint16_t size;
	uint32_t offset;
} __attribute__ ((packed));

struct gdt_entry {
	uint32_t lim	: 16;
	uint32_t base	: 24;
	uint8_t  acs	: 8;
	uint32_t lim_h	: 4;
	uint8_t  flags	: 4;
	uint32_t base_h	: 8;
} __attribute__ ((packed));

struct tss_struct {
	uint16_t	prev, res0;
	uint32_t	esp0;
	uint16_t	ss0, res1;
	uint32_t	esp1;
	uint16_t	ss1, res2;
	uint32_t	esp2;
	uint16_t	ss2, res3;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	eflags;
	uint32_t	eax;
	uint32_t	ecx;
	uint32_t	edx;
        uint32_t	ebx;
	uint32_t	esp;
        uint32_t	ebp;
	uint32_t	esi;
        uint32_t	edi;
	uint16_t	es, res4;
	uint16_t	cs, res5;
	uint16_t	ss, res6;
	uint16_t	ds, res7;
	uint16_t	fs, res8;
	uint16_t	gs, res9;
	uint16_t	ldt, res10;
	uint16_t	res11, iombase;
} __attribute__ ((packed));


void segm_init();

int set_gdt_entry(
    unsigned int index, uint32_t base, uint32_t limit, uint8_t privl, uint8_t flags);

#endif

