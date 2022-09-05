#include "mathlib.h"
#include <math.h>

float
Lerp(float a, float b, float w)
{
    if ( fabsf( a - b ) < 1.0f ) {
        return b;
    } else {
        return (1.0f - w) * a + w * b;
    }
}

#pragma mark - GEOMETRY

//
// LineCircleIntersection
//
// Prints the intersection points (if any) of a circle, center 'cp' with
// radius 'r', and either an infinite line containing the points 'p1' and 'p2'
// or a segment drawn between those points.
//

static double
sq(double x)
{
    return x * x;
}

static const double eps = 1e-14;

bool
LineCircleIntersection
(   vec2_t p1,
    vec2_t p2,
    vec2_t cp,
    double r
    /*, bool segment */ )
{
    double x0 = cp.x, y0 = cp.y;
    double x1 = p1.x, y1 = p1.y;
    double x2 = p2.x, y2 = p2.y;
    double A = y2 - y1;
    double B = x1 - x2;
    double C = x2 * y1 - x1 * y2;
    double a = sq(A) + sq(B);
    double b, c, d;
    bool bnz = true;
    //int cnt = 0;

    if (fabs(B) >= eps) {
        // if B isn't zero or close to it
        b = 2 * (A * C + A * B * y0 - sq(B) * x0);
        c = sq(C) + 2 * B * C * y0 - sq(B) * (sq(r) - sq(x0) - sq(y0));
    } else {
        b = 2 * (B * C + A * B * x0 - sq(A) * y0);
        c = sq(C) + 2 * A * C * x0 - sq(A) * (sq(r) - sq(x0) - sq(y0));
        bnz = false;
    }
    d = sq(b) - 4 * a * c; // discriminant
    if (d < 0) {
        // line & circle don't intersect
        //printf("[]\n");
        return false;
    }

#if 0
    if (d == 0) {
        // line is tangent to circle, so just one intersect at most
        if (bnz) {
            double x = -b / (2 * a);
            double y = fx(A, B, C, x);
            cnt = rxy(x1, y1, x2, y2, x, y, segment);
        } else {
            double y = -b / (2 * a);
            double x = fy(A, B, C, y);
            cnt = rxy(x1, y1, x2, y2, x, y, segment);
        }
    } else {
        // two intersects at most
        d = sqrt(d);
        if (bnz) {
            double x = (-b + d) / (2 * a);
            double y = fx(A, B, C, x);
            cnt = rxy(x1, y1, x2, y2, x, y, segment);

            x = (-b - d) / (2 * a);
            y = fx(A, B, C, x);
            cnt += rxy(x1, y1, x2, y2, x, y, segment);
        } else {
            double y = (-b + d) / (2 * a);
            double x = fy(A, B, C, y);
            cnt = rxy(x1, y1, x2, y2, x, y, segment);

            y = (-b - d) / (2 * a);
            x = fy(A, B, C, y);
            cnt += rxy(x1, y1, x2, y2, x, y, segment);
        }
    }
#endif
    return true;

//    if (cnt <= 0) {
//        printf("[]");
//    }
}

int
RectDiagonalLength(int w, int h)
{
    return (int)sqrt(w * w + h * h);
}

#pragma mark - RANDOM

static u32 next = 1;

void
SeedRandom(u32 seed)
{
    next = seed;
}

void
Randomize(void)
{
    next = (u32)time(NULL);
}

// https://lemire.me/blog/

static inline u32
Wyhash32(void)
{
    uint64_t tmp;
    uint32_t m1, m2;

    next += 0xE120FC15;
    tmp  = (uint64_t)next * 0x4A39B70D;
    m1   = (uint32_t)(( tmp >> 32) ^ tmp );
    tmp  = (uint64_t)m1 * 0x12FAD5C9;
    m2   = (uint32_t)( (tmp >> 32) ^ tmp );

    return m2;
}

u32
Random(u32 min, u32 max)
{
    return Wyhash32() % (max - min + 1) + min;
}

static inline float
_RandomFloat(void)
{
    return (float)((double)Wyhash32() / (double)RANDOM_MAX);
}

float
RandomFloat(float min, float max)
{
    return _RandomFloat() * (max - min) + min;
}

#pragma mark - VECTOR

bool
LerpVector(vec2_t * v, const vec2_t * to, float w)
{
    float dx = to->x - v->x;
    float dy = to->y - v->y;
    float threshold = 1.0f;
    bool x_arrived;
    bool y_arrived;

    if ( fabsf( dx ) > threshold ) {
        v->x += dx * w;
        x_arrived = false;
    } else {
        v->x = to->x;
        x_arrived = true;
    }

    if ( fabsf( dy ) > threshold ) {
        v->y += dy * w;
        y_arrived = false;
    } else {
        v->y = to->y;
        y_arrived = true;
    }

    return x_arrived && y_arrived;
}

vec2_t
NormalizeVector(vec2_t v)
{
    vec2_t result = { 0, 0 };

    float length = sqrtf(v.x * v.x + v.y * v.y);
    if ( length == 0.0f ) {
        return result;
    }

    float ilength = 1.0 / length;
    result.x = v.x * ilength;
    result.y = v.y * ilength;

    return result;
}

vec2_t
RotateVector(vec2_t v, float radians)
{
    vec2_t result;
    result.x = cos(radians) * v.x - sin(radians) * v.y;
    result.y = sin(radians) * v.x + cos(radians) * v.y;

    return result;
}

void
WrapPosition(vec2_t * position, int w, int h)
{
    position->x = fmod(position->x + w, w);
    position->y = fmod(position->y + h, h);
}

vec2_t RandomDirection(void)
{
    return RotateVector((vec2_t){ 1, 0 }, DEG2RAD(Random(0, 359)));
}

vec2_t RandomVelocity(float min, float max)
{
    vec2_t v = RandomDirection();
    VSCALE(v, RandomFloat(min, max));

    return v;
}

void RandomizeVector(vec2_t * v, float radians) {
    *v =  RotateVector(*v, RandomFloat(-radians, +radians));
}

#pragma mark - NOISE

#define P_SIZE 512

//
// These values always are copied to `p` when randomizing noise. This way
// the same seed always produces the same result.
//
static const uint8_t originalPermutation[P_SIZE] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120,
    234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
    134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133,
    230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161,
    1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130,
    116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250,
    124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227,
    47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44,
    154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
    108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34,
    242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
    239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121,
    50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243,
    141, 128, 195, 78, 66, 215, 61, 156, 180,
    // -------------------------------------------------------------------------
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120,
    234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
    134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133,
    230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161,
    1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130,
    116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250,
    124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227,
    47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44,
    154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
    108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34,
    242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
    239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121,
    50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243,
    141, 128, 195, 78, 66, 215, 61, 156, 180,
};

static uint8_t p[P_SIZE] = {
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120,
    234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
    134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133,
    230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161,
    1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130,
    116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250,
    124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227,
    47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44,
    154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
    108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34,
    242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
    239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121,
    50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243,
    141, 128, 195, 78, 66, 215, 61, 156, 180,
    // -------------------------------------------------------------------------
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120,
    234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
    134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133,
    230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161,
    1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130,
    116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250,
    124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227,
    47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44,
    154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
    108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34,
    242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
    239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121,
    50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243,
    141, 128, 195, 78, 66, 215, 61, 156, 180,
};

static float
fade(float t)
{
    return t*t*t*(t*(t*6 - 15) + 10);
}

static float
lerp(float t, float a, float b)
{
    return a + t*(b - a);
}

static float
grad(int hash, float x, float y, float z)
{
    int h = hash & 15;
    float u = h<8 ? x : y,
           v = h<4 ? y : h==12||h==14 ? x : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

//
// COPYRIGHT 2002 KEN PERLIN
// Adapted from Java code by Thomas Foster
//

static float
perlin(float x, float y, float z)
{
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);
    float u = fade(x);
    float v = fade(y);
    float w = fade(z);
    int A = p[X    ] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
    int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA    ], x  , y  , z ),
                                   grad(p[BA    ], x-1, y  , z )),
                           lerp(u, grad(p[AB    ], x  , y-1, z ),
                                   grad(p[BB    ], x-1, y-1, z ))),
                   lerp(v, lerp(u, grad(p[AA + 1], x  , y  , z-1 ),
                                   grad(p[BA + 1], x-1, y  , z-1 )),
                           lerp(u, grad(p[AB + 1], x  , y-1, z-1 ),
                                   grad(p[BB + 1], x-1, y-1, z-1 ))));

}

void
RandomizeNoise(u32 seed)
{
    memcpy(p, originalPermutation, sizeof(p)); // restart
    SeedRandom(seed);

    // shuffle
    for ( int i = 0; i < P_SIZE; i++) {
        int r = Random(0, P_SIZE - 1);

        uint8_t temp = p[i];
        p[i] = p[r];
        p[r] = temp;
    }
}

float
Noise2
(   float x,
    float y,
    float z,
    float frequency,
    int   octaves,
    float amplitude,
    float persistence,
    float lacunarity )
{
    float total = 0;

    for ( int i = 0; i < octaves; i++ ) {
        total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return total;
}

extern inline float Noise(float x, float y, float z);
