/*
 * altera_avalon_VGA_regs.h
 *
 *  Created on: Apr 2, 2023
 *      Author: TomasHägg
 */

#ifndef ALTERA_AVALON_VGA_REGS_H_
#define ALTERA_AVALON_VGA_REGS_H_

#include <system.h>
#include <io.h>

enum color{
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Yellow = 6,
	White = 7
};

#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 240
#define FILL_ADDR  1<<21
#define ASCII_ADDR 1<<20
#define WRITE_ADDR 1<<19
#define CANVAS_SIZE (CANVAS_WIDTH * CANVAS_HEIGHT)

//! Returns the address offset from VGA base for pixel at given screen coordinate (x, y)
#define pixel_addr(x, y) (WRITE_ADDR | ((CANVAS_WIDTH* 4 * (y) + (x*4))) )

//! Writes a pixel with the specified color (rgb) on given screen coordinate (x, y).
#define write_pixel(x, y, rgb) IOWR_32DIRECT(VGAG_HW_IP_0_BASE, pixel_addr(x, y), (rgb))

//! Write pixles with a specified color (rgb) over the whole screen.
#define fill_screen(rgb) IOWR_32DIRECT(VGAG_HW_IP_0_BASE, FILL_ADDR, (rgb))

//! Write pixles with a specified color (rgb) on given screen cordinate (x, y) in the shape of ASCII character (ascii).
#define ASCI_cordinate(x, y) (ASCII_ADDR | pixel_addr(x, y))
#define ASCI_col_and_char(rgb, ascii) (rgb | (ascii << 24))
#define write_ASCII(x, y, rgb, ascii) IOWR_32DIRECT(VGAG_HW_IP_0_BASE, ASCI_cordinate(x, y), ASCI_col_and_char(rgb, ascii))

//! Get pixle value at cordinate (x, y).
#define pixle_read(x, y) IORD_32DIRECT(VGAG_HW_IP_0_BASE, pixel_addr(x, y))


#endif /* ALTERA_AVALON_VGA_REGS_H_ */
