// -----------------------------------------------------------------------------
// Math Library
//
// General math macros, geometry, vectors, random number generator, and noise.
// -----------------------------------------------------------------------------
#ifndef __MATHLIB_H__
#define __MATHLIB_H__

#include "genlib.h"
#include <stdbool.h>

#define RANDOM_MAX          0xFFFFFFFF
#define RANDOM_INDEX(arr)   Random(0, sizeof(arr) / sizeof(arr[0]) - 1)
#define RANDOM_ELEMENT(arr) arr[RANDOM_INDEX(arr)]

#define CLAMP(a, min, max) if (a<min) { a=min; } else if (a>max) { a=max; }
#define SIGN(a) (a < 0 ? -1 : a > 0 ? 1 : 0)
#define MAX(a, b) ((a > b) ? (a) : (b))
#define MIN(a, b) ((a < b) ? (a) : (b))
#define DEG2RAD(a) ( ((a) * M_PI) / 180.0f )
#define RAD2DEG(a) ( ((a) * 180.0f) / M_PI )
#define SQR(x) ((x) * (x))
#define DISTANCE(x1, y1, x2, y2) sqrt(SQR(x2 - x1) + SQR(y2 - y1))

// map x, which is in the range `a...b` to the range `c...d`
#define MAP(x, a, b, c, d) ((x - a) * (d - c) / (b - a) + c)

// TODO: this
#define ADD_2D(a, b) (typeof(a)){ a.x + b.x, a.y + b.y }
// ...

typedef struct {
    float x;
    float y;
} vec2_t;

// create a new vector from +, - operations, etc.
// e.g. vec2_t sum = VEC(vecA, +, vecB)
#define VEC(a, op, b)   (vec2_t){ a.x op b.x, a.y op b.y }

// modify vector a with +=, -= operations, etc.
// e.g. VOP(vecA, +=, vecB)
#define VOP(a, op, b)   { a.x op b.x; a.y op b.y }

// scale vector v by s
#define VSCALE(v, s)    { v.x *= s; v.y *= s; }

/// Linearly interpolate from a to b with factor w.
float Lerp(float a, float b, float w);

/// Test for line-circle interaction.
/// - Parameter p1: Start of line.
/// - Parameter p2: End of line.
/// - Parameter cp: Center of circle.
/// - Parameter r: Circle radius.
/// - Returns: Whether the line intersects with the circle.
bool LineCircleIntersection
(   vec2_t p1,
    vec2_t p2,
    vec2_t cp,
    double r
/*, bool segment */ );

int RectDiagonalLength(int w, int h);

inline vec2_t AddVectors(vec2_t a, vec2_t b) {
    return (vec2_t){ a.x + b.x, a.y + b.y };
}

/// Linearly interpolate factor v to vector `to` with factor `w`.
/// - Returns: true if vector `v` is within 1 of vector `to`.
bool LerpVector(vec2_t * v, const vec2_t * to, float w);

vec2_t  NormalizeVector(vec2_t v);
vec2_t  RotateVector(vec2_t v, float radians);
void    WrapPosition(vec2_t * position, int w, int h);

/// Rotate a vector by a random amount.
/// - Parameter v: The vector to rotate.
/// - Parameter degrees: The range (+/- degrees) in which
///   the vector will be rotated.
void RandomizeVector(vec2_t * v, float degrees);

vec2_t  RandomDirection(void);
vec2_t  RandomVelocity(float min, float max);

void SeedRandom(u32 newSeed);

/// Seed the random number generator with `time()`.
void Randomize(void);

/// Generator a random int between min and max, inclusive
u32 Random(u32 min, u32 max);

/// Generator a random float between min and max, inclusive
float RandomFloat(float min, float max);

#pragma mark - NOISE

void RandomizeNoise(u32 seed);

/// Perlin noise at point x, y, z.
/// - Parameter frequency: Scaling value.
/// - Parameter persistense: `0.0...1.0`
/// - Parameter lacunarity: > `1.0`
float Noise2
(   float x,
    float y,
    float z,
    float frequency,
    int   octaves,
    float amplitude,
    float persistence,
    float lacunarity );

/// Perlin noise at point x, y, z. Uses common default noise parameters. Use
/// `Perlin2` if you need to specify these parameters.
inline float Noise(float x, float y, float z)
{
    return Noise2(x, y, z, 0.01, 6, 1.0, 0.5, 2.0);
}

#endif /* __MATHLIB_H__ */
