#ifndef __VGA_H__
#define __VGA_H__

/* VGA text buffer memory mapped address. */
#define VGA_BUFFER_ADDR		0xC00B8000

/* BIOS Data Area */
#define	BDA_BASE_PORT_ADDRESS	0x0463
#define BDA_NUM_ROWS		0x0484
#define BDA_NUM_ROWS_LEN	1
#define BDA_NUM_COLUMNS		0x044A
#define BDA_NUM_COLUMNS_LEN	2

#define COM1_PORT_ADDRESS	0x3f8
#define COM2_PORT_ADDRESS	0x2f8
#define COM3_PORT_ADDRESS	0x3e8
#define COM4_PORT_ADDRESS	0x2e8

/* VGA text mode palette colors */

#define COLOR_BRIGHT		0x8

#define COLOR_BLACK		0x0
#define COLOR_BLUE		0x1
#define COLOR_GREEN		0x2
#define COLOR_CYAN		0x3
#define COLOR_RED		0x4
#define COLOR_MAGENTA		0x5
#define COLOR_BROWN		0x6
#define COLOR_LIGHT_GREY	0x7
#define COLOR_DARK_GREY		(COLOR_BLACK | COLOR_BRIGHT)
#define COLOR_LIGHT_BLUE	(COLOR_BLUE | COLOR_BRIGHT)
#define COLOR_LIGHT_CYAN	(COLOR_CYAN | COLOR_BRIGHT)
#define COLOR_LIGHT_RED		(COLOR_RED | COLOR_BRIGHT)
#define COLOR_LIGHT_MAGENTA	(COLOR_MAGENTA | COLOR_BRIGHT)
#define COLOR_YELLOW		(COLOR_BROWN | COLOR_BRIGHT)
#define COLOR_WHITE		(COLOR_LIGHT_GREY | COLOR_BRIGHT)

#include <stdint.h>
#include <stddef.h>

void vga_init();

void vga_clear_screen();

void vga_move_hardware_cursor(int x, int y);

void vga_putc(int c);

void vga_puts(const char *s);

void vga_set_bgcolor(int color);

void vga_set_fgcolor(int color);

void vga_get_cursor(int *x, int *y);

int vga_get_bgcolor();

int vga_get_fgcolor();

#endif	/* __VGA_H__ */

