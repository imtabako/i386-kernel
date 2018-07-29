#ifndef	__PHYSPG_H__
#define	__PHYSPG_H__

#include <stdint.h>
typedef uint32_t paddr_t;

void	physpginit();
paddr_t	physpgalloc();
void	physpgfree(paddr_t addr);

#endif	/* __PHYSPG_H__ */

