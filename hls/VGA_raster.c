#include "raster_functions.h"

component
comp_int_bit_size_dec VGA_raster(char mode,
           vec_2d vec_0_pos,
           vec_2d vec_1_pos,
           vec_2d vec_2_pos){
    static vec_2d p, q;
    vec_2d pos;
    static comp_int_bit_size_dec w0, w1, w2, 
                                 x, y, 
                                 bias_w0, bias_w1, bias_w2 ;

    if(mode == 1){
        p.x = vmin(vmin(vec_0_pos.x, vec_1_pos.x), vec_2_pos.x);
        p.y = vmin(vmin(vec_0_pos.y, vec_1_pos.y), vec_2_pos.y);
        if (p.x < 0)
            p.x = 0;
        if(p.y < 0)
            p.y = 0;
        
        q.x = vmax(vmax(vec_0_pos.x, vec_1_pos.x), vec_2_pos.x) + 1;
        q.y = vmax(vmax(vec_0_pos.y, vec_1_pos.y), vec_2_pos.y) + 1;
        if (q.x > xmax_screen)
            q.x = 0;
        if(q.y > ymax_screen)
            q.y = 0;
        
        bias_w0 = bias2d(vec_0_pos, vec_1_pos);
        bias_w1 = bias2d(vec_1_pos, vec_2_pos);
        bias_w2 = bias2d(vec_2_pos, vec_0_pos);

        x = p.x;
        y = p.y;
        return -1;
    }else{
        for(;y < q.y;)
        {
            for(; x < q.x;)
            {
                pos.x = x;
                pos.y = y;
                w0 = cross2d(vec_0_pos, vec_1_pos, pos) + bias_w0;
                w1 = cross2d(vec_1_pos, vec_2_pos, pos) + bias_w1;
                w2 = cross2d(vec_2_pos, vec_0_pos, pos) + bias_w2;
                
                if((w0 >= 0) && (w1 >= 0) && (w2 >= 0))
                {
                    x++;
                    return y * xmax_screen + x - 1;
                }
                x++;
            }
            x = p.x;
            y++;
        }
        pos.x = -1;
        pos.y = -1;
        return -1;
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

    comp_int_bit_size_dec output;

    char screen_0[xmax_screen][ymax_screen];
    for(int y = 0; y < ymax_screen; y++)
    {
        for(int x = 0; x < xmax_screen; x++)
        {
            screen_0[x][y] = 0;
        }
    }
    char screen_1[xmax_screen][ymax_screen];
    for(int y = 0; y < ymax_screen; y++)
    {
        for(int x = 0; x < xmax_screen; x++)
        {
            screen_1[x][y] = 0;
        }
    }

    printf("Start test component\n");
    VGA_raster(1, p0, p1, p2);
    char end = 0;
    while(end == 0){
        output = VGA_raster(0, p0, p1, p2);
        if(output == -1){
            end = 1;
            printf("Found end\n\n");
        }else{
            screen_0[output % xmax_screen][output / xmax_screen] = 1;
        }
    };

    VGA_raster(1, p3, p4, p5);
    end = 0;
    while(end == 0){
        output = VGA_raster(0, p3, p4, p5);
        if(output == -1){
            end = 1;
            printf("Found end}\n\n");
        }else{
            screen_1[output % xmax_screen][output / xmax_screen] = 1;
        }
    };

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
            if(screen_0[x][y] == 1)
            {
                printf("G");
            }else if(screen_1[x][y] == 1)
            {
                printf("U");
            }else if(tri_0_bool == 1) 
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
            if ((screen_0[x][y] == 1) && (screen_1[x][y] == 1))
            {
                printf("*");
            }else{
                printf(" ");
            }
        }
        printf(" |\n");
    }

    return 0;
}