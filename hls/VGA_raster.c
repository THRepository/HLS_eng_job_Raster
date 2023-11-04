#include "raster_functions.h"

component
char VGA_raster(vec_2d pos,
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

    vec_2d pos;

    char tri_0_bool;
    char tri_1_bool;

    for(int y = 0; y < 30; y++)
    {
        printf("| ");
        for(int x = 0; x < 60; x++)
        {
            pos.x = x;
            pos.y = y;
            tri_0_bool = VGA_raster(pos, p0, p1, p2);
            tri_1_bool = VGA_raster(pos, p3, p4, p5);
            if (tri_0_bool == 1) 
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
            tri_0_bool = VGA_raster(pos, p0, p1, p2);
            tri_1_bool = VGA_raster(pos, p3, p4, p5);
            if ((tri_0_bool == 1) && (tri_1_bool == 1)) 
            {
                printf("*");
            }else
            {
                printf(" ");
            }
        }
        printf(" |\n");
    }

    return 0;
}