#include <kernel.h>

#include <segm.h>

struct gdtr gdt_ptr;
struct gdt_entry gdt[6];
struct tss_struct tss;

unsigned char interrupt_stack[4096];

extern void load_gdt(struct gdtr *);

int
set_gdt_entry(index, base, limit, privl, flags)
int index;
uint32_t base, limit;
uint8_t privl, flags;
{
	struct gdt_entry *desc;

	if (index >= NELEMS(gdt))
		return -1;

	desc = &gdt[index];

	/* Special NULL segment */
	if (index == 0) {
		desc->acs = 0;
		desc->flags = 0;
		return 0;
	}

	desc->base = base & 0xFFFFFF;
	desc->base_h = base >> 24;

	desc->flags = GDT_FLAG_MODE32;

	/* Set granularity bit for segments >= 2^20 */
	if (limit > 0xFFFFF) {
		desc->flags |= GDT_FLAG_INPAGES;
		desc->lim = (limit >> 12) & 0xFFFF;
		desc->lim_h = (limit >> 28) & 0xF;
	} else {
		desc->lim = limit & 0xFFFF;
		desc->lim_h = (limit >> 16) & 0xF;
	}

	/* Set privilege level of the segment. */
	desc->acs = (privl & GDT_PRIVL_MASK) << 5;
	/* Turn on present bit to mark valid segment. */
	desc->acs |= GDT_ACCESS_PRESENT | (1 << 4);

	/*
	 * Write access is never allowed for code segments. Data segments are
	 * always readable.
	 */
	if (flags & SEGM_EXEC) {
		desc->acs |= GDT_ACCESS_EXEC;
		if (flags & SEGM_READ)
			desc->acs |= GDT_ACCESS_RW;
	} else {
		if (flags & SEGM_WRITE)
			desc->acs |= GDT_ACCESS_RW;
	}

	return 0;
}

void
segm_init()
{
	gdt_ptr.size = sizeof gdt - 1;
	gdt_ptr.offset = (unsigned long)&gdt;

	/* NULL segment */
	set_gdt_entry(0, 0, 0, 0, 0);
	/* Kernel code/data segments */
	set_gdt_entry(1, 0, 0xFFFFFFFF, PRIVL_KERNEL, SEGM_READ | SEGM_EXEC);
	set_gdt_entry(2, 0, 0xFFFFFFFF, PRIVL_KERNEL, SEGM_READ | SEGM_WRITE);
	/* User code/data segments */
	set_gdt_entry(3, 0, 0xFFFFFFFF, PRIVL_USER, SEGM_READ | SEGM_EXEC);
	set_gdt_entry(4, 0, 0xFFFFFFFF, PRIVL_USER, SEGM_READ | SEGM_WRITE);
	
	/* Set up TSS segment */
	memset(&tss, 0, sizeof tss);
	tss.ss0 = KERNEL_DATA;
	tss.esp0 = (uint32_t)&interrupt_stack;
	tss.cs = KERNEL_CODE|PRIVL_USER;
	tss.ss = tss.es = tss.fs = tss.gs = KERNEL_DATA|PRIVL_USER;

	set_gdt_entry(5, (unsigned long)&tss, (unsigned long)&tss + sizeof(tss), PRIVL_USER, SEGM_READ|SEGM_EXEC);

	load_gdt(&gdt_ptr);
}

