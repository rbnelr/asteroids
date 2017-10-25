
#define PId			3.1415926535897932384626433832795
#define PI			3.1415926535897932384626433832795f

#define TAUd		6.283185307179586476925286766559
#define TAU			6.283185307179586476925286766559f

#define SQRT_2d		1.4142135623730950488016887242097
#define SQRT_2		1.4142135623730950488016887242097f

#define DEG_TO_RADd	0.01745329251994329576923690768489	// 180/PI
#define DEG_TO_RAD	0.01745329251994329576923690768489f

#define RAD_TO_DEGd	57.295779513082320876798154814105	// PI/180
#define RAD_TO_DEG	57.295779513082320876798154814105f

static constexpr f32 to_rad (f32 ang) {	return ang * DEG_TO_RAD; }
static constexpr f32 deg (f32 ang) {	return ang * DEG_TO_RAD; }
static constexpr f32 to_deg (f32 ang) {	return ang * RAD_TO_DEG; }

static constexpr f64 to_rad (f64 ang) {	return ang * DEG_TO_RADd; }
static constexpr f64 degd (f64 ang) {	return ang * DEG_TO_RADd; }
static constexpr f64 to_deg (f64 ang) {	return ang * RAD_TO_DEGd; }

#define RAD_360	deg(360)
#define RAD_180	deg(180)
#define RAD_90	deg(90)

template <typename T> static constexpr
T MIN (T l, T r) { return l <= r ? l : r; }
template <typename T> static constexpr
T MAX (T l, T r) { return l >= r ? l : r; }

static constexpr s32 clamp (s32 val, s32 l, s32 h) {	return MIN( MAX(val,l), h ); }
static constexpr f32 clamp (f32 val, f32 l, f32 h) {	return MIN( MAX(val,l), h ); }
static constexpr f64 clamp (f64 val, f64 l, f64 h) {	return MIN( MAX(val,l), h ); }

static constexpr f32 lerp (f32 a, f32 b, f32 t) {		return a*(1.0f -t) +b*t; }
//static constexpr f32 mapclamp (f32 x, f32 a, f32 b) {		return clamp((x -a)/(b -a), 0.0f,1.0f); }
static constexpr f32 map (f32 x, f32 a, f32 b) {		return (x -a)/(b -a); }

#include <cmath>

using std::abs;
using std::sin;
using std::cos;
#define mod				fmod

template <typename T> static T mymod (T val, T range) {
	#if 1
	T ret = mod(val, range);
	if (range > 0) {
		if (ret < 0) ret += range;
	} else {
		if (ret > 0) ret -= range;
	}
	return ret;
	#endif
}

struct Sin_Cos {
	f32 s, c;
};

Sin_Cos sin_cos (f32 ang) {
	return { sin(ang), cos(ang) };
}
