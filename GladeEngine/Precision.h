#ifndef GLADE_PRECISION_H
#define GLADE_PRECISION_H

#include <float.h>
#include <math.h>

namespace Glade
{

#if 1
	#define SINGLE_PRECISION		// Using single-precision mode (gFloat)
	typedef float gFloat;

	#define G_MAX FLT_MAX			// Highest value for real numbers
	#define Sqrt sqrtf				// Square Root function
	#define Abs fabsf				// Absolute Value function
	#define Ceiling ceilf			// Ceiling function
	#define Floor floorf			// Floor function
	#define Sin sinf				// Sine function
	#define Cos cosf				// Cosine function
	#define Tan tanf				// Tangent function
	#define ASin asinf				// Arc Sine function
	#define ACos acosf				// Arc Cosine function
	#define ATan atanf				// Arc Tangent function
	#define ATan2 atan2f			// Arc Tangent function with 2 parameters
	#define Exp expf				// Exponent function (e^x)
	#define Pow powf				// Power function
	#define FMod fmodf				// Floating point modulo function
	#define EPSILON FLT_EPSILON		// e

	// Pi
	#define PI 3.14159f

	// Timestep for each physics update cycle
	// No matter what the framerate of a game/simulation,
	// physics will update once per timestep
	#define PHYSICS_TIMESTEP			0.01666f	// 1/60th of a second
	#define HALF_PHYSICS_TIMESTEP		0.00833f
	#define PHYSICS_TIMESTEP_SQR		0.00028f
	#define HALF_PHYSICS_TIMESTEP_SQR	0.00007f
#else
	#define DOUBLE_PRECISION		// Using double-precision mode (double)
	typedef double gFloat;
	
	#define G_MAX DBL_MAX			// Highest value for real numbers
	#define Sqrt sqrt				// Square Root function
	#define Abs fabs				// Absolute Value function
	#define Ceiling ceil			// Ceiling function
	#define Floor floor				// Floor function
	#define Sin sin					// Sine function
	#define Cos cos					// Cosine function
	#define Tan tan					// Tangent function
	#define ASin asin				// Arc Sine function
	#define ACos acos				// Arc Cosine function
	#define ATan atan				// Arc Tangent function
	#define ATan2 atan2				// Arc Tangent function with 2 parameters
	#define Exp exp					// Exponent function (e^x)
	#define Pow pow					// Power function
	#define FMod fmod				// Floating point modulo function
	#define EPSILON DBL_EPSILON		// e

	// Pi
	#define PI 3.14159265358979

	// Timestep for each physics update cycle
	// No matter what the framerate of a game/simulation,
	// physics will update once per timestep
	#define PHYSICS_TIMESTEP			0.01666666666666	// 1/60th of a second
	#define HALF_PHYSICS_TIMESTEP		0.00833333333333
	#define PHYSICS_TIMESTEP_SQR		0.00027777777777
	#define HALF_PHYSICS_TIMESTEP_SQR	0.00006944444444
#endif
	#define DEG2RAD PI / 180
	#define RAD2DEG 180 / PI

	#define CLAMP(val, high, low) ((val < low) ? low : ((val > high) ? high : val))
	#define MAX(x, y) (x > y ? x : y)
	#define MIN(x, y) (x < y ? x : y)

	#define VERLET
	#define LEFT_HANDED_COORDS
}
#endif	// GLADE_PRECISION_H