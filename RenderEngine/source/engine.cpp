/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Yves $
   $Notice: This is experimental code $
   ======================================================================== */

#include "engine.h"

internal void
RenderWeirdGradient(offscreen_buffer *Buffer, int BlueOffset, int GreenOffset)
{
    // TODO(casey): Let's see what the optimizer does

    uint8 *Row = (uint8 *)Buffer->Memory;    
    for(int Y = 0;
        Y < Buffer->Height;
        ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for(int X = 0;
            X < Buffer->Width;
            ++X)
        {
            uint8 Blue = (uint8)(X + BlueOffset);
            uint8 Green = (uint8)(Y + GreenOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }
        
        Row += Buffer->Pitch;
    }
}

internal void
UpdateAndRender(engine_memory *Memory, keyboard_input *Keyboard, offscreen_buffer *Buffer)
{
    Assert(sizeof(engine_state) <= Memory->PermanentStorageSize);
    
    engine_state *EngineState = (engine_state *)Memory->PermanentStorage;
    if(!Memory->IsInitialized)
    {
        // TODO(casey): This may be more appropriate to do in the platform layer
        Memory->IsInitialized = true;
    }

    // NOTE(casey): Use digital movement tuning
    if(Keyboard->MoveLeft.EndedDown)
    {
        EngineState->BlueOffset -= 1;
    }
    
    if(Keyboard->MoveRight.EndedDown)
    {
        EngineState->BlueOffset += 1;
    }

    // Input.AButtonEndedDown;
    // Input.AButtonHalfTransitionCount;
    if(Keyboard->ActionDown.EndedDown)
    {
        EngineState->GreenOffset += 1;
    }
    
    RenderWeirdGradient(Buffer, EngineState->BlueOffset, EngineState->GreenOffset);
}
