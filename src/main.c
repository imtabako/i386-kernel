#include <kernel.h>
#include <vga.h>
#include <stdio.h>

#include <vga.h>
#include <mboot.h>
#include <segm.h>
#include <pg.h>

#define	VER_MAJOR	0
#define	VER_MINOR	0

extern char _etext, _edata, _end, _ekern;


void
cmain(size_t magic, unsigned char *mbinfo)
{
	vga_init();
	segm_init();

	kprintf("%79s\r", "DO NOT USE");
	kprintf("sOS v%d.%d  (S[ai]mple OS)\n\n", VER_MAJOR, VER_MINOR);

	kprintf("Kernel:\t0x%X -- %p\n\n"
	        "magic %X\nmultiboot info %p\n\n",
		KERNEL_BASE, &_ekern, magic, mbinfo);


	mb_parse(mbinfo);

	pg_init();


	kprintf("Testing stupid page allocator:\n");

	vaddr_t test, tmp;

	test = pg_alloc();
	tmp = test;
	kprintf(" allocated %8x\n", test);

	test = pg_alloc();
	kprintf(" allocated %8x\n", test);

	pg_free(tmp);
	test = pg_alloc();
	kprintf(" allocated %8x\n", test);

	test = pg_alloc();
	kprintf(" allocated %8x\n", test);
}

