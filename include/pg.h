#ifndef	__PG_H__
#define	__PG_H__

#define	PG_PRES	(1 << 0)
#define	PG_RW	(1 << 1)
#define	PG_US	(1 << 2)
#define	PG_ACS	(1 << 5)
#define	PG_DIRT	(1 << 6)
#define	PG_ALLC	(1 << 9)

#include <stdint.h>
#include <physpg.h>

typedef uint32_t vaddr_t;

struct mem_area {
	uint32_t base;
	uint32_t end;
} __attribute__ ((packed));

void	pginit();
void	pgmap(vaddr_t dst, paddr_t src, unsigned long flags);
vaddr_t	pgfind();
vaddr_t	pgalloc();
void	pgfree(vaddr_t addr);

/* #define	pgfree(addr)	pgmap(addr, 0, 0) */

#endif	/* __PG_H__ */

