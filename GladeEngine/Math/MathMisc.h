#ifndef GLADE_MATH_MISC_H
#define GLADE_MATH_MISC_H
#include "../Math/Precision.h"
#include <stdlib.h>

namespace Glade {

#define G_FLT_SMALL	1e-5		// Small default tolerance value
#define G_FLT_LARGE	1e5			// Large default tolerance value

// Return 'val' squared
template <typename T>
inline T Square(T val) { return val * val; }

// Return 'val' cubed
template <typename T>
inline T Cube(T val) { return val * val * val; }

// Return if two numbers have the same sign
inline bool SameSign(int a, int b) { return (a >= 0) ^ (b < 0); }
template <typename T>
inline bool SameSign(T a, T b) { return (a < 0) == (b < 0); }

// Return unit value with same sign as passed parameter
template <typename T>
inline T Sign(T a) { return a < T(0) ? T(-1) : T(1); }

// Pythagorean Theorem - a^2 + b^2 = c^2
inline gFloat PythagoreanSolve(gFloat a, gFloat b) { return Sqrt(a*a + b*b); }
inline gFloat PythagoreanSolve(gFloat a, gFloat b, gFloat c) { return Sqrt(a*a + b*b + c*c); }
inline gFloat PythagoreanSquaredSolve(gFloat a, gFloat b) { return a*a + b*b; }
inline gFloat PythagoreanSquaredSolve(gFloat a, gFloat b, gFloat c) { return a*a + b*b + c*c; }

// Clamp - Modify/Return value clamped to a min and max
template <typename T>
inline T Clamp(T& val, T min, T max)
{
	if(val < min) val = min;
	else if(val > max) val = max;
	return val;
}
template <typename T>	
inline T ClampMin(T& val, T min)	// Clamp only to a minimum value
{
	if(val < min) val = min;
	return val;
}
template <typename T>
inline T ClampMax(T& val, T max)	// Clamp only to a maximum value
{
	if(val > max) val = max;
	return val;
}

template <typename T>
inline T Clamp(T& val, T min, T max, T step)	// Modify 'val' by 'step', then clamp to range [min,max]
{
	val += step;
	if(val < min) val = min;
	else if(val > max) val = max;
	return val;
}

template <typename T>
inline T ClampMin(T& val, T min, T step)	// Lower 'val' by 'step', then clamp to min
{
	val -= step;
	if(val < min) val = min;
	return val;
}

template <typename T>
inline T ClampMax(T& val, T max, T step)	// Raise 'val' by step, then clamp to max
{
	val += step;
	if(val > max) val = max;
	return val;
}

// Wrap - Modify/Return value wrapped between a min a max
template <typename T>
inline T Wrap(T& val, T min, T max)
{
	T delta = max - min;
	while(val < min)
		val += delta;
	while(val > max)
		val -= delta;
	return val;
}

// Wrap - Modify 'val' by 'step', wrap result between a min and max
template <typename T>
inline T Wrap(T& val, T min, T max, T step)
{
	val += step;
	T delta = max - min;
	while(val < min)
		val += delta;
	while(val > max)
		val -= delta;
	return val;
}

// Min/Max - Return minimum/maximum of 2 options
template <typename T>
inline T Min(const T& a, const T& b) { return (a < b ? a : b); }
template <typename T>
inline T Max(const T& a, const T& b) { return (a > b ? a : b); }

// Swap 2 values
template <typename T>
inline void Swap(T& a, T& b) { T temp = a; a = b; b = temp; }
template <typename T>
inline void SwapXOR(T& a, T& b) { a ^= b; b ^= a; a ^= b; }

// Return Inverse of 'val' with no risk of val being '0'
inline gFloat SafeScalarInverse(gFloat val) { return Abs(val) > G_FLT_SMALL ? (1.0f/val) : G_MAX; }

// Return random number between 'a' and 'b'
inline gFloat RangedRandom(gFloat a, gFloat b) { return a + (b-a) * ((gFloat)rand())/((gFloat)RAND_MAX); }

// Test if number is NaN - Not a Number
inline bool IsNaN(gFloat a) { return a != a; }

// Test if two numbers are equal within some tolerance
inline bool ApproximateEqual(gFloat a, gFloat b, gFloat tolerance=G_FLT_SMALL) { return Abs(a-b) < tolerance; }

// Approximate Square Root
inline gFloat ApproximateSquareRoot(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x;			// evil floating point bit level hacking
	i = 0x1fbd1df5 - (i >> 3);		// what the fuck?
	x = *(float*)&i;
	x = x * (1.5f - (xhalf*x*x));
	return gFloat(x);
}

// Approximate Square Root
inline gFloat ApproximateSquareRoot2(gFloat square, gFloat guess, int iterations)
{
	gFloat approx = guess;
	for(; iterations > 0; --iterations)
	{
		approx = guess - ((guess*guess - square) / (2 * guess));
		guess = approx;
	}

	return approx;
}

// Approximate Inverse Square Root (x^(-1/2))
inline gFloat ApproximateInverseSquareRoot(float x)
{
	float xhalf = 0.5f * x;
	int i = *(int*)&x;			// evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1);		// what the fuck?
	x = *(float*)&i;
	x = x * (1.5f - (xhalf*x*x));
	return gFloat(x);
}

// Linear Interpolation
template <typename T>
inline T Lerp(T a, T b, gFloat t)
{
	return a + (b-a)*t;
}

// Linear Interpolation, but value can wrap between min-max (like angles)
template <typename T>
inline T LerpWrap(T a, T b, gFloat t, T min, T max)
{
	T diff = Abs(b - a);
	T range = max - min;
	if(diff > range / 2.0f)
	{
		if(b > a)
			a += range;
		else
			b += range;
	}

	// Interpolate
	T val = a + (b-a) * t;
	if(val >= min && val <= max)
		return val;
	
	return fmod(val, range);
}
}	// namespace
#endif	// GLADE_MATH_MISC_H