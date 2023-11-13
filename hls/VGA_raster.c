#include "raster_functions.h"

typedef struct compound{
        vec_2d pos;
        vec_2d vec_0_pos, vec_1_pos, vec_2_pos; 
        comp_int_bit_size_dec bias_w0, bias_w1, bias_w2;
        uint4 color;
    } compound_information;

component
void VGA_raster(hls_avalon_slave_memory_argument(7*sizeof(int)) int* tri_args,
                ihc::stream_out<compound_information,  ihc::usesPackets<true> >& hierarchical_stream)
{
    #ifdef MERGE
    hls_merge("w", "depth") comp_int_bit_size_dec w0;
    hls_merge("w", "depth") comp_int_bit_size_dec w1;
    hls_merge("w", "depth") comp_int_bit_size_dec w2;
    
    hls_merge("vec", "depth") vec_2d vec_0;
    hls_merge("vec", "depth") vec_2d vec_1;
    hls_merge("vec", "depth") vec_2d vec_2;
    #else
    comp_int_bit_size_dec w0;
    comp_int_bit_size_dec w1;
    comp_int_bit_size_dec w2;
    
    vec_2d vec_0;
    vec_2d vec_1;
    vec_2d vec_2;
    #endif

    vec_0.x = tri_args[0];
    vec_0.y = tri_args[1];
    vec_1.x = tri_args[2];
    vec_1.y = tri_args[3];
    vec_2.x = tri_args[4];
    vec_2.y = tri_args[5];

    w0 = bias2d(vec_0, vec_1);
    w1 = bias2d(vec_1, vec_2);
    w2 = bias2d(vec_2, vec_0);

    compound_information info;
    info.bias_w0 = w0;
    info.bias_w1 = w1;
    info.bias_w2 = w2;

    info.vec_0_pos.x = vec_0.x; info.vec_0_pos.y = vec_0.y;
    info.vec_1_pos.x = vec_1.x; info.vec_1_pos.y = vec_1.y;
    info.vec_2_pos.x = vec_2.x; info.vec_2_pos.y = vec_2.y;

    info.color = tri_args[6];

    comp_int_bit_size_dec xmax = vmax(vmax(vec_0.x, vec_1.x), vec_2.x);
    comp_int_bit_size_dec xmin = vmin(vmin(vec_0.x, vec_1.x), vec_2.x);

    comp_int_bit_size_dec ymax = vmax(vmax(vec_0.y, vec_1.y), vec_2.y);
    comp_int_bit_size_dec ymin = vmin(vmin(vec_0.y, vec_1.y), vec_2.y);

    for(int y = ymin; y < ymax;)
    {
        for(int x = xmin; x < xmax;)
        {
            info.pos.x = x;
            info.pos.y = y;
            hierarchical_stream.write(info);
            x += 80;
        }
    y += 60;
    
    }
}

component
void hierarchi_part(ihc::stream_in<compound_information,  ihc::usesPackets<true> >& hierarchical_stream, 
                    ihc::stream_out<compound_information, ihc::usesPackets<true> >& pixel_stream){
    compound_information info = hierarchical_stream.read();

    #ifdef MERGE
    hls_merge("direction", "depth") int3 w_direction_top_left  = 0;
    hls_merge("direction", "depth") int3 w_direction_top_right = 0;
    hls_merge("direction", "depth") int3 w_direction_bot_left  = 0;
    hls_merge("direction", "depth") int3 w_direction_bot_right = 0;
    
    hls_merge("hw", "depth") comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    hls_merge("hw", "depth") comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    hls_merge("hw", "depth") comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
    #else
    uint3 w_direction_top_left  = 0;
    uint3 w_direction_top_right = 0;
    uint3 w_direction_bot_left  = 0;
    uint3 w_direction_bot_right = 0;
    //printf("%d, %d\n", info.pos.x.to_int(), info.pos.y.to_int());
    comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
    #endif
    //printf("%d, %d, %d\n", w0.to_int(), w1.to_int(), w2.to_int());
    if(w0 >= 0)
        w_direction_top_left[0] = 1;
    else
        w_direction_top_left[0] = 0;
    if(w1 >= 0)
        w_direction_top_left[1] = 1;
    else
        w_direction_top_left[1] = 0;
    if(w2 >= 0)
        w_direction_top_left[2] = 1;
    else
        w_direction_top_left[2] = 0;
    
    info.pos.x += 79;
    w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
//printf("%d, %d, %d\n", w0.to_int(), w1.to_int(), w2.to_int());
    if(w0 >= 0)
        w_direction_top_right[0] = 1;
    else
        w_direction_top_right[0] = 0;
    if(w1 >= 0)
        w_direction_top_right[1] = 1;
    else
        w_direction_top_right[1] = 0;
    if(w2 >= 0)
        w_direction_top_right[2] = 1;
    else
        w_direction_top_right[2] = 0;

    info.pos.y += 59;
    w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
//printf("%d, %d, %d\n", w0.to_int(), w1.to_int(), w2.to_int());
    if(w0 >= 0)
        w_direction_bot_right[0] = 1;
    else
        w_direction_bot_right[0] = 0;
    if(w1 >= 0)
        w_direction_bot_right[1] = 1;
    else
        w_direction_bot_right[1] = 0;
    if(w2 >= 0)
        w_direction_bot_right[2] = 1;
    else
        w_direction_bot_right[2] = 0;

    info.pos.x -= 79;
    w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
//printf("%d, %d, %d\n", w0.to_int(), w1.to_int(), w2.to_int());
    if(w0 >= 0)
        w_direction_bot_left[0] = 1;
    else
        w_direction_bot_left[0] = 0;
    if(w1 >= 0)
        w_direction_bot_left[1] = 1;
    else
        w_direction_bot_left[1] = 0;
    if(w2 >= 0)
        w_direction_bot_left[2] = 1;
    else
        w_direction_bot_left[2] = 0;

    info.pos.y -= 59;

    //printf("%d, %d, %d, %d\n" , w_direction_top_left.to_int(), w_direction_top_right.to_int(), w_direction_bot_left.to_int(), w_direction_bot_right.to_int());
    if((
       (w_direction_top_left  == 7) ||
       (w_direction_top_right == 7) ||
       (w_direction_bot_left  == 7) ||
       (w_direction_bot_right == 7)
       ) || (
       (w_direction_top_left[0] != w_direction_top_right[0]) ||
       (w_direction_top_left[1] != w_direction_top_right[1]) ||
       (w_direction_top_left[2] != w_direction_top_right[2]) ||
       (w_direction_top_right[0] != w_direction_bot_left[0]) ||
       (w_direction_top_right[1] != w_direction_bot_left[1]) ||
       (w_direction_top_right[2] != w_direction_bot_left[2]) || 
       (w_direction_bot_left[0] != w_direction_bot_right[0]) ||
       (w_direction_bot_left[1] != w_direction_bot_right[1]) ||
       (w_direction_bot_left[2] != w_direction_bot_right[2])
       ))
       {
            for(uint8 y = 0; y < 60;y++)
            {
                for(uint8 x = 0; x < 80;x++)
                {
                    pixel_stream.write(info);
                    info.pos.x += 1;
                }
                info.pos.x -= 80;
                info.pos.y += 1;
            }
        
       }
       
}

component
int24 write_pixle(ihc::stream_in<compound_information>& pixel_stream){
    compound_information info = pixel_stream.read();
    uint23 send;
    
    #ifdef MERGE
    hls_merge("ww", "depth") comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    hls_merge("ww", "depth") comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    hls_merge("ww", "depth") comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
    #else
    comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
    #endif

    if((w0 >= 0) && (w1 >= 0) && (w2 >= 0))
    {
        send = info.pos.y * xmax_screen + info.pos.x;
        send[22] = info.color[3];
        send[21] = info.color[2];
        send[20] = info.color[1];
        send[19] = info.color[0];
    }else{
        send = -1; // dump pixle
    }
    return send;
}

component
void VGA_write(ihc::stream_in<uint21>& from_VGA_raster,
               uint3& data,
               uint18& addr){
    uint21 memory = from_VGA_raster.read();
    addr = memory.slc<3>(18);
    data = memory.slc<17>(0);
}

char old_raster(vec_2d pos,
           vec_2d vec_0_pos,
           vec_2d vec_1_pos,
           vec_2d vec_2_pos){            
    comp_int_bit_size_dec cross_0 = cross2d(vec_0_pos, vec_1_pos, pos) + bias2d(vec_0_pos, vec_1_pos);
    comp_int_bit_size_dec cross_1 = cross2d(vec_1_pos, vec_2_pos, pos) + bias2d(vec_1_pos, vec_2_pos);
    comp_int_bit_size_dec cross_2 = cross2d(vec_2_pos, vec_0_pos, pos) + bias2d(vec_2_pos, vec_0_pos);

    return cross_0 >= 0 && cross_1 >= 0 && cross_2 >= 0;
}


int main(void){
    printf("Test start!\n");

    vec_2d p0 = {10, 10};
    vec_2d p1 = {45, 170};
    vec_2d p2 = {0, 120};

    char screen_0[xmax_screen][ymax_screen];
    for(int y = 0; y < ymax_screen; y++)
    {
        for(int x = 0; x < xmax_screen; x++)
        {
            screen_0[x][y] = 0;
        }
    }

    char screen_1[xmax_screen][ymax_screen];
    for(int y = 0; y < ymax_screen; y++)
    {
        for(int x = 0; x < xmax_screen; x++)
        {
            screen_1[x][y] = 0;
        }
    }
    // char screen_1[xmax_screen][ymax_screen];
    // for(int y = 0; y < ymax_screen; y++)
    // {
    //     for(int x = 0; x < xmax_screen; x++)
    //     {
    //         screen_1[x][y] = 0;
    //     }
    // }

    // char screen_2[xmax_screen][ymax_screen];
    // for(int y = 0; y < ymax_screen; y++)
    // {
    //     for(int x = 0; x < xmax_screen; x++)
    //     {
    //         screen_2[x][y] = 0;
    //     }
    // }

    // char screen_3[xmax_screen][ymax_screen];
    // for(int y = 0; y < ymax_screen; y++)
    // {
    //     for(int x = 0; x < xmax_screen; x++)
    //     {
    //         screen_3[x][y] = 0;
    //     }
    // }

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

    printf("\nStart test hierarchi_part.\n");

    ihc::stream_in< compound_information,  ihc::usesPackets<true> > hierarchi_in_stream;
    ihc::stream_out< compound_information, ihc::usesPackets<true> > hierarchi_out_stream;
    ihc::stream_in< compound_information> write_pixel_in_stream;
    //compound_information output_memory_hierarchi[10000];
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
            write_pixel_in_stream.write(output);
            nr_of_outputs_hierarchi += 1;
        }
            
    }
    printf("hierarchi_part done with %d outputs.\n", nr_of_outputs_hierarchi);

    printf("\nStart test write_pixel.\n");

    int24 pixle_output;
    uint4 color_smal;
    int color_big;

    int xpos, ypos;

    int valid_pixles = 0;
    static int counter = 0;
    for(int i = 0; i < nr_of_outputs_hierarchi; i++)
    {
        pixle_output = write_pixle(write_pixel_in_stream);
        if(pixle_output != 8388607){
            valid_pixles += 1;
            color_smal[3] = pixle_output[22];
            pixle_output[22] = 0;
            color_smal[2] = pixle_output[21];
            pixle_output[21] = 0;
            color_smal[1] = pixle_output[20];
            pixle_output[20] = 0;
            color_smal[0] = pixle_output[19];
            pixle_output[19] = 0;
            color_big = color_smal.to_int();

            xpos = pixle_output.to_int() % xmax_screen; 
            ypos = (pixle_output.to_int() - xpos) / xmax_screen; 
            screen_0[xpos][ypos] = (char)color_big;
        }
    }

    printf("write_pixle complete with %d valid pixles", valid_pixles);


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

    for(int y = 0; y < 480; y++)
    {
        for(int x = 0; x < 640; x++)
        {
            if((screen_0[x][y] == 1) && (screen_1[x][y] == 1))
            {
                correct += 1;
            }else if((screen_0[x][y] == 0) && (screen_1[x][y] == 1))
            {
                error_missing += 1;
            }else if((screen_0[x][y] == 0) && (screen_1[x][y] == 1))
            {
                error_extra += 1;
            }
        }
    }

    printf("Emulation complete with %d correct pixels, %d missing pixles and %d extra pixles!\n", correct, error_missing, error_extra);
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
            if ((screen_0[x][y] > 0) && (tri_1_bool > 0))
            {
                printf("*");
            }else if (tri_1_bool > 0)
            {
                printf("X");
            }else if (screen_0[x][y] > 0)
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
            if ((screen_0[x][y] > 0) && (tri_1_bool > 0))
            {
                printf("*");
            }else if (tri_1_bool > 0)
            {
                printf("X");
            }else if (screen_0[x][y] > 0)
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
            if ((screen_0[x][y] > 0) && (tri_1_bool > 0))
            {
                printf("*");
            }else if (tri_1_bool > 0)
            {
                printf("X");
            }else if (screen_0[x][y] > 0)
            {
                printf("G");
            }
            else{
                printf(" ");
            }
        }
        printf(" |\n");
    }
    
    return 0;
}