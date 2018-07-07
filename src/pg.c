#include <kernel.h>
#include <stdio.h>

#include <pg.h>


extern paddr_t get_cr3();
extern void upd_cr3();

extern char pg_directory;
extern char _ekern;

paddr_t *pg_dir = (unsigned long *)&pg_directory;
paddr_t phys_ptr = (paddr_t)&_ekern - KERNEL_BASE;
vaddr_t  _prev = (vaddr_t)&_ekern;	


void
pg_init()
{
	kprintf("phys_ptr %08X\n", phys_ptr);
}

vaddr_t
pg_alloc()
{
	/* Always fails yet */
	return 0;
}

void
pg_map(vaddr_t src, paddr_t dst, unsigned long flags)
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
pg_find()
{
	vaddr_t *addr, res;
	unsigned long flags, tmp;
	size_t i, j;

	flags = PG_PRES | PG_RW;
	res = 0;

	/*
	 * Self-referencing trick:
	 * temporarily map DIR[1023] to DIR itself
	 */
	tmp = pg_dir[1023];
	pg_dir[1023] = get_cr3() | flags;

	/* Start searching for a page from last allocated page */
	for (i = _prev >> 22; i < 1023; i++) {
		if (! pg_dir[i] & PG_PRES)
			continue;

		addr = (vaddr_t *)(0xFFC00000 | (i << 12));

		for (j = 0x3FF & (_prev >> 12); j < 1024; j++) {
			if (addr[j] & PG_PRES)
				continue;

			/* Result must address a page frame */
			res = ((unsigned long)addr | (j << 2)) << 10;
			goto end;
		}
	}
	addr = NULL;

end:
	pg_dir[1023] = tmp;
	if (addr)
		return res;

	/*
	 * In case a free page isn't found we have one more
	 * page table (1023) that wasn't looked up.
	 * If last page table isn't present, return.
	 */
	if (! pg_dir[1023] & PG_PRES)
		return res;	/* `res' here is equal to 0 */

	tmp = pg_dir[1022];
	pg_dir[1022] = get_cr3() | flags;

	/* 1022 DIR index, 1023 TABLE index*/
	addr = (vaddr_t *)0xFFBFF000;
	for (i = 0; i < 1024; i++) {
		if (addr[i] & PG_PRES)
			continue;

		res = ((paddr_t)addr | (i << 2)) << 10;
		break;
	}

	pg_dir[1022] = tmp;
	return res;
}

