#include "HLS/hls.h"
#include "HLS/ac_int.h"

typedef int16 comp_int_bit_size_dec;

typedef struct vec2{
    comp_int_bit_size_dec x;
    comp_int_bit_size_dec y;
} vec_2d;


// cross vector p0 X p1.
comp_int_bit_size_dec cross2d(vec_2d ref0, vec_2d p0, vec_2d p1){
    //  | p0x |   | p1x |
    //  |     | X |     |
    //  | p0y |   | p1y |
    return (p0.x - ref0.x) * (p1.y - ref0.y) - (p0.y - ref0.y) * (p1.x - ref0.x);
}

int2 bias2d(vec_2d p0, vec_2d p1){
    vec_2d edge = {p1.x - p0.x, p1.y - p0.x};
    if (( (edge.y == 0) && (edge.x > 0) ) ||
        (edge.y < 0)){
        return 0;
    }else{
        return -1;
    }
}

