#include "HLS/hls.h"
#include "HLS/ac_int.h"


component 
void write_test(ihc::mm_master<uint4, ihc::aspace<1>, ihc::awidth<16>, ihc::dwidth<8> > &VGA_screen_memory)
{
    for(int y = 0; y < 10; y++)
    {
        for(int x = 0; x < 10; x++)
        {
            VGA_screen_memory[(y + 100) * 320 + x + 100] = 3;
        }
    }
}






int main()
{

    int screen_0[320][240];
    int screen_1[320][240];

    for(int y = 0; y < 240; y++)
    {
        for(int x = 0; x < 320; x++)
        {
            screen_0[x][y] = 0;
            screen_1[x][y] = 0;
        }
    }

    for(int y = 0; y < 10; y++)
    {
        for(int x = 0; x < 10; x++)
        {
            screen_0[x + 100][(y + 100) * 320] = 3;
        }
    }

    ihc::mm_master<uint4, ihc::aspace<1>, ihc::awidth<16>, ihc::dwidth<8> > m_tb(screen_1, sizeof(uint4)*320*240);

    write_test(m_tb);

    int err = 0;
    for(int y = 0; y < 10; y++)
    {
        for(int x = 0; x < 10; x++)
        {
            if(screen_0[x][y] != screen_1[x][y])
                err += 1;
        }
    }

    printf("Test complete with %d errors!\n", err);
    return 0;
}