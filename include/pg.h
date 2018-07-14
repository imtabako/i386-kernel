#ifndef	__PG_H__
#define	__PG_H__

#define	MEM_MAXLEN	6

#define	PG_PRES	(1 << 0)
#define	PG_RW	(1 << 1)
#define	PG_US	(1 << 2)
#define	PG_ACS	(1 << 5)
#define	PG_DIRT	(1 << 6)

struct mem_area {
	uint32_t base;
	uint32_t end;
} __attribute__ ((packed));


typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;


void pg_init();

void pg_map(vaddr_t src, paddr_t dst, unsigned long flags);

#define	pg_free(vaddr)	pg_map(vaddr, 0, 0)

vaddr_t pg_alloc();

vaddr_t pg_find();

#endif	/* __PG_H__ */
