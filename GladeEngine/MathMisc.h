#ifndef GLADE_MATH_MISC_H
#define GLADE_MATH_MISC_H
#include "Math\


#define DEG2RAD PI / 180
#define RAD2DEG 180 / PI

#define CLAMP(val, high, low) ((val < low) ? low : ((val > high) ? high : val))
#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)

#endif	// GLADE_MATH_MISC_H