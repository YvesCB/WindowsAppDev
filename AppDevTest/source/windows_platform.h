#ifndef WINDOWS_PLATFORM
#define WINDOWS_PLATFORM

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
