/*!
  file          main.c
  company       HLS
  details       Program to validate VGA_raster
  date changed  2023-12-03
 */
#include <stdio.h>
#include <system.h>
#include <io.h>

#include "altera_avalon_raster_regs.h"
#include "altera_avalon_timer_regs.h"
#include "altera_avalon_VGA_regs.h"

typedef struct vec2{
	int x;
	int y;
} vec_2d;

// cross vector p0 X p1.
int cross2d(vec_2d ref0, vec_2d p0, vec_2d p1){
    //  | p0x |   | p1x |
    //  |     | X |     |
    //  | p0y |   | p1y |
    return (p0.x - ref0.x) * (p1.y - ref0.y) - (p0.y - ref0.y) * (p1.x - ref0.x);
}

int bias2d(vec_2d p0, vec_2d p1){
    vec_2d edge = {p1.x - p0.x, p1.y - p0.x};
    if (( (edge.y == 0) && (edge.x > 0) ) ||
        (edge.y < 0)){
        return 0;
    }else{
        return -1;
    }
}

char in_triangle(vec_2d pos,
           vec_2d vec_0_pos,
           vec_2d vec_1_pos,
           vec_2d vec_2_pos){
    int cross_0 = cross2d(vec_0_pos, vec_1_pos, pos) + bias2d(vec_0_pos, vec_1_pos);
    int cross_1 = cross2d(vec_1_pos, vec_2_pos, pos) + bias2d(vec_1_pos, vec_2_pos);
    int cross_2 = cross2d(vec_2_pos, vec_0_pos, pos) + bias2d(vec_2_pos, vec_0_pos);

    return cross_0 >= 0 && cross_1 >= 0 && cross_2 >= 0;
}

int main()
{
	fill_screen(Black); // Clear screen.

	// Memory for each point to draw triangles.
	int p0X[49], p0Y[49], p1X[49], p1Y[49], p2X[49], p2Y[49], pc[49];
	int time[49];

	// x0            y0             x1             y1             x2             y2            color
	p0X[0]  = 0;   p0Y[0]  = 0;   p1X[0]  = 111; p1Y[0]  = 0;   p2X[0]  = 40;  p2Y[0]  = 60;  pc[0] = 1;
	p0X[1]  = 0;   p0Y[1]  = 0;   p1X[1]  = 40;  p1Y[1]  = 60;  p2X[1]  = 0;   p2Y[1]  = 121; pc[1] = 2;
	p0X[2]  = 0;   p0Y[2]  = 121; p1X[2]  = 40;  p1Y[2]  = 60;  p2X[2]  = 60;  p2Y[2]  = 121; pc[2] = 3;
	p0X[3]  = 40;  p0Y[3]  = 60;  p1X[3]  = 120; p1Y[3]  = 80;  p2X[3]  = 60;  p2Y[3]  = 121; pc[3] = 1;
	p0X[4]  = 40;  p0Y[4]  = 60;  p1X[4]  = 160; p1Y[4]  = 40;  p2X[4]  = 120; p2Y[4]  = 80;  pc[4] = 5;
	p0X[5]  = 111; p0Y[5]  = 0;   p1X[5]  = 160; p1Y[5]  = 40;  p2X[5]  = 40;  p2Y[5]  = 60;  pc[5] = 6;
	p0X[6]  = 111; p0Y[6]  = 0;   p1X[6]  = 210; p1Y[6]  = 20;  p2X[6]  = 160; p2Y[6]  = 40;  pc[6] = 7;
	p0X[7]  = 111; p0Y[7]  = 0;   p1X[7]  = 243; p1Y[7]  = 0;   p2X[7]  = 210; p2Y[7]  = 20;  pc[7] = 1;
	p0X[8]  = 120; p0Y[8]  = 80;  p1X[8]  = 160; p1Y[8]  = 40;  p2X[8]  = 175; p2Y[8]  = 115; pc[8] = 2;
	p0X[9]  = 120; p0Y[9]  = 80;  p1X[9]  = 175; p1Y[9]  = 115; p2X[9]  = 135; p2Y[9]  = 145; pc[9] = 3;
	p0X[10] = 60;  p0Y[10] = 120; p1X[10] = 120; p1Y[10] = 80;  p2X[10] = 135; p2Y[10] = 145; pc[10] = 4;
	p0X[11] = 60;  p0Y[11] = 120; p1X[11] = 135; p1Y[11] = 145; p2X[11] = 73;  p2Y[11] = 200; pc[11] = 5;
	p0X[12] = 0;   p0Y[12] = 120; p1X[12] = 60;  p1Y[12] = 120; p2X[12] = 73;  p2Y[12] = 200; pc[12] = 6;
	p0X[13] = 0;   p0Y[13] = 120; p1X[13] = 73;  p1Y[13] = 200; p2X[13] = 0;   p2Y[13] = 210; pc[13] = 7;
	p0X[14] = 0;   p0Y[14] = 210; p1X[14] = 73;  p1Y[14] = 200; p2X[14] = 137; p2Y[14] = 222; pc[14] = 1;
	p0X[15] = 0;   p0Y[15] = 210; p1X[15] = 137; p1Y[15] = 222; p2X[15] = 0;   p2Y[15] = 240; pc[15] = 2;
	p0X[16] = 0;   p0Y[16] = 240; p1X[16] = 137; p1Y[16] = 222; p2X[16] = 137; p2Y[16] = 240; pc[16] = 3;
	p0X[17] = 72;  p0Y[17] = 200; p1X[17] = 135; p1Y[17] = 145; p2X[17] =137;  p2Y[17] = 222; pc[17] = 4;
	p0X[18] = 135; p0Y[18] = 145; p1X[18] = 171; p1Y[18] = 207; p2X[18] = 137; p2Y[18] = 222; pc[18] = 5;
	p0X[19] = 136; p0Y[19] = 221; p1X[19] = 176; p1Y[19] = 240; p2X[19] = 136; p2Y[19] = 240; pc[19] = 6;
	p0X[20] = 136; p0Y[20] = 221; p1X[20] = 176; p1Y[20] = 222; p2X[20] = 176; p2Y[20] = 240; pc[20] = 1;
	p0X[21] = 136; p0Y[21] = 222; p1X[21] = 170; p1Y[21] = 207; p2X[21] = 287; p2Y[21] = 222; pc[21] = 2;
	p0X[22] = 135; p0Y[22] = 145; p1X[22] = 175; p1Y[22] = 115; p2X[22] = 170; p2Y[22] = 207; pc[22] = 1;
	p0X[23] = 160; p0Y[23] = 40;  p1X[23] = 210; p1Y[23] = 20;  p2X[23] = 175; p2Y[23] = 115; pc[23] = 3;
	p0X[24] = 175; p0Y[24] = 115; p1X[24] = 210; p1Y[24] = 20;  p2X[24] = 222; p2Y[24] = 110; pc[24] = 4;
	p0X[25] = 175; p0Y[25] = 115; p1X[25] = 222; p1Y[25] = 110; p2X[25] = 220; p2Y[25] = 151; pc[25] = 5;
	p0X[26] = 175; p0Y[26] = 115; p1X[26] = 220; p1Y[26] = 151; p2X[26] = 170; p2Y[26] = 207; pc[26] = 6;
	p0X[27] = 170; p0Y[27] = 207; p1X[27] = 220; p1Y[27] = 151; p2X[27] = 276; p2Y[27] = 192; pc[27] = 7;
	p0X[28] = 170; p0Y[28] = 207; p1X[28] = 276; p1Y[28] = 192; p2X[28] = 287; p2Y[28] = 222; pc[28] = 1;
	p0X[29] = 276; p0Y[29] = 192; p1X[29] = 300; p1Y[29] = 171; p2X[29] = 287; p2Y[29] = 222; pc[29] = 2;
	p0X[30] = 175; p0Y[30] = 240; p1X[30] = 287; p1Y[30] = 222; p2X[30] = 280; p2Y[30] = 240; pc[30] = 3;
	p0X[31] = 175; p0Y[31] = 222; p1X[31] = 287; p1Y[31] = 222; p2X[31] = 175; p2Y[31] = 240; pc[31] = 4;
	p0X[32] = 280; p0Y[32] = 240; p1X[32] = 287; p1Y[32] = 222; p2X[32] = 320; p2Y[32] = 240; pc[32] = 5;
	p0X[33] = 287; p0Y[33] = 222; p1X[33] = 300; p1Y[33] = 171; p2X[33] = 320; p2Y[33] = 240; pc[33] = 6;
	p0X[34] = 300; p0Y[34] = 171; p1X[34] = 320; p1Y[34] = 153; p2X[34] = 320; p2Y[34] = 240; pc[34] = 7;
	p0X[35] = 300; p0Y[35] = 171; p1X[35] = 310; p1Y[35] = 85;  p2X[35] = 320; p2Y[35] = 153; pc[35] = 1;
	p0X[36] = 295; p0Y[36] = 130; p1X[36] = 310; p1Y[36] = 85;  p2X[36] = 300; p2Y[36] = 171; pc[36] = 2;
	p0X[37] = 276; p0Y[37] = 192; p1X[37] = 295; p1Y[37] = 130; p2X[37] = 300; p2Y[37] = 171; pc[37] = 3;
	p0X[38] = 222; p0Y[38] = 110; p1X[38] = 295; p1Y[38] = 130; p2X[38] = 276; p2Y[38] = 192; pc[38] = 4;
	p0X[39] = 222; p0Y[39] = 110; p1X[39] = 276; p1Y[39] = 192; p2X[39] = 220; p2Y[39] = 151; pc[39] = 5;
	p0X[40] = 222; p0Y[40] = 110; p1X[40] = 260; p1Y[40] = 60;  p2X[40] = 295; p2Y[40] = 130; pc[40] = 6;
	p0X[41] = 210; p0Y[41] = 20;  p1X[41] = 260; p1Y[41] = 60;  p2X[41] = 222; p2Y[41] = 110; pc[41] = 7;
	p0X[42] = 210; p0Y[42] = 20;  p1X[42] = 290; p1Y[42] = 35;  p2X[42] = 260; p2Y[42] = 60;  pc[42] = 1;
	p0X[43] = 210; p0Y[43] = 20;  p1X[43] = 243; p1Y[43] = 0;   p2X[43] = 290; p2Y[43] = 35;  pc[43] = 2;
	p0X[44] = 243; p0Y[44] = 0;   p1X[44] = 320; p1Y[44] = 0;   p2X[44] = 290; p2Y[44] = 35;  pc[44] = 3;
	p0X[45] = 290; p0Y[45] = 35;  p1X[45] = 320; p1Y[45] = 0;   p2X[45] = 310; p2Y[45] = 85;  pc[45] = 4;
	p0X[46] = 310; p0Y[46] = 85;  p1X[46] = 320; p1Y[46] = 0;   p2X[46] = 320; p2Y[46] = 153; pc[46] = 5;
	p0X[47] = 290; p0Y[47] = 35;  p1X[47] = 310; p1Y[47] = 85;  p2X[47] = 295; p2Y[47] = 130; pc[47] = 6;
	p0X[48] = 260; p0Y[48] = 60;  p1X[48] = 290; p1Y[48] = 35;  p2X[48] = 295; p2Y[48] = 130; pc[48] = 7;

	// Clear image buffer.
	for (int i = 0; i < 76800; i++)
	{
		buffer_memory_write(i, 0);
	}

	// Disable interrupt.
	VGA_RASTER_disable_interrupt();

	for(int tri = 0; tri < 49; tri++){ // Iterate over eatch triangle.
		// Reset component status.
		VGA_RASTER_clear_done_status();

		// Load cordinate data.
		sample_memory_write((0<<2), *(alt_32 *) (&p0X[tri])); // x0
		sample_memory_write((1<<2), *(alt_32 *) (&p0Y[tri])); // y0
		sample_memory_write((2<<2), *(alt_32 *) (&p1X[tri])); // x1
		sample_memory_write((3<<2), *(alt_32 *) (&p1Y[tri])); // y1
		sample_memory_write((4<<2), *(alt_32 *) (&p2X[tri])); // x2
		sample_memory_write((5<<2), *(alt_32 *) (&p2Y[tri])); // y2
		sample_memory_write((6<<2), *(alt_32 *) (&pc[tri]));  // Triangle color.

		// Call components
		while (VGA_RASTER_busy()){}
		TIMER_RESET;
		TIMER_START;
		VGA_RASTER_start();
		while (!(VGA_RASTER_done())){} // Wait for component to be done.
		time[tri] = TIMER_READ; // Collect time per triangle.

		printf("Triangle nr %d time = %d\n", tri, time[tri]);
	}

	TIMER_RESET;
	TIMER_START;
	// Transfer image buffer to VGA memory.
	for(int i = 0; i < 76800; i++)
	{
		IOWR_32DIRECT(VGAG_HW_IP_0_BASE, WRITE_ADDR|(i*4), (buffer_memory_read(i)));
	}
	int trasfere_time = TIMER_READ;
	printf("Transfer buffer to VGA component time = %d\n", trasfere_time);
	printf("complete\n\n");

	return 0;
}
