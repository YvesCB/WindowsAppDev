#ifndef MY_MATH
#define MY_MATH

#include <math.h>
#include "windows_platform.h"

float SQR2 = 1.41421f;

typedef struct v2f
{
    float x;
    float y;
} v2f;

v2f vecAdd(v2f a, v2f b)
{
    v2f result = { };

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

v2f vecMult(v2f a, float t)
{
    v2f result = { };

    result.x = a.x * t;
    result.y = a.y * t;

    return result;
}

v2f vecNeg(v2f a)
{
    v2f result = { };

    result.x = -a.x;
    result.y = -a.y;

    return result;
}

float vecDot(v2f a, v2f b)
{
    return (a.x*b.x + a.y*b.y);
}

float vecDist(v2f a, v2f b)
{
    return sqrtf((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}

float vecLen(v2f a)
{
    return (sqrtf(a.x*a.x + a.y*a.y));
}

inline float sqr(float x)
{
    return(x*x);
};

inline float parabola(float x, StateInfo* state)
{
    return state->parab_a*(x-state->parab_h)*(x-state->parab_h) + state->parab_k;
};


inline float squareDerivDistPointCurve(float x, float p_x, float p_y, StateInfo* state)
{
    return 2 * (2 * state->parab_a * (x-state->parab_h) * (state->parab_a * sqr(x-state->parab_h) + state->parab_k - p_y) - p_x + x);
}

float deriveAtX(float x, float p_x, float p_y, StateInfo* state)
{
    float eps = 0.1f;
    float sample1 = (squareDerivDistPointCurve(x+eps, p_x, p_y, state) - squareDerivDistPointCurve(x, p_x, p_y, state))/eps; 
    float sample2 = (squareDerivDistPointCurve(x-eps, p_x, p_y, state) - squareDerivDistPointCurve(x, p_x, p_y, state))/(-1*eps); 

    return (sample1 + sample2)/2.0f;
}

float newtonFindZero(float p_x, float p_y, StateInfo* state)
{
    float margin = 0.01f;
    int attempt = 0;
    float guess = p_x;

    //if(p_x <= state->parab_h)
    //{
        //guess = p_x + (state->parab_h - p_x) / 2.0f;
    //}
    //else 
    //{
        //guess = p_x - (p_x - state->parab_h) / 2.0f;    
    //}

    float squareDerivAtGuess = 1.0f;

    while(squareDerivAtGuess > margin || squareDerivAtGuess < -1.0f * margin)
    {
        guess = guess - (squareDerivDistPointCurve(guess, p_x, p_y, state)/deriveAtX(guess, p_x, p_y, state));
        squareDerivAtGuess = squareDerivDistPointCurve(guess, p_x, p_y, state);
        attempt++;
        if(attempt > 10) 
        {
            guess = NAN;
            break;
        }
    }

    return guess;
}

float squareDistancePointCurve(float x, StateInfo* state, float p_x, float p_y)
{
    return powf((x-p_x), 2) + powf(parabola(x, state)-p_y, 2);
}
#endif
