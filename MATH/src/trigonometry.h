#pragma once

#include <cmath>

namespace ar
{
	namespace mat
	{
		/// <summary>
		/// Convert an angle given in degrees to radians.
		/// </summary>
		/// <param name="degrees">The angle in degrees.</param>
		/// <returns>The angle in radians.</returns>
		float Radians(const float degrees);

		/// <summary>
		/// Convert an angle given in radians to degrees.
		/// </summary>
		/// <param name="radians">The angle in radians.</param>
		/// <returns>The angle in degrees.</returns>
		float Degrees(const float radians);
	}
	
}