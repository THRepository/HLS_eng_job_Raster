#ifndef HIERARCHI_PART_H_
#define HIERARCHI_PART_H_

// Shared includes and defines. Stopps multiples.
#ifndef PROJECT_SHARED_INC_DEF_FIR_FILTER_
#define PROJECT_SHARED_INC_DEF_FIR_FILTER_
    #include "HLS/hls.h"
    #include "HLS/ac_int.h"
    #include <stdio.h>
    
    typedef int15 comp_int_bit_size_dec;

    typedef struct vec2{
        comp_int_bit_size_dec x;
        comp_int_bit_size_dec y;
    } vec_2d;

    typedef struct compound{
        vec_2d pos;
        vec_2d vec_0_pos, vec_1_pos, vec_2_pos; 
        comp_int_bit_size_dec bias_w0, bias_w1, bias_w2;
        uint4 color;
    } compound_information;

    #define xmax_screen 320
    #define ymax_screen 240
#endif

#include "raster_functions.h"

hls_always_run_component component
void hierarchi_part(ihc::stream_in<compound_information,  ihc::usesPackets<true> >& hierarchical_stream, 
                    ihc::stream_out<compound_information, ihc::usesPackets<true> >& pixel_stream);

#endif