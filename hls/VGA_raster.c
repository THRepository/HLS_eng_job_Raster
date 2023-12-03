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

hls_always_run_component component
void hierarchi_part(ihc::stream_in<compound_information,  ihc::usesPackets<true> >& hierarchical_stream, 
                    ihc::stream_out<compound_information, ihc::usesPackets<true> >& pixel_stream){
    bool available;
    compound_information info = hierarchical_stream.tryRead(available);
    if(available) // If available new data do work. 
    {

        #ifdef MERGE
        hls_merge("direction", "depth") int3 w_direction_top_left  = 0;
        hls_merge("direction", "depth") int3 w_direction_top_right = 0;
        hls_merge("direction", "depth") int3 w_direction_bot_left  = 0;
        hls_merge("direction", "depth") int3 w_direction_bot_right = 0;
        
        hls_merge("hw", "depth") comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
        hls_merge("hw", "depth") comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
        hls_merge("hw", "depth") comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
        #else
        uint3 w_direction_top_left  = 0; // Memory to check if left or right from vector.
        uint3 w_direction_top_right = 0; // Using only 1 bit to represent this.
        uint3 w_direction_bot_left  = 0;
        uint3 w_direction_bot_right = 0;
        
        // Check where top left pixle are depending on triangle.
        comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
        comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
        comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
        #endif
        
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
        
        // Check where top right pixle are depending on triangle.
        info.pos.x += 79;
        w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
        w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
        w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;

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

        // Check where bot right pixle are depending on triangle.
        info.pos.y += 59;
        w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
        w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
        w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
    
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

        // Check where bot left pixle are depending on triangle.
        info.pos.x -= 79;
        w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
        w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
        w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
    
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

        // If all corners are on the same side and no corner is inside of the triangle
        // do nothing. Else send eatch pixel of this superpixel to next component.
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
                {                               // Eatch pixle in superpixel.
                    for(uint8 x = 0; x < 80;x++)
                    {
                        pixel_stream.write(info); // Send
                        info.pos.x += 1; // Increment with 1 is cheeper then using x in for loop.
                    }
                    info.pos.x -= 80;
                    info.pos.y += 1;
                }
            
        }
    }
       
}

hls_always_run_component component
void write_pixle(ihc::stream_in<compound_information,  ihc::usesPackets<true> >& pixel_stream,
                 ihc::mm_master<char, ihc::aspace<1>, ihc::awidth<17>, ihc::dwidth<8>, ihc::latency<1>, ihc::readwrite_mode<writeonly>, ihc::waitrequest<true> > &VGA_screen_memory){
    bool available;
    compound_information info = pixel_stream.tryRead(available);
    if(available) // If available new data do work.  
    {
        #ifdef MERGE
        hls_merge("ww", "depth") comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
        hls_merge("ww", "depth") comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
        hls_merge("ww", "depth") comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
        #else
        comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
        comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
        comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;
        #endif
    
        if((w0 >= 0) && (w1 >= 0) && (w2 >= 0)) // If inside of triangle.
        {
            VGA_screen_memory[info.pos.y * xmax_screen + info.pos.x] = info.color; // Set pixel to color in screen memory.
        }
    }
}

