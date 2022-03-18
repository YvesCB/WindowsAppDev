#ifndef WINDOWS_PLATFORM
#define WINDOWS_PLATFORM

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

struct StateInfo
{
    int rect_pos_x;
    int rect_pos_y;
    int rect_width;
    int rect_height;

    float circ_pos_x;
    float circ_pos_y;
    float circ_r;

    float parab_h;
    float parab_k;
    float parab_a;
};

struct OffscreenBuffer
{
    // NOTE(casey): Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
    BITMAPINFO info;
    void *memory;
    int width;
    int height;
    int pitch;
    int bytesPerPixel;
};

struct Dimensions
{
    int width;
    int height;
};

#endif
