#if !defined ENGINE_H
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Yves $
   $Notice: This is experimental code $
   ======================================================================== */

/*
  ENGINE_INTERNAL:
    0 - Build for public release
    1 - Build for developer only

  ENGINE_SLOW:
    0 - Not slow code allowed!
    1 - Slow code welcome.
*/

#include "vector_math.h"

#if ENGINE_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

#define EPS 0.00001f

#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

inline uint32
SafeTruncateUInt64(uint64 Value)
{
    Assert(Value <= 0xFFFFFFFF);
    uint32 Result = (uint32)Value;
    return(Result);
}

/*
  NOTE: Services that the platform layer provides to the game
*/
// FOUR THINGS - timing, controller/keyboard input, bitmap buffer to use, sound buffer to use

struct offscreen_buffer
{
    // NOTE: Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
    void *Memory;
    int Width;
    int Height;
    int Pitch;
};

struct button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct keyboard_input
{
    union
    {
        button_state Buttons[12];
        struct
        {
            button_state MoveUp;
            button_state MoveDown;
            button_state MoveLeft;
            button_state MoveRight;
            
            button_state ActionUp;
            button_state ActionDown;
            button_state ActionLeft;
            button_state ActionRight;
            
            button_state LeftShoulder;
            button_state RightShoulder;

            button_state Back;
            button_state Start;

            // NOTE: All buttons must be added above this line
            
            button_state Terminator;
        };
    };
};

struct triangle
{
    v3f T0;
    v3f T1;
    v3f T2;

    v3f N;
    float D;

    int32 Color;
};

struct geometry
{
    struct triangle *Triangles;    
};

struct engine_state
{
    v3f CameraPosition;
    v3f CameraViewVector;

    v3f CameraToScreenVector;
    v3f CameraUpVector;

    struct geometry SceneGeometry;
};

struct engine_memory
{
    bool32 IsInitialized;

    uint64 PermanentStorageSize;
    void *PermanentStorage; // NOTE: REQUIRED to be cleared to zero at startup

    uint64 TransientStorageSize;
    void *TransientStorage; // NOTE: REQUIRED to be cleared to zero at startup
};

internal void UpdateAndRender(engine_memory *Memory, keyboard_input *Input, offscreen_buffer *Buffer);

internal void InitializeGeometry(engine_state *State);

#define ENGINE_H
#endif
