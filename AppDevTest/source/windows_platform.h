#ifndef WINDOWS_PLATFORM
#define WINDOWS_PLATFORM

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}

#define global_variable static
#define internal static

struct StateInfo
{
    float parab_h;
    float parab_k;
    float parab_a;
    
    int showGrid = 0;
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
