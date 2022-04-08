/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Yves $
   $Notice: This is experimental code $
   ======================================================================== */

#include "engine.h"

//internal void
//RenderWeirdGradient(offscreen_buffer *Buffer, engine_memory *Memory)
//{
    //// TODO(casey): Let's see what the optimizer does

    //uint8 *Row = (uint8 *)Buffer->Memory;    
    //for(int Y = 0;
        //Y < Buffer->Height;
        //++Y)
    //{
        //uint32 *Pixel = (uint32 *)Row;
        //for(int X = 0;
            //X < Buffer->Width;
            //++X)
        //{
            //uint8 Blue = (uint8)(X);
            //uint8 Green = (uint8)(Y);

            //*Pixel++ = ((Green << 8) | Blue);
        //}
        
        //Row += Buffer->Pitch;
    //}
//}

internal v3f
IntersectionPointRayTriangle(v3f R0, v3f R, triangle *Triangle)
{
    v3f Result = {0.0f, 0.0f, 0.0f};
    return (Result);
}

internal bool32
PointInsideTriangle(v3f P, triangle *Triangle)
{
    return true;
}

internal void
RenderGeometry(offscreen_buffer *Buffer, engine_state *State)
{
    // First we compute the vector pointing to the edges of the screen from the
    // center. We get this by scaling the UpVector by half the height and the
    // LeftVector by half the width.

    v3f ScreenY = vScale(State->CameraUpVector, Buffer->Height/2.0f);
    v3f ScreenXPrime = vNorm(vCross(State->CameraUpVector, State->CameraToScreenVector));
    v3f ScreenX = vScale(ScreenXPrime, Buffer->Width/2.0f);

    v3f CurrentPixel = vAdd(State->CameraPosition, State->CameraToScreenVector);
    CurrentPixel = vAdd(CurrentPixel, ScreenY);
    CurrentPixel = vAdd(CurrentPixel, ScreenX);

    v3f StepX = vNorm(vNeg(ScreenX));
    v3f StepY = vNorm(vNeg(ScreenY));

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
            uint8 Blue = (uint8)(X);
            uint8 Green = (uint8)(Y);

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

    InitializeGeometry(EngineState);

    // NOTE(casey): Use digital movement tuning
    if(Keyboard->MoveLeft.EndedDown)
    {
        //EngineState->BlueOffset -= 1;
    }
    
    if(Keyboard->MoveRight.EndedDown)
    {
        //EngineState->BlueOffset += 1;
    }

    // Input.AButtonEndedDown;
    // Input.AButtonHalfTransitionCount;
    if(Keyboard->ActionDown.EndedDown)
    {
        //EngineState->GreenOffset += 1;
    }
    
    RenderGeometry(Buffer, EngineState);
}

internal void
InitializeGeometry(engine_state *State)
{
    triangle Triangle = {};
    Triangle.T0 = {0.0f, 0.0f, 0.0f};
    Triangle.T1 = {100.0f, 0.0f, 0.0f};
    Triangle.T2 = {0.0f, 0.0f, 100.0f};
    Triangle.Color = (int32)(200 << 16 | 0 << 8 | 0);
    geometry Geometry = {&Triangle};
    State->SceneGeometry = Geometry;

    State->CameraPosition = {50.0f, -1000.0f, 50.0f};
    State->CameraUpVector = {0.0f, 0.0f, 1.0f};
    State->CameraToScreenVector = {0.0f, 700.0f, 0.0f};

    // Look right at Origin
    v3f CameraViewVector = vAdd(Triangle.T0, vNeg(State->CameraPosition));
    State->CameraViewVector = CameraViewVector;
}
