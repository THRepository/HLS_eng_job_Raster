#include "hierarchi_part.h"

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