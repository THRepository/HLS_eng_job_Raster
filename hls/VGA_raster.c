#include "raster_functions.h"

component
void VGA_raster( hls_avalon_slave_memory_argument(7*sizeof(comp_int_bit_size_dec)) comp_int_bit_size_dec* tri_data,
           ihc::stream_out<uint21>& to_vga_write){

    vec_2d vec_0_pos = {tri_data[0], tri_data[1]};
    vec_2d vec_1_pos = {tri_data[2], tri_data[3]};
    vec_2d vec_2_pos = {tri_data[4], tri_data[5]};
    uint8 color = tri_data[6];

    vec_2d p, q;
    vec_2d pos;
    comp_int_bit_size_dec w0, w1, w2,  
                          bias_w0, bias_w1, bias_w2;
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
        q.x = xmax_screen;
    if(q.y > ymax_screen)
        q.y = ymax_screen;
    
    bias_w0 = bias2d(vec_0_pos, vec_1_pos);
    bias_w1 = bias2d(vec_1_pos, vec_2_pos);
    bias_w2 = bias2d(vec_2_pos, vec_0_pos);
    

    for(int y = p.y; y < q.y;y++)
    {
        for(int x = p.x; x < q.x;x++)
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
            
        }
        
    }

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
    ihc::stream_in<uint21> sample_in;
    

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
    printf("%d, %d\n", tri_0_nr_elements, tri_1_nr_elements);
    int output_from_stream_0[tri_0_nr_elements];
    int output_from_stream_1[tri_1_nr_elements];

    uint3  out_data_0[100];
    uint18 out_addr_0[100];
    uint3  out_data_1[100];
    uint18 out_addr_1[100];

    printf("Start test component\n");
    comp_int_bit_size_dec in_raster_0[7] = {p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, 1};
    VGA_raster(in_raster_0, sample_out);
    for(int i = 0; i < tri_0_nr_elements; i++)
    {
        output_from_stream_0[i] = sample_out.read().to_int();
        sample_in.write(output_from_stream_0[i]);
    }

    for (int i = 0; i < tri_0_nr_elements; ++i){
        ihc_hls_enqueue_noret(&VGA_write, sample_in, out_data_0[i], out_addr_0[i]);        
    }

    ihc_hls_component_run_all(VGA_write);  
    
    comp_int_bit_size_dec in_raster_1[7] = {p3.x, p3.y, p4.x, p4.y, p5.x, p5.y, 1};
    VGA_raster(in_raster_1, sample_out);
    for(int i = 0; i < tri_1_nr_elements; i++)
    {
        output_from_stream_1[i] = sample_out.read().to_int();
        sample_in.write(output_from_stream_1[i]);
    }

    for (int i = 0; i < tri_1_nr_elements; ++i){
        ihc_hls_enqueue_noret(&VGA_write, sample_in, out_data_1[i], out_addr_1[i]);        
    }

    ihc_hls_component_run_all(VGA_write);  

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