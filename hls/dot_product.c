#include "HLS/hls.h"
#include "HLS/ac_int.h"

typedef int16 comp_int_bit_size_dec;

typedef struct vec2{
    comp_int_bit_size_dec x;
    comp_int_bit_size_dec y;
} vec_2d;


// cross vector p0 X p1.
component
comp_int_bit_size_dec cross2d(vec_2d ref, vec_2d p0, vec_2d p1){
    //  | p0x |   | p1x |
    //  |     | X |     |
    //  | p0y |   | p1y |
    return (p0.x - ref.x) * (p1.y - ref.y) - (p0.y - ref.y) * (p1.x - ref.x);
}


int main(){
    comp_int_bit_size_dec ret;

    vec_2d p_ref = {0, 0};

    vec_2d p0 = {50, 50};

    vec_2d p1 = {20, 25};

    ret = cross2d(p_ref, p0, p1);
    printf("result: %d\n", ret.to_int());

    p1.x = 30;

    ret = cross2d(p_ref, p0, p1);
    printf("result: %d\n", ret.to_int());

    return 0;
}