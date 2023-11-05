#include "raster_functions.h"

component
void VGA_raster( hls_stable_argument uint8 color,
           hls_stable_argument vec_2d vec_0_pos,
           hls_stable_argument vec_2d vec_1_pos,
           hls_stable_argument vec_2d vec_2_pos,
           ihc::stream_out<uint21>& to_vga_write){
    static vec_2d p, q;
    vec_2d pos;
    static comp_int_bit_size_dec w0, w1, w2, 
                                 x, y, 
                                 bias_w0, bias_w1, bias_w2 ;
    uint21 output;

    p.x = vmin(vmin(vec_0_pos.x, vec_1_pos.x), vec_2_pos.x);
    p.y = vmin(vmin(vec_0_pos.y, vec_1_pos.y), vec_2_pos.y);
    if (p.x < 0)
        p.x = 0;
    if(p.y < 0)
        p.y = 0;
    
    q.x = vmax(vmax(vec_0_pos.x, vec_1_pos.x), vec_2_pos.x) + 1;
    q.y = vmax(vmax(vec_0_pos.y, vec_1_pos.y), vec_2_pos.y) + 1;
    if (q.x > xmax_screen)
        q.x = 0;
    if(q.y > ymax_screen)
        q.y = 0;
    
    bias_w0 = bias2d(vec_0_pos, vec_1_pos);
    bias_w1 = bias2d(vec_1_pos, vec_2_pos);
    bias_w2 = bias2d(vec_2_pos, vec_0_pos);

    x = p.x;
    y = p.y;
    

    for(;y < q.y;)
    {
        for(; x < q.x;)
        {
            pos.x = x;
            pos.y = y;
            w0 = cross2d(vec_0_pos, vec_1_pos, pos) + bias_w0;
            w1 = cross2d(vec_1_pos, vec_2_pos, pos) + bias_w1;
            w2 = cross2d(vec_2_pos, vec_0_pos, pos) + bias_w2;
            
            if((w0 >= 0) && (w1 >= 0) && (w2 >= 0))
            {
                output = y * xmax_screen + x;
                output[20] = color[2];
                output[19] = color[1];
                output[18] = color[0];
                to_vga_write.write(output);
            }
            x++;
        }
        x = p.x;
        y++;
    }

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

int main(){
    printf("Test start!\n");
    vec_2d p0 = {5, 5};
    vec_2d p1 = {30, 20};
    vec_2d p2 = {10, 14};

    vec_2d p3 = {5, 5};
    vec_2d p4 = {20, 3};
    vec_2d p5 = {30, 20};

    comp_int_bit_size_dec output;
    ihc::stream_out<uint21> sample_out;
    

    int tri_0_nr_elements = 0;
    int tri_1_nr_elements = 0;

    char tri_0_bool;
    char tri_1_bool;
    vec_2d pos;

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

    for(int y = 0; y < 30; y++)
    {
        printf("| ");
        for(int x = 0; x < 60; x++)
        {
            pos.x = x;
            pos.y = y;
            tri_0_bool = old_raster(pos, p0, p1, p2);
            tri_1_bool = old_raster(pos, p3, p4, p5);
            if(tri_0_bool == 1) 
            {
                tri_0_nr_elements++;
            }
            if (tri_1_bool == 1)
            {
                tri_1_nr_elements++;
            }
        }
    }
    int output_from_stream_0[tri_0_nr_elements];
    int output_from_stream_1[tri_1_nr_elements];

    printf("Start test component\n");

    VGA_raster(1, p0, p1, p2, sample_out);
    for(int i = 0; i < tri_0_nr_elements; i++)
    {
        output_from_stream_0[i] = sample_out.read().to_int();
    }
    
    VGA_raster(1, p3, p4, p5, sample_out);
    for(int i = 0; i < tri_0_nr_elements; i++)
    {
        output_from_stream_1[i] = sample_out.read().to_int();
    }

    for(int y = 0; y < 30; y++)
    {
        printf("| ");
        for(int x = 0; x < 60; x++)
        {
            pos.x = x;
            pos.y = y;
            tri_0_bool = old_raster(pos, p0, p1, p2);
            tri_1_bool = old_raster(pos, p3, p4, p5);
            if(screen_0[x][y] == 1)
            {
                printf("G");
            }else if(screen_1[x][y] == 1)
            {
                printf("U");
            }else if(tri_0_bool == 1) 
            {
                printf("X");
            }else if (tri_1_bool == 1)
            {
                printf("O");
            }else
            {
                printf(" ");
            }
        }
        printf(" |\n");
    }
    printf("\n");
    for(int y = 0; y < 30; y++)
    {
        printf("| ");
        for(int x = 0; x < 60; x++)
        {
            pos.x = x;
            pos.y = y;
            tri_0_bool = old_raster(pos, p0, p1, p2);
            tri_1_bool = old_raster(pos, p3, p4, p5);
            if ((screen_0[x][y] == 1) && (screen_1[x][y] == 1))
            {
                printf("*");
            }else{
                printf(" ");
            }
        }
        printf(" |\n");
    }

    return 0;
}