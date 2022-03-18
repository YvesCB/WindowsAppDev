#ifndef MY_MATH
#define MY_MATH

#include <math.h>
#include "windows_platform.h"

inline float sqr(float x)
{
    return(x*x);
};

inline float parabola(float x, StateInfo* state)
{
    return state->parab_a*(x-state->parab_h)*(x-state->parab_h) +state->parab_k;
};

float deriveAtX(float (*f)(float value_x, float p_x, float p_y, StateInfo* state), 
        float x, float p_x, float p_y, StateInfo* state)
{
    float eps = 0.1f;
    float sample1 = (f(x+eps, p_x, p_y, state) - f(x, p_x, p_y, state))/eps; 
    float sample2 = (f(x-eps, p_x, p_y, state) - f(x, p_x, p_y, state))/(-1*eps); 

    return (sample1 + sample2)/2.0f;
}

float newtonFindZero(float (*f)(float value_x, float p_x, float p_y, StateInfo* state),
        float p_x, float p_y, StateInfo* state)
{
    float margin = 0.001f;
    int attempt = 0;
    float guess = 1.0f;

    while(f(guess, p_x, p_y, state) > margin || f(guess, p_x, p_y, state) < -1.0f * margin)
    {
        guess = guess - (f(guess, p_x, p_y, state)/deriveAtX(f, guess, p_x, p_y, state));
        attempt++;
        if(attempt > 10) 
        {
            guess = NAN;
            break;
        }
    }

    return guess;
}

float squareDistancePointCurve(float (*f)(float value_x, StateInfo* state), 
        float x, StateInfo* state, float p_x, float p_y)
{
    return sqr((x-p_x)) + sqr(f(x, state)-p_y);
}

float squareDerivDistPointCurve(float x, float p_x, float p_y, StateInfo* state)
{
    return 2 * (2 * state->parab_a * (x-state->parab_h) * (state->parab_a * sqr(x-state->parab_h) + state->parab_k - p_y) - p_x + x);
}

#endif
