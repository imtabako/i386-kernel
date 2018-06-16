#include <kernel.h>
#include <vga.h>
#include <stdio.h>

#include <segm.h>
#include <pg.h>
#include <vga.h>

#define	VER_MAJOR	0
#define	VER_MINOR	0

extern char _etext, _edata, _end, _ekern;

void
cmain(size_t magic, unsigned char *mbinfo)
{
	/* VGA may be initialized before `flat' memory model */
	vga_init();

	segm_init();
	pg_init();

	kprintf("sOS v%d.%d  (S[ai]mple OS)\n\n", VER_MAJOR, VER_MINOR);

	kprintf("Kernel:\t0x%X -- %p\n\n"
	        "magic %X\nmultiboot info %p\n\n",
		KERNEL_BASE, &_ekern, magic, mbinfo);

	kprintf("free page at 0x%08X!\n\n", pg_find());
}

