#include <libc.h>
#include <stdio.h>
#include <kernel.h>
#include <defs.h>
#include <pg.h>

#include <mboot.h>


uint32_t *mb_ptr;
uint32_t mb_flags;


static int
_cmp_mem(const void *ptr1, const void *ptr2)
{
	struct mem_area *m1, *m2;

	m1 = (struct mem_area *)ptr1;
	m2 = (struct mem_area *)ptr2;

	if (m1->base == m2->base)
		return m2->end - m1->end;
	return m1->base - m2->base;
}

void
mb_parse(unsigned char *mbinfo)
{
	struct mem *mem;
	uint8_t *bdev;
	char *cmd;
	struct mods *mod;
	struct symtab *sym;

	mb_ptr = (uint32_t *)mbinfo;
	mb_flags = *mb_ptr;

	if (mb_flags & MB_MEM) {
		mem = (struct mem *)(mb_ptr + 1);

		dprintf("mem lower: %dK\n"
		        "mem upper: %dK\n\n", mem->low, mem->up);
	}

	if (mb_flags & MB_DEV) {
		bdev = (uint8_t *)(mb_ptr + 3);

		dprintf("drive: %X\n"
		        "part: %02X %02X %02X\n",
			bdev[0], bdev[1], bdev[2], bdev[3]);
	}

	if (mb_flags & MB_CMD) {
		cmd = (char *)(mb_ptr + 4);

		dprintf("cmdline: %s\n", cmd);
	}

	if (mb_flags & MB_MODS) {
		mod = (struct mods *)(mb_ptr + 5);

		dprintf("%d modules at addr %p\n", mod->count, mod->addr);
	}

	if (mb_flags & MB_SYMO) {
		dprintf(":-( should not see this\n");
	}
	if (mb_flags & MB_SYME) {
		sym = (struct symtab *)(mb_ptr + 4);
		dprintf(":-D\n");
		dprintf("symtable\n"
		        "num %d\tsize %d\n"
			"addr %p\tshndx %x\n",
			sym->elf.num, sym->elf.size, sym->elf.addr, sym->elf.shndx);
	}
}

int
mb_getmmap(struct mem_area *memory)
{
	struct mmap_header *mmap;
	struct mmap_area *area;
	int i, j, siz, idx;

	idx = 0;

	if (! (mb_flags & MB_MMAP)) {
		return -1;
	}

	mmap = (struct mmap_header *)(mb_ptr + 11);
	area = (struct mmap_area *)(mmap->addr + KERNEL_BASE);

	kprintf("mmap length:\t%d\n"
		"mmap addr:\t%p\n\n", mmap->length, mmap->addr);

	siz = mmap->length;
	while (siz > 0) {
		dprintf("%s RAM:\n"
			" base address:  %8X X 4Gb + %8X\n"
			" length:\t%8X X 4Gb + %8X\n",
			area->type == 1 ? "Usable" : "Reserved",
			area->base_h, area->base_l,
			area->len_h, area->len_l);

		/* 
		 * If memory chunk is of type `1' (usable) and is above
		 * 1 Mb (BDA), add it to `memory' array.
		 */
		if (idx < MEM_MAXLEN && area->type == 1 && 
		    area->base_h == 0 && area->base_l >= 0x100000) {
			memory[idx].base = area->base_l;
			memory[idx].end = area->base_l + area->len_l;
			idx++;
		}

		siz -= area->size + 4;
		area = (unsigned char *)area + area->size+4;
	}

	/*
	 * Normalize memory array. Memory chunks may be provided not in order.
	 * Some memory chunks may overlap or be equal.
	 */
	qsort(memory, idx, sizeof (*memory), _cmp_mem);

	i = j = 0;
	while (i < idx) {
		uint32_t areabase;

		/* Merging check is too simple! */
		areabase = memory[i].base;
		while (i < idx-1 && 
		    memory[i].end >= memory[i+1].end) {
			i++;
		}

		/* Pretend like `base' & `length' less than 4Gb. */
		memory[j].base = areabase;
		memory[j].end = memory[i].end;

		i++; j++;
	}


	return j;
}

