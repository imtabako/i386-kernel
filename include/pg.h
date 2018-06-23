#ifndef	__PG_H__
#define	__PG_H__

#define	PG_PRES	(1 << 0)
#define	PG_RW	(1 << 1)
#define	PG_US	(1 << 2)
#define	PG_ACS	(1 << 5)
#define	PG_DIRT	(1 << 6)

typedef uint32_t paddr_t;
typedef uint32_t vaddr_t;

void pg_init();

void pg_map(vaddr_t src, paddr_t dst, unsigned long flags);

paddr_t pg_alloc();

paddr_t pg_find();

#endif	/* __PG_H__ */
