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

internal void
RenderGeometry(offscreen_buffer *Buffer, engine_state *State)
{
    // NOTE: First we compute the vector pointing to the edges of the screen from the
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
            // For each pixel, we calculate t which will give us the intersect point
            struct triangle CurTriangle = State->SceneGeometry.Triangles[0];
            v3f CamToPixel = vAdd(CurrentPixel, vNeg(State->CameraPosition));
            float Divider = vDot(CurTriangle.N, CamToPixel);

            // Check if t is gonna be INF or 0 within margin
            if(CheckZero(Divider, EPS) || Divider < 0.0f)
            {
                // If t would be 0 or >0, the pixel will be black
                *Pixel++ = 0; 
            }
            else
            {
                float T = -(CurTriangle.D/Divider);
                v3f IntersectPoint = vAdd(State->CameraPosition, vScale(CamToPixel, T));
                int Inside = 0;
                v3f T0T1 = vAdd(CurTriangle.T1, vNeg(CurTriangle.T0));
                v3f T1T2 = vAdd(CurTriangle.T2, vNeg(CurTriangle.T1));
                v3f T2T0 = vAdd(CurTriangle.T0, vNeg(CurTriangle.T2));
                v3f T0I = vAdd(IntersectPoint, vNeg(CurTriangle.T0));
                v3f T1I = vAdd(IntersectPoint, vNeg(CurTriangle.T1));
                v3f T2I = vAdd(IntersectPoint, vNeg(CurTriangle.T2));
                
                if(vDot(T0T1, T0I) > 0.0f && vDot(T1T2, T1I) > 0.0f && vDot(T2T0, T2I) > 0.0f ||
                    vDot(T0T1, T0I) < 0.0f && vDot(T1T2, T1I) < 0.0f && vDot(T2T0, T2I) < 0.0f)
                {
                    *Pixel++ = CurTriangle.Color;
                }
                else
                {
                    *Pixel++ = 0;
                }
            }
            CurrentPixel = vAdd(CurrentPixel, StepX);
        }
        CurrentPixel = vAdd(CurrentPixel, StepY);
        
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
        Memory->IsInitialized = true;
    }

    InitializeGeometry(EngineState);

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
    Triangle.N =        vNorm(vCross(vAdd(Triangle.T1, vNeg(Triangle.T0)), 
                        vAdd(Triangle.T2, vNeg(Triangle.T0))));
    Triangle.D =        -Triangle.N.X*Triangle.T0.X
                        -Triangle.N.Y*Triangle.T0.Y
                        -Triangle.N.Z*Triangle.T0.Z;
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
