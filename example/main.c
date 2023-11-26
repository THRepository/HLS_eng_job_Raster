/*
 * "Hello World" example.
 *
 * This example prints 'Hello from Nios II' to the STDOUT stream. It runs on
 * the Nios II 'standard', 'full_featured', 'fast', and 'low_cost' example
 * designs. It runs with or without the MicroC/OS-II RTOS and requires a STDOUT
 * device in your system's hardware.
 * The memory footprint of this hosted application is ~69 kbytes by default
 * using the standard reference design.
 *
 * For a reduced footprint version of this template, and an explanation of how
 * to reduce the memory footprint for a given application, see the
 * "small_hello_world" template.
 *
 */

#include <stdio.h>
#include <system.h>
#include <io.h>
#include "raster.h"
#include "altera_avalon_timer_regs.h"

int main()
{
	int input[7] = {10, 10, 45, 170, 0, 120, 7};

	// Disable interrupt and clear interrupt status register
	VGA_RASTER_disable_interrupt();
	VGA_RASTER_clear_done_status();

	// Load samples in local memory (sample buffer)

	for (int i=0;i<7;i++){
		sample_memory_write((i<<2), *(alt_32 *) (&input[i]));
	}


	int timer_comp;
	// Call components
	while (VGA_RASTER_busy()){}
	TIMER_RESET;
	TIMER_START;
	VGA_RASTER_start();
	while (!(VGA_RASTER_done())){}
	timer_comp = TIMER_READ;

	printf("Time = %d\n", timer_comp);

	return 0;
}
