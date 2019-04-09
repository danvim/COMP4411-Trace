#pragma once
#include <cmath>

/**
 * Interpolates the input to form a smooth gradient.
 * @code
 * f(x)      .**
 *         .*
 *        *
 *       *
 *      *
 *    .*
 * ..*          x
 * @endcode
 */
inline double hermiteInterpolation(const double min, const double max, const double x)
{
	if (x >= max) return 1;
	if (x < min) return 0;
	return -2 * pow((x - min) / (max - min), 3) + 3 * pow((x - min) / (max - min), 2);
}
