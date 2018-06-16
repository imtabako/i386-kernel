#include <kernel.h>
#include <stdio.h>

#include <pg.h>


extern paddr_t get_cr3();

extern char pg_directory;
extern char _ekern;

paddr_t *pg_dir;
vaddr_t  _prev;		/* No idea how to name it */


void
pg_init()
{
	pg_dir = (unsigned long *)&pg_directory;
	_prev = (vaddr_t)&_ekern;
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
	if (addr) {
		_prev = res + 0x1000;
		return res;
	}

	/*
	 * In case a free page isn't found we have one more
	 * page table (1023) that wasn't looked up
	 */
	tmp = pg_dir[1022];
	pg_dir[1022] = get_cr3() + flags;

	/* 1022 DIR index, 1023 TABLE index*/
	addr = (paddr_t *)0xFFBFF000;
	for (i = 0; i < 1024; i++) {
		if (addr[i] & PG_PRES)
			continue;

		res = ((paddr_t)addr | (i << 2)) << 10;
		break;
	}

	pg_dir[1022] = tmp;
	_prev = res + 0x1000;
	return res;
}

