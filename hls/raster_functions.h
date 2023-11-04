#ifndef RASTER_FUNCTIONS_H_
#define RASTER_FUNCTIONS_H_

// Shared includes nad defines. Stopps multiples.
#ifndef PROJECT_SHARED_INC_DEF_FIR_FILTER_
#define PROJECT_SHARED_INC_DEF_FIR_FILTER_
    #include "HLS/hls.h"
    #include "HLS/ac_int.h"
    #include <stdio.h>
    
    typedef int16 comp_int_bit_size_dec;

    typedef struct vec2{
        comp_int_bit_size_dec x;
        comp_int_bit_size_dec y;
    } vec_2d;
#endif

#define xmax_screen 640
#define ymax_screen 480

comp_int_bit_size_dec cross2d(vec_2d ref0, vec_2d p0, vec_2d p1);
int2 bias2d(vec_2d p0, vec_2d p1);
comp_int_bit_size_dec vmin(comp_int_bit_size_dec x, comp_int_bit_size_dec y);
comp_int_bit_size_dec vmax(comp_int_bit_size_dec x, comp_int_bit_size_dec y);
#endif