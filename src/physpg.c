#include <kernel.h>
#include <stdio.h>

#include <physpg.h>

extern char _ekern;

uint32_t super_pages[32];
uint32_t pages[0x8000];

void
physpginit()
{
	paddr_t test;
	int i, end;

	end = ((unsigned long)&_ekern - KERNEL_BASE) / 4096; /* In pages */

	/* 
	 * Mark BDA and kernel as allocated memory. 
	 * BDA is below 1 Mb
	 * Kernel starts at 1 Mb, ends at `_ekern' (virt addr).
	 */
	for (i = 0; i < end; i++) {
		if (i != 0 && i % 1024 == 0)
			super_pages[i / 0x8000] = 1 << (i / 1024);

		pages[i / 32] |= 1 << i;
	}
	kprintf("%d pages (%x)\n", i, i);

	kprintf("Testing...\n\n");

	while (physpgalloc() != 0x400000)
		;

	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	physpgfree(test);	kprintf("freed %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test -= 0x1000;
	physpgfree(test);	kprintf("freed %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);

	physpgfree(test);	kprintf("freed %X\n", test);
	test -= 0x1000; 
	physpgfree(test);	kprintf("freed %X\n", test);
	test -= 0x1000;
	physpgfree(test);	kprintf("freed %X\n", test);
	test -= 0x1000; 
	physpgfree(test);	kprintf("freed %X\n", test); 
	test -= 0x1000;
	physpgfree(test);	kprintf("freed %X\n", test);
	test -= 0x1000; 
	physpgfree(test);	kprintf("freed %X\n", test);
	test -= 0x1000;
	physpgfree(test);	kprintf("freed %X\n", test);
	test -= 0x1000;
	physpgfree(test);	kprintf("freed %X\n", test); 
	test -= 0x1000;
	physpgfree(test);	kprintf("freed %X\n", test);
	test -= 0x1000;

	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);
	physpgfree(0x1000);	kprintf("freed 1000\n");
	test = physpgalloc();	kprintf("allocated %X\n", test);
	test = physpgalloc();	kprintf("allocated %X\n", test);

	kprintf("\ndone\n");
}

paddr_t
physpgalloc()
{
	unsigned int i, j, spi;
	paddr_t res;

	i = 0;
	while (i < 32 && super_pages[i] == 0xFFFFFFFF)
		i++;

	if (i == 32) {
		kprintf("out of memory\n");
		while (1);
	}

	/* Checking which super-page (4 Mb) is free. */
	j = 0;
	while (super_pages[i] & (1 << j))
		j++;

	i = i*32 + j;	/* index of super-page */
	i *= 32;	/* multiply by 1024, divide by 32 -- index in array */

	while (pages[i] == 0xFFFFFFFF)
		i++;

	j = 0;
	while (pages[i] & (1 << j))
		j++;

	pages[i] |= 1 << j;
	res = i * 0x20000 + j * 0x1000;

	spi = i / 32 * 32;
	for (j = 0; j < 32; j++)
		if (pages[spi + j] != 0xFFFFFFFF)
			goto ret_alloc;

	super_pages[i / 1024] |= 1 << (i / 32);

ret_alloc:
	return res;
}

void
physpgfree(paddr_t addr)
{
	unsigned int i;

	i = addr / 0x1000;	/* page number */
	pages[i / 32] ^= 1 << (i % 32);

	i /= 32;
	super_pages[i / 1024] &= ~(1 << (i / 32));
}

