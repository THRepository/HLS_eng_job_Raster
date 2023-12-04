#include "VGA_raster.h"

hls_avalon_slave_component component
void VGA_raster(hls_avalon_slave_memory_argument(7*sizeof(int)) int* tri_args,
                ihc::stream_out<compound_information,  ihc::usesPackets<true> >& hierarchical_stream)
{
    #ifdef MERGE // Test merged memory while optimization.
    hls_merge("w", "depth") comp_int_bit_size_dec w0;
    hls_merge("w", "depth") comp_int_bit_size_dec w1;
    hls_merge("w", "depth") comp_int_bit_size_dec w2;
    
    hls_merge("vec", "depth") vec_2d vec_0;
    hls_merge("vec", "depth") vec_2d vec_1;
    hls_merge("vec", "depth") vec_2d vec_2;
    #else
////// Memory
    comp_int_bit_size_dec w0;
    comp_int_bit_size_dec w1;
    comp_int_bit_size_dec w2;
    
    vec_2d vec_0;
    vec_2d vec_1;
    vec_2d vec_2;
    #endif
////// Memory

    // Read from avalon slave
    vec_0.x = tri_args[0];
    vec_0.y = tri_args[1];
    vec_1.x = tri_args[2];
    vec_1.y = tri_args[3];
    vec_2.x = tri_args[4];
    vec_2.y = tri_args[5];

    // Generate Bias for this triangle.
    w0 = bias2d(vec_0, vec_1);
    w1 = bias2d(vec_1, vec_2);
    w2 = bias2d(vec_2, vec_0);

    // Initiate information to be streamed.
    compound_information info;
    info.bias_w0 = w0;
    info.bias_w1 = w1;
    info.bias_w2 = w2;

    info.vec_0_pos.x = vec_0.x; info.vec_0_pos.y = vec_0.y;
    info.vec_1_pos.x = vec_1.x; info.vec_1_pos.y = vec_1.y;
    info.vec_2_pos.x = vec_2.x; info.vec_2_pos.y = vec_2.y;

    info.color = tri_args[6]; // Color

    // Generate bounding box.
    comp_int_bit_size_dec xmax = vmax(vmax(vec_0.x, vec_1.x), vec_2.x);
    comp_int_bit_size_dec xmin = vmin(vmin(vec_0.x, vec_1.x), vec_2.x);

    comp_int_bit_size_dec ymax = vmax(vmax(vec_0.y, vec_1.y), vec_2.y);
    comp_int_bit_size_dec ymin = vmin(vmin(vec_0.y, vec_1.y), vec_2.y);

    for(uint9 y = ymin; y < ymax;)
    {                                  // While inside of bounding box.
        for(uint9 x = xmin; x < xmax;)
        {
            info.pos.x = x;
            info.pos.y = y;
            hierarchical_stream.write(info); // Send superpixle to next component.
            x += 80;
        }   // Eatch superpixle are 80 x 60 box.
    y += 60;
    
    }
}
