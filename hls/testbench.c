#include "VGA_raster.h"

// Software implementation to check if pixel is inside of triangle.
char old_raster(vec_2d pos,
           vec_2d vec_0_pos,
           vec_2d vec_1_pos,
           vec_2d vec_2_pos){            
    int cross_0 = cross2d(vec_0_pos, vec_1_pos, pos) + bias2d(vec_0_pos, vec_1_pos);
    int cross_1 = cross2d(vec_1_pos, vec_2_pos, pos) + bias2d(vec_1_pos, vec_2_pos);
    int cross_2 = cross2d(vec_2_pos, vec_0_pos, pos) + bias2d(vec_2_pos, vec_0_pos);

    return cross_0 >= 0 && cross_1 >= 0 && cross_2 >= 0;
}


int main(void){
    printf("Test start!\n");

    // Cordinates of test triangle.
    vec_2d p0 = {10, 10};
    vec_2d p1 = {45, 170};
    vec_2d p2 = {0, 120};

    char screen_0[xmax_screen * ymax_screen];
    ihc::mm_master<char, ihc::aspace<1>, ihc::awidth<17>, ihc::dwidth<8>, ihc::latency<1>, ihc::readwrite_mode<writeonly>, ihc::waitrequest<true> > m_tb(screen_0, sizeof(char)*320*240);

    for(int y = 0; y < ymax_screen; y++)
    {
        for(int x = 0; x < xmax_screen; x++)
        {
            screen_0[x + xmax_screen * y] = 0; // Clear screen memory for component.
        }
    }

    char screen_1[xmax_screen][ymax_screen];
    for(int y = 0; y < ymax_screen; y++)
    {
        for(int x = 0; x < xmax_screen; x++)
        {
            screen_1[x][y] = 0; // Clear screen memory for software.
        }
    }


////////// Test VGA_raster component
//// Insert test triangle vectors. Followed by read all avaible superpixles. Check how many.

    printf("\nStart test VGA_raster component\n");

    compound_information vga_raster_stream_memory[64];
    compound_information output;
    ihc::stream_out<compound_information,  ihc::usesPackets<true> > vga_raster_output_stream;
    int tri_args[7] = {p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, 1};

    VGA_raster(tri_args, vga_raster_output_stream);
    bool available;
    int nr_of_outputs_vga_raster = 0;
    for(int i = 0; i < 64; i++)
    {
        output = vga_raster_output_stream.tryRead(available);
        if(available)
        {
            vga_raster_stream_memory[nr_of_outputs_vga_raster] = output;
            nr_of_outputs_vga_raster += 1;
        }
    }
    printf("First triangle %d super pixles generated.\n", nr_of_outputs_vga_raster);



////////// Test hierarchi_part component
//// Stream in superpixles. Then read eatch pixle adress that comes out. Check how many.

    printf("\nStart test hierarchi_part.\n");

    ihc::stream_in< compound_information,  ihc::usesPackets<true> > hierarchi_in_stream;
    ihc::stream_out< compound_information, ihc::usesPackets<true> > hierarchi_out_stream;
    ihc::stream_in< compound_information,  ihc::usesPackets<true> > write_pixel_in_stream;
    
    for(int i = 0; i < nr_of_outputs_vga_raster; i++)
    {
        hierarchi_in_stream.write(vga_raster_stream_memory[i]);
    }
    for(int i = 0; i < nr_of_outputs_vga_raster; i++)
    {
         ihc_hls_enqueue_noret(&hierarchi_part, hierarchi_in_stream, hierarchi_out_stream);
    }
    ihc_hls_component_run_all(hierarchi_part);
    
    bool status;
    int nr_of_outputs_hierarchi = 0;
    for(int i = 0; i < 100000; i++)
    {
        output = hierarchi_out_stream.tryRead(available);
        if(available)
        {
            write_pixel_in_stream.write(output); // Stream out to next component direcly while output exists.
            nr_of_outputs_hierarchi += 1;
        }
            
    }
    printf("hierarchi_part done with %d outputs.\n", nr_of_outputs_hierarchi);
    

////////// Test write_pixel component
//// Stream in pixels. Eatch pixle with it's color inserted in screen memory.

    printf("\nStart test write_pixel.\n");

     for(int i = 0; i < nr_of_outputs_hierarchi; i++)
    {
         ihc_hls_enqueue_noret(&write_pixle, write_pixel_in_stream, m_tb);
    }
    ihc_hls_component_run_all(write_pixle);

    int24 pixle_output;
    uint4 color_smal;
    int color_big;

    int xpos, ypos;



////////// Test if correct pixel positions on screen memory.
//// In bounding box, check if eatch pixle are out or in triangle. Save in software screen memory.

    printf("\nStart test if all pixles are in correct position.");
    int xmax, xmin, ymax, ymin;

    if(p0.x > p1.x)
    {
        xmax = p0.x;
        xmin = p1.x;
    }else
    {
        xmax = p1.x;
        xmin = p0.x; 
    }
    if(xmax < p2.x)
    {
        xmax = p2.x;
    }
    if(xmin > p2.x)
    {
        xmin = p2.x;
    }

    if(p0.y > p1.y)
    {
        ymax = p0.y;
        ymin = p1.y;
    }else
    {
        ymax = p1.y;
        ymin = p0.y; 
    }
    if(ymax < p2.y)
    {
        ymax = p2.y;
    }
    if(ymin > p2.y)
    {
        ymin = p2.y;
    }

    for(int y = ymin; y < ymax; y++)
    {
        for(int x = xmin; x < xmax; x++)
        {
            vec_2d pos = {x, y};
            screen_1[x][y] = old_raster(pos, p0, p1, p2);
        }
    }

    int correct = 0;
    int error_missing = 0;
    int error_extra = 0;
    // Compare both memories and collect errors.
    for(int y = 0; y < ymax_screen; y++)
    {
        for(int x = 0; x < xmax_screen; x++)
        {
            if((screen_0[x + xmax_screen * y] == 1) && (screen_1[x][y] == 1))
            {
                correct += 1;
            }else if((screen_0[x + xmax_screen * y] == 0) && (screen_1[x][y] == 1))
            {
                error_missing += 1;
            }else if((screen_0[x + xmax_screen * y] == 0) && (screen_1[x][y] == 1))
            {
                error_extra += 1;
            }
        }
    }
    


////////// Print out result triangle in console.
//// Draw with help of ASCII charecters in the console. 
//// From top left corner 3 superpixles down are printed where triangle are drawn.

    vec_2d pos;

    printf("\n");
    for(int y = 0; y < 60; y++)
    {
        printf("| ");
        for(int x = 0; x < 80; x++)
        {
            pos.x = x;
            pos.y = y;
            char tri_1_bool = old_raster(pos, p0, p1, p2);
            if ((screen_0[x + xmax_screen * y] > 0) && (tri_1_bool > 0)) // Show errors if there are any.
            {
                printf("*");
            }else if (tri_1_bool > 0)
            {
                printf("X");
            }else if (screen_0[x + xmax_screen * y] > 0)
            {
                printf("G");
            }
            else{
                printf(" ");
            }
        }
        printf(" |\n");
    }

    printf("\n");
    for(int y = 60; y < 120; y++)
    {
        printf("| ");
        for(int x = 0; x < 80; x++)
        {
            pos.x = x;
            pos.y = y;
            char tri_1_bool = old_raster(pos, p0, p1, p2);
            if ((screen_0[x + xmax_screen * y] > 0) && (tri_1_bool > 0))
            {
                printf("*");
            }else if (tri_1_bool > 0)
            {
                printf("X");
            }else if (screen_0[x + xmax_screen * y] > 0)
            {
                printf("G");
            }
            else{
                printf(" ");
            }
        }
        printf(" |\n");
    }

    printf("\n");
    for(int y = 120; y < 180; y++)
    {
        printf("| ");
        for(int x = 0; x < 80; x++)
        {
            pos.x = x;
            pos.y = y;
            char tri_1_bool = old_raster(pos, p0, p1, p2);
            if ((screen_0[x + xmax_screen * y]) && (tri_1_bool > 0))
            {
                printf("*");
            }else if (tri_1_bool > 0)
            {
                printf("X");
            }else if (screen_0[x + xmax_screen * y] > 0)
            {
                printf("G");
            }
            else{
                printf(" ");
            }
        }
        printf(" |\n");
    }

    // Results.
    printf("\nEmulation complete with %d correct pixels, %d missing pixles and %d extra pixles!\n", correct, error_missing, error_extra);
    
    return 0;
}