#if !defined VECTOR_MATH_H

#include <math.h>

typedef struct v3f
{
    float X;
    float Y;
    float Z;
} v3f;

float vDot(v3f A, v3f B)
{
    return (A.X * B.X + A.Y * B.Y + A.Z * B.Z);
}

v3f vCross(v3f A, v3f B)
{
    v3f Res = {A.Y*B.Z - A.Z*B.Y, A.X*B.Z - A.Z*B.X, A.X*B.Y - A.Y*B.X};
    return (Res);
}

v3f vAdd(v3f A, v3f B)
{
    v3f Res = {A.X + B.X, A.Y + B.Y, A.Z + B.Z};
    return (Res);
}

v3f vNeg(v3f A)
{
    v3f Res = {-A.X, -A.Y, -A.Z};
    return (Res);
}

v3f vScale(v3f A, float K)
{
    v3f Res = {A.X * K, A.Y * K, A.Z * K};
    return (Res);
}

float vLen(v3f A)
{
    return (sqrtf(A.X*A.X + A.Y*A.Y + A.Z*A.Z));
}

v3f vNorm(v3f A)
{
    v3f Res = {A.X, A.Y, A.Z};
    return (vScale(Res, (1.0f/vLen(Res))));
}

int CheckZero(float A, float eps)
{
    if(fabsf(0.0f - A) < eps)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

#define VECTOR_MATH_H
#endif
