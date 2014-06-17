#ifndef INCLUDE_JRC_MATH_H
#define INCLUDE_JRC_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

// PI IS EXACTLY THREE
#define MY_PI  3.14159265358979323846
#define MY_2PI 6.28318530717958647692

typedef int ivec_t;
typedef ivec_t ivec2_t[2];
typedef ivec_t ivec3_t[3];
typedef ivec_t ivec4_t[4];

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

typedef vec_t mat4_t[16];
typedef vec_t mat4x3_t[12];

/* Quaternion is split into vector [0-2] and scalar [3]. */
typedef vec4_t quat_t;

/* plane is the unit normal [0-2] and the distance from origin[3], point on plane can be derived by multiplying the normal by the distance. */
typedef vec4_t plane_t;

/* sphere is the origin [0-2] and the radius[3] */
typedef vec4_t sphere_t;

typedef struct line_s
{
	vec3_t pos;
	vec3_t dir;
} line_t;

typedef line_t ray_t;

typedef vec3_t triangle_t[3];

typedef vec3_t aabb_t[2];

#define EPSILON 0.00000001f

float FastSqrt(float x);
float FastInvSqrt(float x);
int FloatToInt(float f);
float IntToFloat(int ival);

#ifdef NO_MACROS
void Vec2Copy(vec2_t r, vec2_t a);
void Vec3Copy(vec3_t r, vec3_t a);
void Vec4Copy(vec4_t r, vec4_t a);
void Vec2Set(vec2_t r, vec_t x, vec_t y);
void Vec3Set(vec3_t r, vec_t x, vec_t y, vec_t z);
void Vec4Set(vec4_t r, vec_t x, vec_t y, vec_t z, vec_t w);
vec_t DotProduct3(vec3_t a, vec3_t b);
void CrossProduct3(vec3_t r, vec3_t a, vec3_t b);
void Vec3Add(vec3_t r, vec3_t a, vec3_t b);
void Vec3MultiplyAdd(vec3_t r, vec3_t a, vec_t s, vec3_t b);
void Vec3Subtract(vec3_t r, vec3_t a, vec3_t b);
void Vec3Scale(vec3_t r, vec_t s, vec3_t a);
vec_t VecLerp(vec_t a, vec_t s, vec_t b);
#else
#define Vec2Copy(r, a) \
	(((r)[0] = (a)[0]), \
	 ((r)[1] = (a)[1])) \

#define Vec3Copy(r, a) \
	(((r)[0] = (a)[0]), \
	 ((r)[1] = (a)[1]), \
	 ((r)[2] = (a)[2]))

#define Vec4Copy(r, a) \
	(((r)[0] = (a)[0]), \
	 ((r)[1] = (a)[1]), \
	 ((r)[2] = (a)[2]), \
	 ((r)[3] = (a)[3]))
	 
#define Vec2Set(r, x, y) \
	(((r)[0] = x), \
	 ((r)[1] = y))

#define Vec3Set(r, x, y, z) \
	(((r)[0] = x), \
	 ((r)[1] = y), \
	 ((r)[2] = z))

#define Vec4Set(r, x, y, z, w) \
	(((r)[0] = x), \
	 ((r)[1] = y), \
	 ((r)[2] = z), \
	 ((r)[3] = w))

#define Vec2Add(r, a, b) \
	(((r)[0] = (a)[0] + (b)[0]), \
	 ((r)[1] = (a)[1] + (b)[1]))

#define Vec2MultiplyAdd(r, a, s, b) \
	(((r)[0] = (a)[0] + (s) * (b)[0]), \
	 ((r)[1] = (a)[1] + (s) * (b)[1]))

#define Vec2Subtract(r, a, b) \
	(((r)[0] = (a)[0] - (b)[0]), \
	 ((r)[1] = (a)[1] - (b)[1]))

#define Vec2Scale(r, s, a) \
	(((r)[0] = (s) * (a)[0]), \
	 ((r)[1] = (s) * (a)[1]))
	 
#define DotProduct2(a, b) \
	((a)[0] * (b)[0] + (a)[1] * (b)[1])

#define Vec3Add(r, a, b) \
	(((r)[0] = (a)[0] + (b)[0]), \
	 ((r)[1] = (a)[1] + (b)[1]), \
	 ((r)[2] = (a)[2] + (b)[2]))

#define Vec4Add(r, a, b) \
	(((r)[0] = (a)[0] + (b)[0]), \
	 ((r)[1] = (a)[1] + (b)[1]), \
	 ((r)[2] = (a)[1] + (b)[2]), \
	 ((r)[3] = (a)[2] + (b)[3]))

#define Vec3MultiplyAdd(r, a, s, b) \
	(((r)[0] = (a)[0] + (s) * (b)[0]), \
	 ((r)[1] = (a)[1] + (s) * (b)[1]), \
	 ((r)[2] = (a)[2] + (s) * (b)[2]))

#define Vec3Subtract(r, a, b) \
	(((r)[0] = (a)[0] - (b)[0]), \
	 ((r)[1] = (a)[1] - (b)[1]), \
	 ((r)[2] = (a)[2] - (b)[2]))

#define Vec3Scale(r, s, a) \
	(((r)[0] = (s) * (a)[0]), \
	 ((r)[1] = (s) * (a)[1]), \
	 ((r)[2] = (s) * (a)[2]))
	 
#define Vec4Scale(r, s, a) \
	(((r)[0] = (s) * (a)[0]), \
	 ((r)[1] = (s) * (a)[1]), \
	 ((r)[2] = (s) * (a)[2]), \
	 ((r)[3] = (s) * (a)[3]))
	 
#define DotProduct3(a, b) \
	((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2])

#define DotProduct4(a, b) \
	((a)[0] * (b)[0] + (a)[1] * (b)[1] + (a)[2] * (b)[2] + (a)[3] * (b)[3])

#define CrossProduct3(r, a, b) \
	(((r)[0] = (a)[1] * (b)[2] - (a)[2] * (b)[1]), \
	 ((r)[1] = (a)[2] * (b)[0] - (a)[0] * (b)[2]), \
	 ((r)[2] = (a)[0] * (b)[1] - (a)[1] * (b)[0]))

#define VecLerp(a, s, b) \
	((a) + ((b) - (a)) * (s))
	
#define Vec2Lerp(r, a, s, b) \
	(((a)[0] + ((b)[0] - (a)[0]) * (s)), \
	 ((a)[1] + ((b)[1] - (a)[1]) * (s)))

#define Vec3Lerp(r, a, s, b) \
	(((r)[0] = (a)[0] + ((b)[0] - (a)[0]) * (s)), \
	 ((r)[1] = (a)[1] + ((b)[1] - (a)[1]) * (s)), \
	 ((r)[2] = (a)[2] + ((b)[2] - (a)[2]) * (s)))
	 
#define Vec2Equal(a, b) \
    (((a)[0] == (b)[0]) && \
	 ((a)[1] == (b)[1]))
	 
#define Vec3Equal(a, b) \
    (((a)[0] == (b)[0]) && \
	 ((a)[1] == (b)[1]) && \
	 ((a)[2] == (b)[2]))
	 
#endif

void Vec3Normalize(vec3_t a);
vec_t Vec3Length(vec3_t a);

void Vec4Normalize(vec4_t a);


void Mat4Identity(mat4_t r);
void Mat4Copy(mat4_t r, mat4_t a);
void Mat4Multiply(mat4_t r, mat4_t a, mat4_t b);
void Mat4MultiplyVec3(vec3_t r, mat4_t a, vec3_t b);
void Mat4MultiplyVec4(vec4_t r, mat4_t a, vec4_t b);
void Mat4Model(mat4_t model, vec3_t right, vec3_t up, vec3_t back, vec3_t pos);
void Mat4View(mat4_t view, vec3_t facing, vec3_t up, vec3_t pos);
void Mat4Ortho(mat4_t ortho, vec_t left, vec_t right, vec_t top, vec_t bottom, vec_t nearPlane, vec_t farPlane);
void Mat4Perspective(mat4_t projection, vec_t width, vec_t height, vec_t nearPlane, vec_t farPlane);
void Mat4InvertSimple(mat4_t r, mat4_t a);
void Mat4InvertSimple2(mat4_t r, mat4_t a);

void Mat4x3_FromTranslateRotateScale(mat4x3_t m, vec3_t translate, quat_t rotation, vec3_t scale);
void Mat4x3_Copy(mat4x3_t r, mat4x3_t a);
void Mat4x3_Multiply(mat4x3_t r, mat4x3_t a, mat4x3_t b);
void Mat4x3_MultiplyVec3(vec3_t r, mat4x3_t a, vec3_t b);
void Mat4x3_MultiplyVec4(vec4_t r, mat4x3_t a, vec4_t b);
void Mat4x3_InvertSimple(mat4x3_t r, mat4x3_t a);

int Vec2PointInTriangle(vec2_t point, vec2_t t1, vec2_t t2, vec2_t t3);

char LinePlaneIntersection(line_t line, plane_t plane, vec_t *distance);
char RayPlaneIntersection(ray_t ray, plane_t plane, vec_t *distance);
char RaySphereIntersection(ray_t ray, sphere_t sphere, vec_t *distance);
char RayTriangleIntersection(ray_t ray, triangle_t triangle, plane_t triPlane, vec4_t triCache, vec_t *distance);

void TriangleCalcCaches(triangle_t triangle, plane_t plane, vec4_t triCache);

#define SWEEP_ALL_OUT   0
#define SWEEP_IN_TO_OUT 1
#define SWEEP_OUT_TO_IN 2
#define SWEEP_ALL_IN    3

typedef struct
{
	aabb_t aabb;
	plane_t *planes;
	int numPlanes;
}
convexHull_t;

void Aabb_Clear(aabb_t aabb);
void Aabb_SetToPoint(aabb_t aabb, vec3_t point);
void Aabb_AddPoint(aabb_t aabb, vec3_t point);
void Aabb_Copy(aabb_t a, aabb_t b);
void Aabb_Add(aabb_t r, aabb_t a, aabb_t b);
int Aabb_Intersect(aabb_t a, aabb_t b);
int Aabb_SweepCollision(aabb_t bounds1, vec3_t move1, aabb_t bounds2, vec3_t move2, vec_t *impactTime);
void Aabb_ToPosAndSize(vec3_t pos, vec3_t size, aabb_t aabb);
void Aabb_FromPosAndSize(aabb_t aabb, vec3_t pos, vec3_t size);

void CalcTexVectors(vec3_t sdir, vec3_t tdir, vec3_t v1, vec3_t v2, vec3_t v3, vec2_t w1, vec2_t w2, vec2_t w3);
vec_t CalcTangentSpace(vec3_t tangent, vec3_t bitangent, vec3_t normal, vec3_t sdir, vec3_t tdir);

void QuatFromAxes(vec4_t quat, vec3_t x, vec3_t y, vec3_t z);
void AxesFromPitchYaw(vec3_t x, vec3_t y, vec3_t z, vec_t radYaw, 
                      vec_t radPitch);

int PointInsidePlane(vec3_t point, plane_t plane);
int SweepLineWithPlane(vec3_t startPos, vec3_t move, plane_t plane, vec_t *hitFraction);
int SweepAabbWithPlane(aabb_t aabb, vec3_t move, plane_t plane, vec_t *hitFraction);
void ConvexHullFromAabb(convexHull_t *ch, aabb_t aabb);
int SweepAabbWithConvexHull(aabb_t aabb, vec3_t move, convexHull_t *ch, vec_t *hitFraction, plane_t hitPlane);
int Aabb_SweepCollision2(aabb_t bounds1, vec3_t move1, aabb_t bounds2, vec3_t move2, vec_t *hitDistance, plane_t hitPlane);
int CheckAndResolveCollision(aabb_t staticBounds, aabb_t movingBounds, vec3_t correction);

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define CLAMP(t,a,b) ((t) > (b) ? (b) : (t) < (a) ? (a) : (t))

#define QuatConjugate(r, a) \
	(((r)[0] = -(a)[0]), \
	 ((r)[1] = -(a)[1]), \
	 ((r)[2] = -(a)[2]), \
	 ((r)[3] =  (a)[3]))

#define QuatMultiply(r, a, b) \
	(((r)[0] = (a)[3] * (b)[0] + (a)[0] * (b)[3] + (a)[1] * (b)[2] - (a)[2] * (b)[1]), \
	 ((r)[1] = (a)[3] * (b)[1] + (a)[1] * (b)[3] + (a)[2] * (b)[0] - (a)[0] * (b)[2]), \
	 ((r)[2] = (a)[3] * (b)[2] + (a)[2] * (b)[3] + (a)[0] * (b)[1] - (a)[1] * (b)[0]), \
	 ((r)[3] = (a)[3] * (b)[3] - (a)[0] * (b)[0] - (a)[1] * (b)[1] - (a)[2] * (b)[2]))

#ifdef __cplusplus
}
#endif

#endif


#ifdef JRC_MATH_IMPLEMENTATION
#include <assert.h>
#include <stdio.h>
#include <math.h>

#ifdef NO_MACROS

void Vec2Copy(vec2_t r, vec2_t a)
{
	r[0] = a[0];
	r[1] = a[1];
}

void Vec3Copy(vec3_t r, vec3_t a)
{
	r[0] = a[0];
	r[1] = a[1];
	r[2] = a[2];
}

void Vec4Copy(vec4_t r, vec4_t a)
{
	r[0] = a[0];
	r[1] = a[1];
	r[2] = a[2];
	r[3] = a[3];
}

void Vec2Set(vec2_t r, vec_t x, vec_t y)
{
	r[0] = x; r[1] = y;
}

void Vec3Set(vec3_t r, vec_t x, vec_t y, vec_t z)
{
	r[0] = x; r[1] = y; r[2] = z;
}

void Vec4Set(vec4_t r, vec_t x, vec_t y, vec_t z, vec_t w)
{
	r[0] = x; r[1] = y; r[2] = z; r[3] = w;
}

vec_t DotProduct3(vec3_t a, vec3_t b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void CrossProduct3(vec3_t r, vec3_t a, vec3_t b)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
}

void Vec3Add(vec3_t r, vec3_t a, vec3_t b)
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
}

void Vec3MultiplyAdd(vec3_t r, vec3_t a, vec_t s, vec3_t b)
{
	r[0] = a[0] + s * b[0];
	r[1] = a[1] + s * b[1];
	r[2] = a[2] + s * b[2];
}

void Vec3Subtract(vec3_t r, vec3_t a, vec3_t b)
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
	r[2] = a[2] - b[2];
}

void Vec3Scale(vec3_t r, vec_t s, vec3_t a)
{
	r[0] = s * a[0];
	r[1] = s * a[1];
	r[2] = s * a[2];
}

vec_t VecLerp(vec_t a, vec_t s, vec_t b)
{
	//return a * (1.0f - s) + b * s;
	return a + (b - a) * s;
}

void Vec3Lerp(vec3_t r, vec3_t a, vec_t s, vec3_t b)
{
	vec3_t d;
	
	Vec3Subtract(d, b, a);
	Vec3MultiplyAdd(r, a, s, d);
}

#endif

void Vec3Normalize(vec3_t a)
{
	vec_t invLen = 1.0f / sqrt(DotProduct3(a,a));
	Vec3Scale(a, invLen, a);
}

vec_t Vec3Length(vec3_t a)
{
	return sqrt(DotProduct3(a,a));
}

void Vec4Normalize(vec4_t a)
{
	vec_t invLen = 1.0f / sqrt(DotProduct4(a,a));
	Vec4Scale(a, invLen, a);
}

void Mat4Identity(mat4_t r)
{
	r[ 0] = 1.0f;
	r[ 1] = 0.0f;
	r[ 2] = 0.0f;
	r[ 3] = 0.0f;

	r[ 4] = 0.0f;
	r[ 5] = 1.0f;
	r[ 6] = 0.0f;
	r[ 7] = 0.0f;

	r[ 8] = 0.0f;
	r[ 9] = 0.0f;
	r[10] = 1.0f;
	r[11] = 0.0f;

	r[12] = 0.0f;
	r[13] = 0.0f;
	r[14] = 0.0f;
	r[15] = 1.0f;
}

void Mat4Copy(mat4_t r, mat4_t a)
{
	r[ 0] = a[ 0];
	r[ 1] = a[ 1];
	r[ 2] = a[ 2];
	r[ 3] = a[ 3];

	r[ 4] = a[ 4];
	r[ 5] = a[ 5];
	r[ 6] = a[ 6];
	r[ 7] = a[ 7];

	r[ 8] = a[ 8];
	r[ 9] = a[ 9];
	r[10] = a[10];
	r[11] = a[11];

	r[12] = a[12];
	r[13] = a[13];
	r[14] = a[14];
	r[15] = a[15];
}

void Mat4Multiply(mat4_t r, mat4_t a, mat4_t b)
{
	r[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
	r[ 1] = a[ 1] * b[ 0] + a[ 5] * b[ 1] + a[ 9] * b[ 2] + a[13] * b[ 3];
	r[ 2] = a[ 2] * b[ 0] + a[ 6] * b[ 1] + a[10] * b[ 2] + a[14] * b[ 3];
	r[ 3] = a[ 3] * b[ 0] + a[ 7] * b[ 1] + a[11] * b[ 2] + a[15] * b[ 3];
	
	r[ 4] = a[ 0] * b[ 4] + a[ 4] * b[ 5] + a[ 8] * b[ 6] + a[12] * b[ 7];
	r[ 5] = a[ 1] * b[ 4] + a[ 5] * b[ 5] + a[ 9] * b[ 6] + a[13] * b[ 7];
	r[ 6] = a[ 2] * b[ 4] + a[ 6] * b[ 5] + a[10] * b[ 6] + a[14] * b[ 7];
	r[ 7] = a[ 3] * b[ 4] + a[ 7] * b[ 5] + a[11] * b[ 6] + a[15] * b[ 7];

	r[ 8] = a[ 0] * b[ 8] + a[ 4] * b[ 9] + a[ 8] * b[10] + a[12] * b[11];
	r[ 9] = a[ 1] * b[ 8] + a[ 5] * b[ 9] + a[ 9] * b[10] + a[13] * b[11];
	r[10] = a[ 2] * b[ 8] + a[ 6] * b[ 9] + a[10] * b[10] + a[14] * b[11];
	r[11] = a[ 3] * b[ 8] + a[ 7] * b[ 9] + a[11] * b[10] + a[15] * b[11];

	r[12] = a[ 0] * b[12] + a[ 4] * b[13] + a[ 8] * b[14] + a[12] * b[15];
	r[13] = a[ 1] * b[12] + a[ 5] * b[13] + a[ 9] * b[14] + a[13] * b[15];
	r[14] = a[ 2] * b[12] + a[ 6] * b[13] + a[10] * b[14] + a[14] * b[15];
	r[15] = a[ 3] * b[12] + a[ 7] * b[13] + a[11] * b[14] + a[15] * b[15];
}

void Mat4MultiplyVec3(vec3_t r, mat4_t a, vec3_t b)
{
	r[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12];
	r[ 1] = a[ 1] * b[ 0] + a[ 5] * b[ 1] + a[ 9] * b[ 2] + a[13];
	r[ 2] = a[ 2] * b[ 0] + a[ 6] * b[ 1] + a[10] * b[ 2] + a[14];
}

void Mat4MultiplyVec4(vec4_t r, mat4_t a, vec4_t b)
{
	r[ 0] = a[ 0] * b[ 0] + a[ 4] * b[ 1] + a[ 8] * b[ 2] + a[12] * b[ 3];
	r[ 1] = a[ 1] * b[ 0] + a[ 5] * b[ 1] + a[ 9] * b[ 2] + a[13] * b[ 3];
	r[ 2] = a[ 2] * b[ 0] + a[ 6] * b[ 1] + a[10] * b[ 2] + a[14] * b[ 3];
	r[ 3] = a[ 3] * b[ 0] + a[ 7] * b[ 1] + a[11] * b[ 2] + a[15] * b[ 3];
}

void Mat4InvertSimple(mat4_t r, mat4_t a)
{
	r[ 0] = a[ 0]; r[ 4] = a[ 1]; r[ 8] = a[ 2]; r[12] = -(r[ 0] * a[12] + r[ 4] * a[13] + r[ 8] * a[14]);
	r[ 1] = a[ 4]; r[ 5] = a[ 5]; r[ 9] = a[ 6]; r[13] = -(r[ 0] * a[12] + r[ 4] * a[13] + r[ 8] * a[14]);
	r[ 2] = a[ 8]; r[ 6] = a[ 9]; r[10] = a[10]; r[14] = -(r[ 0] * a[12] + r[ 4] * a[13] + r[ 8] * a[14]);
	r[ 3] = 0.0f;  r[ 7] = 0.0f;  r[11] = 0.0f;  r[15] = 1.0f;
	
	
}

void Mat4Model(mat4_t r, vec3_t right, vec3_t up, vec3_t back, vec3_t pos)
{
	r[ 0] = right[0];
	r[ 1] = right[1];
	r[ 2] = right[2];
	r[ 3] = 0.0f;

	r[ 4] = up[0];
	r[ 5] = up[1];
	r[ 6] = up[2];
	r[ 7] = 0.0f;

	r[ 8] = back[0];
	r[ 9] = back[1];
	r[10] = back[2];
	r[11] = 0.0f;

	r[12] = pos[0];
	r[13] = pos[1];
	r[14] = pos[2];
	r[15] = 1.0f;
}

void Mat4View(mat4_t view, vec3_t facing, vec3_t up, vec3_t pos)
{
	vec3_t right;
	
	CrossProduct3(right, facing, up);
	Vec3Normalize(right);
	CrossProduct3(up, right, facing);
	Vec3Normalize(up);
	
	view[ 0] = right[0];
	view[ 1] = up[0];
	view[ 2] = -facing[0];
	view[ 3] = 0.0f;

	view[ 4] = right[1];
	view[ 5] = up[1];
	view[ 6] = -facing[1];
	view[ 7] = 0.0f;

	view[ 8] = right[2];
	view[ 9] = up[2];
	view[10] = -facing[2];
	view[11] = 0.0f;

	view[12] = -(view[ 0] * pos[0] + view[ 4] * pos[1] + view[ 8] * pos[2]);
	view[13] = -(view[ 1] * pos[0] + view[ 5] * pos[1] + view[ 9] * pos[2]);
	view[14] = -(view[ 2] * pos[0] + view[ 6] * pos[1] + view[10] * pos[2]);
	view[15] = 1.0f;
}

void Mat4Ortho(mat4_t r, vec_t left, vec_t right, vec_t top, vec_t bottom, vec_t nearPlane, vec_t farPlane)
{
	r[ 0] = 2.0f / (right - left);
	r[ 1] = 0.0f;
	r[ 2] = 0.0f;
	r[ 3] = 0.0f;
	
	r[ 4] = 0.0f;
	r[ 5] = 2.0f / (top - bottom);
	r[ 6] = 0.0f;
	r[ 7] = 0.0f;

	r[ 8] = 0.0f;
	r[ 9] = 0.0f;
	r[10] = -2.0f / (farPlane - nearPlane);
	r[11] = 0.0f;

	r[12] = -(right + left) / (right - left);
	r[13] = -(top + bottom) / (top - bottom);
	r[14] = -(farPlane + nearPlane) / (farPlane - nearPlane);
	r[15] = 1.0f;
	
}

void Mat4Perspective(mat4_t r, vec_t width, vec_t height, vec_t nearPlane, vec_t farPlane)
{
	vec_t nmf = nearPlane - farPlane;
	
	r[ 0] = nearPlane * 2.0f / width;
	r[ 1] = 0.0f;
	r[ 2] = 0.0f;
	r[ 3] = 0.0f;
	
	r[ 4] = 0.0f;
	r[ 5] = nearPlane * 2.0f / height;
	r[ 6] = 0.0f;
	r[ 7] = 0.0f;

	r[ 8] = 0.0f;
	r[ 9] = 0.0f;
	r[10] = (farPlane + nearPlane) / nmf;
	r[11] = -1.0f;
	
	r[12] = 0.0f;
	r[13] = 0.0f;
	r[14] = (2.0f * farPlane * nearPlane) / nmf;
	r[15] = 0.0f;
}

// assumes matrix is a rotation/scale followed by a translation
void Mat4InvertSimple2(mat4_t r, mat4_t a)
{
	float scale, *v;
	
	r[ 0] = a[ 0]; r[ 4] = a[ 1]; r[ 8] = a[ 2];
	r[ 1] = a[ 4]; r[ 5] = a[ 5]; r[ 9] = a[ 6]; 
	r[ 2] = a[ 8]; r[ 6] = a[ 9]; r[10] = a[10];
	r[ 3] = 0.0f;  r[ 7] = 0.0f;  r[11] = 0.0f;
	
	v = a + 0; scale = 1.0f / DotProduct3(v, v); r[ 0] *= scale; r[ 4] *= scale; r[ 8] *= scale; 
	v = a + 4; scale = 1.0f / DotProduct3(v, v); r[ 1] *= scale; r[ 5] *= scale; r[ 9] *= scale; 
	v = a + 8; scale = 1.0f / DotProduct3(v, v); r[ 2] *= scale; r[ 6] *= scale; r[10] *= scale; 
	
	r[12] = -(r[ 0] * a[12] + r[ 4] * a[13] + r[ 8] * a[14]);
	r[13] = -(r[ 1] * a[12] + r[ 5] * a[13] + r[ 9] * a[14]);
	r[14] = -(r[ 2] * a[12] + r[ 6] * a[13] + r[10] * a[14]);
	r[15] = 0.0f;
}

void Mat4Dump(mat4_t a)
{
	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
	       a[ 0], a[ 4], a[ 8], a[12],
	       a[ 1], a[ 5], a[ 9], a[13],
	       a[ 2], a[ 6], a[10], a[14],
	       a[ 3], a[ 7], a[11], a[15]);
}

void Mat4x3_FromTranslateRotateScale(mat4x3_t m, vec3_t translate, quat_t rotation, vec3_t scale)
{
	vec_t xx, xy, xz, xw, yy, yz, yw, zz, zw;
	quat_t nq;

	Vec4Copy(nq, rotation);
	Vec4Normalize(nq);

	xx = nq[0] * nq[0];
	xy = nq[0] * nq[1];
	xz = nq[0] * nq[2];
	xw = nq[0] * nq[3];

	yy = nq[1] * nq[1];
	yz = nq[1] * nq[2];
	yw = nq[1] * nq[3];

	zz = nq[2] * nq[2];
	zw = nq[2] * nq[3];

	m[0] = 0.5f - yy - zz;
	m[1] =        xy + zw;
	m[2] =        xz - yw;

	m[3] =        xy - zw;
	m[4] = 0.5f - xx - zz;
	m[5] =        yz + xw;

	m[6] =        xz + yw;
	m[7] =        yz - xw;
	m[8] = 0.5f - xx - yy;

	Vec3Scale(&m[0], 2.0f * scale[0], &m[0]);
	Vec3Scale(&m[3], 2.0f * scale[1], &m[3]);
	Vec3Scale(&m[6], 2.0f * scale[2], &m[6]);

	Vec3Copy(&m[9], translate);
}

void Mat4x3_Copy(mat4x3_t r, mat4x3_t a)
{
	r[ 0] = a[ 0];
	r[ 1] = a[ 1];
	r[ 2] = a[ 2];

	r[ 3] = a[ 3];
	r[ 4] = a[ 4];
	r[ 5] = a[ 5];

	r[ 6] = a[ 6];
	r[ 7] = a[ 7];
	r[ 8] = a[ 8];

	r[ 9] = a[ 9];
	r[10] = a[10];
	r[11] = a[11];
}

void Mat4x3_Multiply(mat4x3_t r, mat4x3_t a, mat4x3_t b)
{
	r[ 0] = a[0] * b[0] + a[3] * b[ 1] + a[6] * b[ 2];
	r[ 1] = a[1] * b[0] + a[4] * b[ 1] + a[7] * b[ 2];
	r[ 2] = a[2] * b[0] + a[5] * b[ 1] + a[8] * b[ 2];

	r[ 3] = a[0] * b[3] + a[3] * b[ 4] + a[6] * b[ 5];
	r[ 4] = a[1] * b[3] + a[4] * b[ 4] + a[7] * b[ 5];
	r[ 5] = a[2] * b[3] + a[5] * b[ 4] + a[8] * b[ 5];

	r[ 6] = a[0] * b[6] + a[3] * b[ 7] + a[6] * b[ 8];
	r[ 7] = a[1] * b[6] + a[4] * b[ 7] + a[7] * b[ 8];
	r[ 8] = a[2] * b[6] + a[5] * b[ 7] + a[8] * b[ 8];

	r[ 9] = a[0] * b[9] + a[3] * b[10] + a[6] * b[11] + a[ 9];
	r[10] = a[1] * b[9] + a[4] * b[10] + a[7] * b[11] + a[10];
	r[11] = a[2] * b[9] + a[5] * b[10] + a[8] * b[11] + a[11];
}

void Mat4x3_MultiplyVec3(vec3_t r, mat4x3_t a, vec3_t b)
{
	r[ 0] = a[0] * b[0] + a[3] * b[ 1] + a[6] * b[ 2];
	r[ 1] = a[1] * b[0] + a[4] * b[ 1] + a[7] * b[ 2];
	r[ 2] = a[2] * b[0] + a[5] * b[ 1] + a[8] * b[ 2];
}

void Mat4x3_MultiplyVec4(vec4_t r, mat4x3_t a, vec3_t b)
{
	r[ 0] = a[0] * b[0] + a[3] * b[ 1] + a[6] * b[ 2] + a[ 9];
	r[ 1] = a[1] * b[0] + a[4] * b[ 1] + a[7] * b[ 2] + a[10];
	r[ 2] = a[2] * b[0] + a[5] * b[ 1] + a[8] * b[ 2] + a[11];
}


void Mat4x3_InvertSimple(mat4x3_t r, mat4x3_t a)
{
	vec_t s;

	s = 1.0f / DotProduct3( a,     a);    r[0] = a[0] * s; r[3] = a[1] * s; r[6] = a[2] * s;
	s = 1.0f / DotProduct3(&a[3], &a[3]); r[1] = a[3] * s; r[4] = a[4] * s; r[7] = a[5] * s;
	s = 1.0f / DotProduct3(&a[6], &a[6]); r[2] = a[6] * s; r[5] = a[7] * s; r[8] = a[8] * s;

	r[ 9] = -(r[0] * a[ 9] + r[3] * a[10] + r[6] * a[11]);
	r[10] = -(r[1] * a[ 9] + r[4] * a[10] + r[7] * a[11]);
	r[11] = -(r[2] * a[ 9] + r[5] * a[10] + r[8] * a[11]);
}

// derived from http://en.wikipedia.org/wiki/Barycentric_coordinate_system
int Vec2PointInTriangle(vec2_t point, vec2_t t1, vec2_t t2, vec2_t t3)
{
	vec_t a, b, c, d;

	a = (t2[1] - t3[1]) * (point[0] - t3[0]) + (t3[0] - t2[0]) * (point[1] - t3[1]);
	b = (t3[1] - t1[1]) * (point[0] - t3[0]) + (t1[0] - t3[0]) * (point[1] - t3[1]);
	d = (t2[1] - t3[1]) * (t1[0]    - t3[0]) + (t3[0] - t2[0]) * (t1[1]    - t3[1]);

#if 0
	if (d == 0.0f)
		return 0;
#endif

	if (d < 0.0f)
	{
		a = -a;
		b = -b;
		d = -d;
	}

	c = a + b;

	if ((a < 0.0f) || (b < 0.0f) || (c < 0.0f) || (a > d) || (b > d) || (c > d))
		return 0;

	return 1;
}

// Line-plane intersection
// based on http://en.wikipedia.org/wiki/Line-plane_intersection
char LinePlaneIntersection(line_t line, plane_t plane, vec_t *distance)
{
	vec_t ldotn;
	vec3_t p0, l2p;

	ldotn = DotProduct3(line.dir, plane);

#if 0 // no backface cull
	if (fabs(ldotn) < EPSILON)
	{
		return 0;
	}
#else // backface cull
	if (ldotn > 0.0f)
	{
		return 0;
	}
#endif

	Vec3Scale(p0, plane[3], plane);

	Vec3Subtract(l2p, p0, line.pos);

	*distance = DotProduct3(plane, l2p) / ldotn;

	return 1;
}

char RayPlaneIntersection(ray_t ray, plane_t plane, vec_t *distance)
{
	char result = LinePlaneIntersection(ray, plane, distance);

	if (!result || (*distance < 0.0f))
		return 0;

	return 1;
}

// Line-sphere intersection
// based on http://en.wikipedia.org/wiki/Line-sphere_intersection
//
// returns number of intersections, 0, 1, or 2, and distances in dist1 and dist2
// dist1 is always lower than dist2
int LineSphereIntersection(line_t line, sphere_t sphere, vec_t *dist1, vec_t *dist2)
{
	vec3_t c;
	vec_t cc, rr, ldotc, dd, d;

	Vec3Subtract(c, sphere, line.pos);
	rr = sphere[3] * sphere[3];
	cc = DotProduct3(c, c);
	ldotc = DotProduct3(line.dir, c);

	dd = ldotc * ldotc - cc + rr;

	if (dd < 0.0f)
	{
		// no solutions
		return 0;
	}

	// should sqrt dd first, but this is good enough
	if (dd < EPSILON)
	{
		// one solution
		*dist1 = ldotc;
		return 1;
	}

	// two solutions
	d = sqrt(dd);

	*dist1 = ldotc - d;
	*dist2 = ldotc + d;

	return 2;
}


char RaySphereIntersection(ray_t ray, sphere_t sphere, vec_t *distance)
{
	vec_t dist1, dist2;
	int numIntersects;
	
	numIntersects = LineSphereIntersection(ray, sphere, &dist1, &dist2);
	
	if (numIntersects == 0)
	{
		return 0;
	}
	else if (numIntersects == 1)
	{
		if (dist1 < 0.0f)
		{
			return 0;
		}

		*distance = dist1;
		return 1;
	}
	// else if (numIntersects == 2)

	if (dist2 < 0.0f)
	{
		return 0;
	}
	
	if (dist1 < 0.0f)
	{
		*distance = dist2;
	}
	else
	{
		*distance = dist1;
	}
	
	return 1;
}

char RayTriangleIntersection(ray_t ray, triangle_t triangle, plane_t triPlane, vec4_t triCache, vec_t *distance)
{
	vec3_t u, v, w, I;
	vec_t uu, uv, vv, wu, wv, iD, s, t;
	plane_t plane;

#if 0 // assuming plane is always given
	if (!triPlane)
	{
		Vec3Subtract(u, triangle[1], triangle[0]);
		Vec3Subtract(v, triangle[2], triangle[0]);
		CrossProduct3(plane, u, v);
		Vec3Normalize(plane);
		plane[3] = DotProduct3(triangle[0], plane);
	}
	else
#endif
	{
		Vec4Copy(plane, triPlane);
	}

#if 0 // assuming no degenerate triangles
	if (DotProduct3(plane, plane) < EPSILON)
		return 0;
#endif

#if 0 // assuming RayPlaneIntersection does a backface cull
	if (DotProduct3(ray.dir, plane) > 0.0f)
		return 0;
#endif
	
#if 0 // integrated plane intersection here
	if (!RayPlaneIntersection(ray, plane, distance))
		return 0;
#else
	{
		vec_t ldotn;
		vec3_t p0, l2p;

		ldotn = DotProduct3(ray.dir, plane);
        
		if (ldotn > 0.0f)
		{
			return 0;
		}

		Vec3Scale(p0, plane[3], plane);

		Vec3Subtract(l2p, p0, ray.pos);

		*distance = DotProduct3(plane, l2p) / ldotn;

		if (*distance < 0.0f)
			return 0;
	}
#endif
	
#if 0 // assuming plane is always given
	if (triPlane)
#endif
	{
		Vec3Subtract(u, triangle[1], triangle[0]);
		Vec3Subtract(v, triangle[2], triangle[0]);
	}

#if 0 // assuming triCache is always given
	if (!triCache)
	{
		uu = DotProduct3(u, u);
		uv = DotProduct3(u, v);
		vv = DotProduct3(v, v);
		iD = 1.0f / (uv * uv - uu * vv);
	}
	else
#endif
	{
		uu = triCache[0];
		uv = triCache[1];
		vv = triCache[2];
		iD = triCache[3];
	}
	
	Vec3MultiplyAdd(I, ray.pos, *distance, ray.dir);
	Vec3Subtract(w, I, triangle[0]);
	wu = DotProduct3(w, u);
	wv = DotProduct3(w, v);

	s = (uv * wv - vv * wu) * iD;
	if (s < 0.0f || s > 1.0f)
		return 0;
	t = (uv * wu - uu * wv) * iD;
	if (t < 0.0f || (s + t) > 1.0f)
		return 0;

	return 1;
}

void TriangleCalcCaches(triangle_t triangle, plane_t plane, vec4_t triCache)
{
	vec3_t u, v;
	vec_t uu, uv, vv, iD;
	
	Vec3Subtract(u, triangle[1], triangle[0]);
	Vec3Subtract(v, triangle[2], triangle[0]);
	CrossProduct3(plane, u, v);
	Vec3Normalize(plane);
	plane[3] = DotProduct3(triangle[0], plane);
	
	uu = DotProduct3(u, u);
	uv = DotProduct3(u, v);
	vv = DotProduct3(v, v);
	iD = 1.0f / (uv * uv - uu * vv);
	triCache[0] = uu;
	triCache[1] = uv;
	triCache[2] = vv;	
	triCache[3] = iD;
}

void Aabb_Clear(aabb_t aabb)
{
	// FIXME: may want larger ranges than this some day
	Vec3Set(aabb[0],  99999999.0f,  99999999.0f,  99999999.0f);
	Vec3Set(aabb[1], -99999999.0f, -99999999.0f, -99999999.0f);
}

void Aabb_SetToPoint(aabb_t aabb, vec3_t point)
{
	Vec3Copy(aabb[0], point);
	Vec3Copy(aabb[1], point);
}

void Aabb_AddPoint(aabb_t aabb, vec3_t point)
{
	if (point[0] < aabb[0][0])
		aabb[0][0] = point[0];
	if (point[1] < aabb[0][1])
		aabb[0][1] = point[1];
	if (point[2] < aabb[0][2])
		aabb[0][2] = point[2];

	if (point[0] > aabb[1][0])
		aabb[1][0] = point[0];
	if (point[1] > aabb[1][1])
		aabb[1][1] = point[1];
	if (point[2] > aabb[1][2])
		aabb[1][2] = point[2];
}

void Aabb_Copy(aabb_t a, aabb_t b)
{
	Vec3Copy(a[0], b[0]);
	Vec3Copy(a[1], b[1]);
}

void Aabb_Add(aabb_t r, aabb_t a, aabb_t b)
{
	if (r == a)
	{
		Aabb_AddPoint(r, b[0]);
		Aabb_AddPoint(r, b[1]);
	}
	else if (r == b)
	{
		Aabb_AddPoint(r, a[0]);
		Aabb_AddPoint(r, a[1]);
	}
	else
	{
		Aabb_Copy(r, a);
		Aabb_AddPoint(r, b[0]);
		Aabb_AddPoint(r, b[1]);
	}
}

int Aabb_Intersect(aabb_t a, aabb_t b)
{
	if (a[1][0] <= b[0][0]
	 || a[0][0] >= b[1][0]
	 || a[1][1] <= b[0][1]
	 || a[0][1] >= b[1][1]
	 || a[1][2] <= b[0][2]
	 || a[0][2] >= b[1][2])
	{
		return 0;
	}

	return 1;
}

int Aabb_SweepCollision(aabb_t bounds1, vec3_t move1, aabb_t bounds2, vec3_t move2, vec_t *impactTime)
{
	// derived from http://www.gamasutra.com/view/feature/131790/simple_intersection_tests_for_games.php?page=3
	vec3_t move, firstOverlap, lastOverlap;
	vec_t first, last;
	aabb_t bounds2andMove;
	int i;

	if (Aabb_Intersect(bounds1, bounds2))
	{
		if (impactTime)
			*impactTime = 0.0f;
		return 1;
	}

	if (move1)
		Vec3Subtract(move, move2, move1);
	else
		Vec3Copy(move, move2);

	// test bounds and move first
	// 
	Vec3Copy(bounds2andMove[0], bounds2[0]);
	Vec3Copy(bounds2andMove[1], bounds2[1]);

	bounds2andMove[move[0] < 0.0f ? 0 : 1][0] += move[0];
	bounds2andMove[move[1] < 0.0f ? 0 : 1][1] += move[1];
	bounds2andMove[move[2] < 0.0f ? 0 : 1][2] += move[2];

	if (!Aabb_Intersect(bounds1, bounds2andMove))
		return 0;

	Vec3Set(firstOverlap, 0.0f, 0.0f, 0.0f);
	Vec3Set(lastOverlap,  1.0f, 1.0f, 1.0f);

	for (i = 0; i < 3; i++)
	{
		if (bounds1[1][i] < bounds2[0][i] && move[i] < 0.0f)
		{
			firstOverlap[i] = (bounds1[1][i] - bounds2[0][i]) / move[i];
		}
		else if (bounds2[1][i] < bounds1[0][i] && move[i] > 0.0f)
		{
			firstOverlap[i] = (bounds1[0][i] - bounds2[1][i]) / move[i];
		}
		
		if (bounds2[1][i] > bounds1[0][i] && move[i] < 0.0f)
		{
			lastOverlap[i] = (bounds1[0][i] - bounds2[1][i]) / move[i];
		}
		else if (bounds1[1][i] > bounds2[0][i] && move[i] > 0.0f)
		{
			lastOverlap[i] = (bounds1[1][i] - bounds2[0][i]) / move[i];
		}
	}

	first = MAX(firstOverlap[0], MAX(firstOverlap[1], firstOverlap[2]));
	last  = MIN(lastOverlap[0],  MIN(lastOverlap[1],  lastOverlap[2] ));

	if (first < last)
	{
		if (impactTime)
			*impactTime = first;
		return 1;
	}

	return 0;
}

void Aabb_ToPosAndSize(vec3_t pos, vec3_t size, aabb_t aabb)
{
	Vec3Add(pos, aabb[0], aabb[1]);
	Vec3Scale(pos, 0.5f, pos);
	
	Vec3Subtract(size, aabb[1], aabb[0]);
}

void Aabb_FromPosAndSize(aabb_t aabb, vec3_t pos, vec3_t size)
{
	Vec3MultiplyAdd(aabb[0], pos, -0.5f, size);
	Vec3Add(aabb[1], aabb[0], size);
}

// from Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html
void CalcTexVectors(vec3_t sdir, vec3_t tdir, vec3_t v1, vec3_t v2, vec3_t v3, vec2_t w1, vec2_t w2, vec2_t w3)
{
	vec_t x1, x2, y1, y2, z1, z2, s1, s2, t1, t2, r;

	x1 = v2[0] - v1[0];
	x2 = v3[0] - v1[0];
	y1 = v2[1] - v1[1];
	y2 = v3[1] - v1[1];
	z1 = v2[2] - v1[2];
	z2 = v3[2] - v1[2];

	s1 = w2[0] - w1[0];
	s2 = w3[0] - w1[0];
	t1 = w2[1] - w1[1];
	t2 = w3[1] - w1[1];

	r = 1.0f / (s1 * t2 - s2 * t1);

	sdir[0] = (t2 * x1 - t1 * x2) * r;
	sdir[1] = (t2 * y1 - t1 * y2) * r;
	sdir[2] = (t2 * z1 - t1 * z2) * r;

	tdir[0] = (s1 * x2 - s2 * x1) * r;
	tdir[1] = (s1 * y2 - s2 * y1) * r;
	tdir[2] = (s1 * z2 - s2 * z1) * r;
}

// from Lengyel, Eric. “Computing Tangent Space Basis Vectors for an Arbitrary Mesh”. Terathon Software 3D Graphics Library, 2001. http://www.terathon.com/code/tangent.html
vec_t CalcTangentSpace(vec3_t tangent, vec3_t bitangent, vec3_t normal, vec3_t sdir, vec3_t tdir)
{
	vec3_t n_cross_t;
	vec_t n_dot_t, handedness;

	// Gram-Schmidt orthogonalize
	n_dot_t = DotProduct3(normal, sdir);
	Vec3MultiplyAdd(tangent, sdir, -n_dot_t, normal);
	Vec3Normalize(tangent);

	// Calculate handedness
	CrossProduct3(n_cross_t, normal, sdir);
	handedness = (DotProduct3(n_cross_t, tdir) < 0.0f) ? -1.0f : 1.0f;

	// Calculate orthogonal bitangent, if necessary
	if (bitangent)
		CrossProduct3(bitangent, normal, tangent);

	return handedness;
}

void QuatFromAxes(quat_t q, vec3_t x, vec3_t y, vec3_t z)
{
	// from http://www.cg.info.hiroshima-cu.ac.jp/~miyazaki/knowledge/teche52.html
	q[0] =  x[0] - y[1] - z[2] + 1.0f;
	q[1] = -x[0] + y[1] - z[2] + 1.0f;
	q[2] = -x[0] - y[1] + z[2] + 1.0f;
	q[3] =  x[0] + y[1] + z[2] + 1.0f;

	q[0] = (q[0] < 0.0f) ? 0.0f : sqrt(q[0]) * 0.5f;
	q[1] = (q[1] < 0.0f) ? 0.0f : sqrt(q[1]) * 0.5f;
	q[2] = (q[2] < 0.0f) ? 0.0f : sqrt(q[2]) * 0.5f;
	q[3] = (q[3] < 0.0f) ? 0.0f : sqrt(q[3]) * 0.5f;

	if (y[2] < z[1]) q[0] = -q[0];
	if (z[0] < x[2]) q[1] = -q[1];
	if (x[1] < y[0]) q[2] = -q[2];
}

void AxesFromQuat(vec3_t x, vec3_t y, vec3_t z, quat_t q)
{
	vec_t xx, xy, xz, xw, yy, yz, yw, zz, zw;

	xx = q[0] * q[0];
	xy = q[0] * q[1];
	xz = q[0] * q[2];
	xw = q[0] * q[3];

	yy = q[1] * q[1];
	yz = q[1] * q[2];
	yw = q[1] * q[3];

	zz = q[2] * q[2];
	zw = q[2] * q[3];

	x[0] = 0.5f - yy - zz;
	x[1] =        xy + zw;
	x[2] =        xz - yw;

	y[0] =        xy - zw;
	y[1] = 0.5f - xx - zz;
	y[2] =        yz + xw;

	z[0] =        xz + yw;
	z[1] =        yz - xw;
	z[2] = 0.5f - xx - yy;

	Vec3Scale(x, 2.0f, x);
	Vec3Scale(y, 2.0f, y);
	Vec3Scale(z, 2.0f, z);
}

void AxesFromPitchYaw(vec3_t x, vec3_t y, vec3_t z, vec_t radYaw, 
                      vec_t radPitch)
{
	vec_t cosYaw, sinYaw;

	cosYaw = cos(radYaw);
	sinYaw = sin(radYaw);

	if (x)
		Vec3Set(x,  cosYaw, 0.0f,  sinYaw);
	
	if (z)
		Vec3Set(z, -sinYaw, 0.0f,  cosYaw);

	if (radPitch)
	{
		vec_t cosPitch, sinPitch;

		cosPitch = cos(radPitch);
		sinPitch = sin(radPitch);
		
		if (z)
		{
			z[0] *=  cosPitch;
			z[1]  = -sinPitch;
			z[2] *=  cosPitch;
		}
		
		if (y)
		{
			//CrossProduct3(y, z, x);
			y[0] = -sinYaw * sinPitch;
			y[1] = cosPitch;
			y[2] = cosYaw * sinPitch;
		}
	}
	else if (y)
	{
		Vec3Set(y, 0.0f, 1.0f, 0.0f);
	}
}

int PointInsidePlane(vec3_t point, plane_t plane)
{
	return DotProduct3(point, plane) < plane[3];
}

int SweepLineWithPlane(vec3_t startPos, vec3_t move, plane_t plane, vec_t *hitFraction)
{
	vec3_t endPos;
	char startInside, endInside;
	
	Vec3Add(endPos, startPos, move);
	
	startInside = PointInsidePlane(startPos, plane);
	endInside   = PointInsidePlane(endPos,   plane);
	
	if (!startInside && !endInside)
	{
		return SWEEP_ALL_OUT;
	}

	if (startInside && endInside)
	{
		return SWEEP_ALL_IN;
	}
	
	// determine intersection point
	// based on http://en.wikipedia.org/wiki/Line-plane_intersection
	if (hitFraction)
	{
		vec3_t p0, l2p;
		vec_t ldotn;
		
		ldotn = DotProduct3(move, plane);
		Vec3Scale(p0, plane[3], plane);
		Vec3Subtract(l2p, p0, startPos);
		*hitFraction = DotProduct3(plane, l2p) / ldotn;
	}

	if (startInside && !endInside)
		return SWEEP_IN_TO_OUT;

	return SWEEP_OUT_TO_IN;

}

int SweepAabbWithPlane(aabb_t aabb, vec3_t move, plane_t plane, vec_t *hitFraction)
{
	vec3_t startPos;

	// determine closest aabb corner using plane normal signs
	startPos[0] = aabb[(plane[0] < 0.0f) ? 1 : 0][0];
	startPos[1] = aabb[(plane[1] < 0.0f) ? 1 : 0][1];
	startPos[2] = aabb[(plane[2] < 0.0f) ? 1 : 0][2];

	return SweepLineWithPlane(startPos, move, plane, hitFraction);
}

void ConvexHullFromAabb(convexHull_t *ch, aabb_t aabb)
{
	int i;

	assert(ch->planes != NULL);
	assert(ch->numPlanes == 6);

	for (i = 0; i < 6; i++)
	{
		vec3_t p0;
		
		// calculate plane normal
		Vec3Set(ch->planes[i], 0.0f, 0.0f, 0.0f);
		ch->planes[i][i / 2] = (i & 1) ? -1.0f : 1.0f;
		
		// get a point on plane based on bounds and plane normal signs
		p0[0] = aabb[(ch->planes[i][0] < 0.0f) ? 0 : 1][0];
		p0[1] = aabb[(ch->planes[i][1] < 0.0f) ? 0 : 1][1];
		p0[2] = aabb[(ch->planes[i][2] < 0.0f) ? 0 : 1][2];
		
		// calculate plane distance from origin
		ch->planes[i][3] = DotProduct3(p0, ch->planes[i]);
	}

	Vec3Copy(ch->aabb[0], aabb[0]);
	Vec3Copy(ch->aabb[1], aabb[1]);
}

int SweepAabbWithConvexHull(aabb_t aabb, vec3_t move, convexHull_t *ch, vec_t *hitFraction, plane_t hitPlane)
{
	vec_t closestFraction = 1.0f;
	vec_t minFraction = 1.0f;
	plane_t closestHitPlane;
	int i;

	if (!Aabb_SweepCollision(ch->aabb, NULL, aabb, move, &minFraction))
		return 0;

	for (i = 0; i < ch->numPlanes; i++)
	{
		vec_t fraction;
		
		switch(SweepAabbWithPlane(aabb, move, ch->planes[i], &fraction))
		{
			case SWEEP_ALL_OUT:
				return 0;
				break;

			case SWEEP_IN_TO_OUT:
				break;
				
			case SWEEP_OUT_TO_IN:
				if (fraction < closestFraction)
				{
					Vec4Copy(closestHitPlane, ch->planes[i]);
					closestFraction = fraction;
				}
				break;
				
			case SWEEP_ALL_IN:
				break;
			
			default:
				break;
		}
	}
	
	if (closestFraction < minFraction)
		closestFraction = minFraction;

	if (closestFraction < 1.0f)
	{
		if (hitFraction)
			*hitFraction = closestFraction;
		if (hitPlane)
			Vec4Copy(hitPlane, closestHitPlane);

		return 1;
	}

	return 0;
}

// Converts bounds1 into a convex hull and combines move1 into move2
int Aabb_SweepCollision2(aabb_t bounds1, vec3_t move1, aabb_t bounds2, vec3_t move2, vec_t *hitDistance, plane_t hitPlane)
{
	vec3_t move;
	convexHull_t ch;
	plane_t chPlanes[6];

#if 0	
	if (Aabb_Intersect(bounds1, bounds2))
	{
		if (hitDistance)
			*hitDistance = 0.0f;
		return 1;
	}
#endif
	
	ch.planes = chPlanes;
	ch.numPlanes = 6;
	
	ConvexHullFromAabb(&ch, bounds1);

	if (move1)
		Vec3Subtract(move, move2, move1);
	else
		Vec3Copy(move, move2);

	// FIXME: push hitplane by move1 * hitDistance, assumes |move1| == 0.
	return SweepAabbWithConvexHull(bounds2, move, &ch, hitDistance, hitPlane);
}

int CheckAndResolveCollision(aabb_t staticBounds, aabb_t movingBounds, vec3_t correction)
{
	int i;
	
	if (!Aabb_Intersect(staticBounds, movingBounds))
		return 0;

	for (i = 0; i < 3; i++)
	{
		vec_t pos, neg;
		
		pos = staticBounds[1][i] - movingBounds[0][i];
		neg = staticBounds[0][i] - movingBounds[1][i];
		
		if (pos < -neg)
			correction[i] = pos;
		else
			correction[i] = neg;
	}
	
	if (abs(correction[0]) < abs(correction[1]))
	{
		if (abs(correction[2]) < abs(correction[0]))
		{
			correction[0] = 0.0f;
			correction[1] = 0.0f;
		}
		else
		{
			correction[1] = 0.0f;
			correction[2] = 0.0f;
		}
	}
	else // correction[0] >= correction[1]
	{
		if (abs(correction[2]) < abs(correction[1]))
		{
			correction[0] = 0.0f;
			correction[1] = 0.0f;
		}
		else
		{
			correction[0] = 0.0f;
			correction[2] = 0.0f;
		}
	}
	
	return 1;
}

#endif
