#include "write_pixle.h"

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