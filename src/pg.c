#include <kernel.h>
#include <stdio.h>

#include <mboot.h>
#include <pg.h>

#define	MEM_MAXLEN	6


extern paddr_t get_cr3();
extern void upd_cr3();

extern char pg_directory;
extern char _ekern;


paddr_t *pg_dir = (unsigned long *)&pg_directory;

paddr_t phys_ptr = (paddr_t)&_ekern - KERNEL_BASE;

struct mem_area memory[MEM_MAXLEN];
int memory_len;
int memory_idx = 0;

void
pg_init()
{
	int i;

	memory_len = mb_getmmap(memory);
	if (memory_len < 0) {
		kprintf("memory map not provided!\n"
		        "Abort! Abort! Abort!\n");

		while (1);
	} 

	for (i = 0; i < memory_len; i++)
		kprintf("base: %8X\n"
		        "end:  %8X\n", memory[i].base, memory[i].end);
	kprintf("\n");
}

vaddr_t
pg_alloc()
{
	size_t i;
	vaddr_t page;

	page = pg_find();
	if (page == 0) {
		kprintf(" haven't found page\n"
		        " phys pointer %8x\n", phys_ptr);

		if (phys_ptr >= memory[memory_idx].end) {
			memory_idx++;
			if (memory_idx >= memory_len) {
				kprintf("out of memory\n");
				return 0;
			}

			phys_ptr = memory[memory_idx].base;
		}

		/* Find non-presented page table to allocate one. */
		for (i = 0; i < 1024; i++) {
			if (pg_dir[i] & PG_PRES)
				continue;

			pg_dir[i] = phys_ptr | PG_PRES | PG_RW;
			phys_ptr += 0x1000;

			return pg_alloc();
		}

		kprintf("cannot allocate page table\n");
		return 0;
	}

	pg_map(page, phys_ptr, PG_PRES | PG_RW);
	phys_ptr += 0x1000;

	return page;
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
	for (i = 0; i < 1023; i++) {
		if (! pg_dir[i] & PG_PRES)
			continue;

		addr = (vaddr_t *)(0xFFC00000 | (i << 12));

		/* 
		 * `i' and `j' cannot be both 0 because virtual address
		 * will be 0, which is NULL
		 */
		j = (i == 0) ? 1 : 0;
		for (; j < 1024; j++) {
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

