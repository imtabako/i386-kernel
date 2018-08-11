#include <kernel.h>
#include <stdio.h>

#include <physpg.h>
#include <pg.h>

extern char pg_directory;
extern paddr_t get_cr3();

vaddr_t *pg_dir;

void
pginit()
{
	pg_dir = (vaddr_t *)&pg_directory;


	vaddr_t test;

	do {
		test = pgalloc();
		kprintf("allocated %08x\n", test);
	} while (test != 0x1000);
}

void
pgmap(vaddr_t src, paddr_t dst, unsigned long flags)
{
	vaddr_t *page;
	unsigned long ind, tmp;

	if (src >> 22 == 0x3FF)
		ind = 1022;
	else
		ind = 1023;

	tmp = pg_dir[ind];
	pg_dir[ind] = get_cr3() | PG_PRES | PG_RW;

	page = (vaddr_t *)((ind << 22) | src >> 10);
	*page = dst | flags;

	pg_dir[ind] = tmp;
}

vaddr_t
pgfind()
{
	vaddr_t *addr, res;
	unsigned long flags, tmp;
	size_t i, j;

	res = 0;
	flags = PG_PRES | PG_RW;

	/* Self-referencing trick! */
	tmp = pg_dir[1023];
	pg_dir[1023] = get_cr3() | flags;

	for (i = 0; i < 1023; i++) {
		if (! (pg_dir[i] & PG_ALLC))
			continue;

		addr = (vaddr_t *)(0xFFC00000 | (i << 12));

		/* `i' and `j' cannot both be 0 because result will be 0 (NULL). */
		j = (i == 0) ? 1 : 0;
		for (; j < 1024; j++) {
			if (addr[j] & PG_ALLC)
				continue;

			res = ((vaddr_t)addr | j * 4) << 10;
			goto end;
		}
	}
	addr = NULL;

	/* 
	 * We are here in two cases:
	 * 1)	there is a non-allocated page table;
	 * 2)	all pages are mapped.
	 * We are going to allocate a page table. If impossible, return NULL
	 */

	for (i = 0; i < 1023; i++) {
		if (! (pg_dir[i] & PG_ALLC))
			break;
	}

	/* All page tables are allocated and full. */
	if (i == 1023)
		return 0;

	addr = (vaddr_t *)(0xFFFFF000 | i);
	kprintf(">> %x\n", addr);
	*addr = physpgalloc() | flags | PG_ALLC;
	kprintf(">> %x\n", addr);

	pg_dir[1023] = tmp;
	return pgfind();

end:
	pg_dir[1023] = tmp;
	return res;
}

vaddr_t
pgalloc()
{
	vaddr_t page;
	paddr_t addr;

	addr = physpgalloc();
	page = pgfind();

	kprintf("> %x %x\n", addr, page);

	if (page == 0) {
		kprintf("cannot map %08x\n", addr);
		physpgfree(addr);
		return 0;
	}

	pgmap(page, addr, PG_PRES | PG_RW | PG_ALLC);

	return page;
}

