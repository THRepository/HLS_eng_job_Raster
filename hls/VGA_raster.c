#include "raster_functions.h"

// component
// comp_int_bit_size_dec VGA_raster(char mode,
//            hls_stable_argument vec_2d vec_0_pos,
//            hls_stable_argument vec_2d vec_1_pos,
//            hls_stable_argument vec_2d vec_2_pos){
//     static vec_2d p, q;
//     vec_2d pos;
//     static comp_int_bit_size_dec w0, w1, w2, 
//                                  x, y, 
//                                  bias_w0, bias_w1, bias_w2 ;

//     if(mode == 1){
//         p.x = vmin(vmin(vec_0_pos.x, vec_1_pos.x), vec_2_pos.x);
//         p.y = vmin(vmin(vec_0_pos.y, vec_1_pos.y), vec_2_pos.y);
//         if (p.x < 0)
//             p.x = 0;
//         if(p.y < 0)
//             p.y = 0;
        
//         q.x = vmax(vmax(vec_0_pos.x, vec_1_pos.x), vec_2_pos.x) + 1;
//         q.y = vmax(vmax(vec_0_pos.y, vec_1_pos.y), vec_2_pos.y) + 1;
//         if (q.x > xmax_screen)
//             q.x = 0;
//         if(q.y > ymax_screen)
//             q.y = 0;
        
//         bias_w0 = bias2d(vec_0_pos, vec_1_pos);
//         bias_w1 = bias2d(vec_1_pos, vec_2_pos);
//         bias_w2 = bias2d(vec_2_pos, vec_0_pos);

//         x = p.x;
//         y = p.y;
//         return -1;
//     }else{
//         for(;y < q.y;)
//         {
//             for(; x < q.x;)
//             {
//                 pos.x = x;
//                 pos.y = y;
//                 w0 = cross2d(vec_0_pos, vec_1_pos, pos) + bias_w0;
//                 w1 = cross2d(vec_1_pos, vec_2_pos, pos) + bias_w1;
//                 w2 = cross2d(vec_2_pos, vec_0_pos, pos) + bias_w2;
                
//                 if((w0 >= 0) && (w1 >= 0) && (w2 >= 0))
//                 {
//                     x++;
//                     return y * xmax_screen + x - 1;
//                 }
//                 x++;
//             }
//             x = p.x;
//             y++;
//         }
//         pos.x = -1;
//         pos.y = -1;
//         return -1;
//     }
//}

typedef ihc::mm_master<comp_int_bit_size_dec, ihc::aspace<1>, ihc::awidth<4>, ihc::dwidth<16> > mm_shared_mem;

// component
// uint4 VGA_raster(hls_avalon_slave_memory_argument(7*sizeof(int)) int* triangle_cordinates,
//                                                  mm_shared_mem &shared_mem,
//                                                  ihc::stream_out<vec_2d>& input_out){
    
//     vec_2d vec_0_pos, vec_1_pos, vec_2_pos;


//     vec_0_pos.x = triangle_cordinates[0];
//     vec_0_pos.y = triangle_cordinates[1];

//     vec_1_pos.x = triangle_cordinates[2];
//     vec_1_pos.y = triangle_cordinates[3];

//     vec_2_pos.x = triangle_cordinates[4];
//     vec_2_pos.y = triangle_cordinates[5];

//     shared_mem[0] = vec_0_pos.x;
//     shared_mem[1] = vec_0_pos.y;

//     shared_mem[2] = vec_1_pos.x;
//     shared_mem[3] = vec_1_pos.y;

//     shared_mem[4] = vec_2_pos.x;
//     shared_mem[5] = vec_2_pos.y;

//     shared_mem[6] = bias2d(vec_0_pos, vec_1_pos);
//     shared_mem[7] = bias2d(vec_1_pos, vec_2_pos);
//     shared_mem[8] = bias2d(vec_2_pos, vec_0_pos);

//     vec_2d p = {0,0};
//     input_out.write(p);
//     return triangle_cordinates[6];
// }

typedef struct compound{
        vec_2d pos;
        vec_2d vec_0_pos, vec_1_pos, vec_2_pos; 
        comp_int_bit_size_dec bias_w0, bias_w1, bias_w2;
        uint4 color;
    } compound_information;

component
void VGA_raster(hls_avalon_slave_memory_argument(7*sizeof(int)) int* tri_args,
                ihc::stream_out<compound_information>& hierarchical_stream)
{
    hls_merge("w", "depth") comp_int_bit_size_dec w0;
    hls_merge("w", "depth") comp_int_bit_size_dec w1;
    hls_merge("w", "depth") comp_int_bit_size_dec w2;
    
    hls_merge("vec", "depth") vec_2d vec_0;
    hls_merge("vec", "depth") vec_2d vec_1;
    hls_merge("vec", "depth") vec_2d vec_2;

    vec_0.x = tri_args[0];
    vec_0.y = tri_args[1];
    vec_1.x = tri_args[2];
    vec_1.y = tri_args[3];
    vec_2.x = tri_args[4];
    vec_2.y = tri_args[5];

    w0 = bias2d(vec_0, vec_1);
    w1 = bias2d(vec_1, vec_2);
    w2 = bias2d(vec_2, vec_0);

    compound_information info;
    info.bias_w0 = w0;
    info.bias_w1 = w1;
    info.bias_w2 = w2;

    info.vec_0_pos.x = vec_0.x; info.vec_0_pos.y = vec_0.y;
    info.vec_1_pos.x = vec_1.x; info.vec_1_pos.y = vec_1.y;
    info.vec_2_pos.x = vec_2.x; info.vec_2_pos.y = vec_2.y;

    info.color = tri_args[6];

    for(int y = 0; y < 8; y++)
    {
        for(int x = 0; x < 8; x++)
        {
            info.pos.x = x*80;
            info.pos.y = y*60;
            hierarchical_stream.write(info);
        }
    }
}

component
void hierarchi_part(ihc::stream_in<compound_information>& hierarchical_stream,
                    ihc::stream_out<compound_information>& pixel_stream){
    compound_information info = hierarchical_stream.read();

    hls_merge("direction", "depth") int3 w_direction_top_left  = 0;
    hls_merge("direction", "depth") int3 w_direction_top_right = 0;
    hls_merge("direction", "depth") int3 w_direction_bot_left  = 0;
    hls_merge("direction", "depth") int3 w_direction_bot_right = 0;
    
    hls_merge("hw", "depth") comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    hls_merge("hw", "depth") comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    hls_merge("hw", "depth") comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;

    if(w0 >= 0)
        w_direction_top_left[0] = 1;
    if(w1 >= 0)
        w_direction_top_left[1] = 1;
    if(w2 >= 0)
        w_direction_top_left[2] = 1;
    
    info.pos.x += 79;
    w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;

    if(w0 >= 0)
        w_direction_top_right[0] = 1;
    if(w1 >= 0)
        w_direction_top_right[1] = 1;
    if(w2 >= 0)
        w_direction_top_right[2] = 1;

    info.pos.y += 59;
    w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;

    if(w0 >= 0)
        w_direction_bot_right[0] = 1;
    if(w1 >= 0)
        w_direction_bot_right[1] = 1;
    if(w2 >= 0)
        w_direction_bot_right[2] = 1;

    info.pos.x -= 79;
    w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;

    if(w0 >= 0)
        w_direction_bot_left[0] = 1;
    if(w1 >= 0)
        w_direction_bot_left[1] = 1;
    if(w2 >= 0)
        w_direction_bot_left[2] = 1;

    info.pos.y -= 59;

    if((
       (w_direction_top_left  == 7) ||
       (w_direction_top_right == 7) ||
       (w_direction_bot_left  == 7) ||
       (w_direction_bot_right == 7)
       ) || !(
       (w_direction_top_left  == w_direction_top_right) &&
       (w_direction_top_right == w_direction_bot_right) && 
       (w_direction_bot_right == w_direction_bot_left)  &&
       (w_direction_bot_left  == w_direction_top_left)))
       {
            for(uint8 y = 0; y < 60;y++)
            {
                for(uint8 x = 0; x < 80;x++)
                {
                    pixel_stream.write(info);
                    info.pos.x += 1;
                }
                info.pos.x -= 80;
                info.pos.y += 1;
            }
        
       }
       
}

component
uint23 write_pixle(ihc::stream_in<compound_information>& pixel_stream){
    compound_information info = pixel_stream.read();
    uint23 send;
    
    hls_merge("ww", "depth") comp_int_bit_size_dec w0 = cross2d(info.vec_0_pos, info.vec_1_pos, info.pos) + info.bias_w0;
    hls_merge("ww", "depth") comp_int_bit_size_dec w1 = cross2d(info.vec_1_pos, info.vec_2_pos, info.pos) + info.bias_w1;
    hls_merge("ww", "depth") comp_int_bit_size_dec w2 = cross2d(info.vec_2_pos, info.vec_0_pos, info.pos) + info.bias_w2;

    if((w0 >= 0) && (w0 >= 0) && (w0 >= 0))
    {
        send = info.pos.y * xmax_screen + info.pos.x;
        send[22] = info.color[3];
        send[21] = info.color[2];
        send[20] = info.color[1];
        send[19] = info.color[0];
    }else{
        send = 307201; // dump pixle
    }
    return send;
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
    // VGA_raster(1, p0, p1, p2);
    // char end = 0;
    // while(end == 0){
    //     output = VGA_raster(0, p0, p1, p2);
    //     if(output == -1){
    //         end = 1;
    //         printf("Found end\n\n");
    //     }else{
    //         screen_0[output % xmax_screen][output / xmax_screen] = 1;
    //     }
    // };

    // VGA_raster(1, p3, p4, p5);
    // end = 0;
    // while(end == 0){
    //     output = VGA_raster(0, p3, p4, p5);
    //     if(output == -1){
    //         end = 1;
    //         printf("Found end}\n\n");
    //     }else{
    //         screen_1[output % xmax_screen][output / xmax_screen] = 1;
    //     }
    // };

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