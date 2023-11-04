#include "raster_functions.h"

component
vec_2d VGA_raster(char mode,
           vec_2d vec_0_pos,
           vec_2d vec_1_pos,
           vec_2d vec_2_pos){
    vec_2d pos;
    static comp_int_bit_size_dec delta_col_0, delta_col_1, delta_col_2,
                                 delta_row_0, delta_row_1, delta_row_2,
                                 w0, w0_ini, w1, w1_ini, w2, w2_ini,
                                 x, y;

    
    if(mode == 1){
        pos.x = vmin(vmin(vec_0_pos.x, vec_1_pos.x), vec_2_pos.x);
        pos.y = vmin(vmin(vec_0_pos.y, vec_1_pos.y), vec_2_pos.y);

        delta_col_0 = (vec_1_pos.y - vec_2_pos.y);
        delta_col_1 = (vec_2_pos.y - vec_0_pos.y);
        delta_col_2 = (vec_0_pos.y - vec_1_pos.y);

        delta_row_0 = (vec_1_pos.x - vec_2_pos.x);
        delta_row_1 = (vec_2_pos.x - vec_0_pos.x);
        delta_row_2 = (vec_0_pos.x - vec_1_pos.x);

        w0_ini = cross2d(vec_1_pos, vec_2_pos, pos) + bias2d(vec_1_pos, vec_2_pos);
        w0 = 0;
        w1_ini = cross2d(vec_2_pos, vec_0_pos, pos) + bias2d(vec_2_pos, vec_0_pos);
        w1 = 0;
        w2_ini = cross2d(vec_0_pos, vec_1_pos, pos) + bias2d(vec_0_pos, vec_1_pos);
        w2 = 0;

        x = -1;
        y = 0;
        return pos;
    }else{
        for(;y < ymax_screen;)
        {
            for(; x < xmax_screen;)
            {
                w0 += delta_col_0;
                w1 += delta_col_1;
                w2 += delta_col_2;
                x++;
                if(w0 >= 0 && w1 >= 0 && w2 >= 0)
                    pos.x = x;
                    pos.y = y;
                    return pos;
            }
            x = 0;
            y++;
            delta_col_0 += delta_row_0;
            delta_col_1 += delta_row_1;
            delta_col_2 += delta_row_2;
            w0 = delta_col_0;
            w1 = delta_col_1;
            w2 = delta_col_2;
        }
        pos.x = -1;
        pos.y = -1;
        return pos;
    }
    

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
            tri_0_bool = old_raster(pos, p0, p1, p2);
            tri_1_bool = old_raster(pos, p3, p4, p5);
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
            tri_0_bool = old_raster(pos, p0, p1, p2);
            tri_1_bool = old_raster(pos, p3, p4, p5);
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