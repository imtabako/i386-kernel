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

paddr_t pg_find();

#endif
