#include "raster_functions.h"


typedef ihc::mm_master<comp_int_bit_size_dec, ihc::aspace<1>, ihc::awidth<4>, ihc::dwidth<16> > mm_shared_mem;

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

component
void VGA_write(ihc::stream_in<uint21>& from_VGA_raster,
               uint3& data,
               uint18& addr){
    uint21 memory = from_VGA_raster.read();
    addr = memory.slc<3>(18);
    data = memory.slc<17>(0);
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