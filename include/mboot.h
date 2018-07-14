#ifndef	__MBOOT_H__
#define	__MBOOT_H__

#include <pg.h>

#define	MB_MEM	(1 << 0)	/* memory */
#define	MB_DEV	(1 << 1)	/* boot device */
#define	MB_CMD	(1 << 2)	/* cmdline */
#define	MB_MODS	(1 << 3)	/* mods */
#define	MB_SYMO	(1 << 4)	/* symtable a.out */
#define	MB_SYME	(1 << 5)	/* symtable ELF */
#define	MB_MMAP	(1 << 6)	/* memory map */
#define	MB_DRIV	(1 << 7)	/* drives */
#define	MB_CONF	(1 << 8)	/* config table */
#define	MB_NAM	(1 << 9)	/* boot loader name */
#define	MB_APM	(1 << 10)	/* APM table */
#define	MB_VBE	(1 << 11)	/* VBE table */
#define	MB_BUF	(1 << 12)	/* frame buffer */

/* The most important structures! */
struct mmap_header {
	uint32_t length;
	uint32_t addr;
} __attribute__ ((packed));

struct mmap_area {
	uint32_t size;
	uint32_t base_l, base_h;
	uint32_t len_l, len_h;
	uint32_t type;
} __attribute__ ((packed));


struct mem {
	uint32_t low;
	uint32_t up;
} __attribute__ ((packed));

struct mods {
	uint32_t count;
	uint32_t addr;
} __attribute__ ((packed));

struct symtab {
	union {
		struct {
			uint32_t tabsiz;
			uint32_t strsiz;
			uint32_t *addr_;
			uint32_t reserved;
		} aout;
		struct {
			uint32_t num;
			uint32_t size;
			uint32_t *addr;
			uint32_t shndx;
		} elf;
	};
} __attribute__ ((packed));


void mb_parse(unsigned char *mbinfo);

int mb_getmmap(struct mem_area *memory);

#endif	/* __MBOOT_H__ */

