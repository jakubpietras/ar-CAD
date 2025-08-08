#pragma once
#include <cmath>
#include <cassert>

namespace ar
{
	namespace mat
	{
		struct Vec4;	// forward declaration
		struct Vec3
		{
			float x, y, z;
			inline const float* Data() const { return &x; };
			inline float* Data() { return &x; };
			constexpr Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
			constexpr Vec3(const Vec4& other);

			/// <summary>
			/// Component-wise sum of two vectors.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns></returns>
			constexpr Vec3 operator+(const Vec3& other) const
			{
				return {x + other.x, y + other.y, z + other.z};
			}

			/// <summary>
			/// Component-wise sum of two vectors.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns></returns>
			constexpr Vec3& operator+=(const Vec3& other)
			{
				x += other.x;
				y += other.y;
				z += other.z;
				return *this;
			}

			/// <summary>
			/// Component-wise difference of two vectors.
			/// </summary>
			/// <param name="other">The vector to subtract.</param>
			/// <returns></returns>
			constexpr Vec3 operator-(const Vec3& other) const
			{
				return { x - other.x, y - other.y, z - other.z };
			}

			/// <summary>
			/// Component-wise difference of two vectors.
			/// </summary>
			/// <param name="other">The vector to subtract.</param>
			/// <returns></returns>
			constexpr Vec3& operator-=(const Vec3& other)
			{
				x -= other.x;
				y -= other.y;
				z -= other.z;
				return *this;
			}

			/// <summary>
			/// Component-wise multiplication by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec3 operator*(const float scalar) const
			{
				return { x * scalar, y * scalar, z * scalar };
			}

			/// <summary>
			/// Multiplies a scalar with a vector (scalar on the left-hand side).
			/// </summary>
			/// <param name="scalar">The scalar multiplier.</param>
			/// <param name="v">The vector being multiplied.</param>
			/// <returns>A scaled vector.</returns>
			constexpr friend Vec3 operator*(float scalar, const Vec3& v);

			/// <summary>
			/// Component-wise multiplication by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec3& operator*=(const float scalar)
			{
				x *= scalar;
				y *= scalar;
				z *= scalar;
				return *this;
			}

			/// <summary>
			/// Component-wise multiplication by a vector.
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			constexpr Vec3 operator*(const Vec3& other) const
			{
				return { x * other.x, y * other.y, z * other.z };
			}

			/// <summary>
			/// Component-wise multiplication by a vector.
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			constexpr Vec3& operator*=(const Vec3& other)
			{
				x *= other.x;
				y *= other.y;
				z *= other.z;
				return *this;
			}

			/// <summary>
			/// Component-wise division by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec3 operator/(const float scalar) const
			{
				assert(scalar != 0.0f && "Vec3 division by zero");
				return { x / scalar, y / scalar, z / scalar };
			}

			/// <summary>
			/// Component-wise division by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec3& operator/=(const float scalar)
			{
				assert(scalar != 0.0f && "Vec3 division by zero");
				x /= scalar;
				y /= scalar;
				z /= scalar;
				return *this;
			}

			/// <summary>
			/// Component-wise division by a vector.
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			constexpr Vec3 operator/(const Vec3& other) const
			{
				assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && "Vec3 component-wise division by zero");
				return { x / other.x, y / other.y, z / other.z };
			}

			/// <summary>
			/// Component-wise division by a vector.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec3& operator/=(const Vec3& other)
			{
				assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && "Vec3 component-wise division by zero");
				x /= other.x;
				y /= other.y;
				z /= other.z;
				return *this;
			}

			/// <summary>
			/// Component-wise inequality comparison operator.
			/// </summary>
			/// <param name="other">The vector to compare.</param>
			/// <returns>True if vectors differ on any component, false otherwise.</returns>
			constexpr bool operator!=(const Vec3& other) const
			{
				return x != other.x || y != other.y || z != other.z;
			}

			/// <summary>
			/// Component-wise equality comparison operator.
			/// </summary>
			/// <param name="other">The vector to compare.</param>
			/// <returns>True if vectors are identical, false otherwise.</returns>
			constexpr bool operator==(const Vec3& other) const
			{
				return x == other.x && y == other.y && z == other.z;
			}

			/// <summary>
			/// Unary negation.
			/// </summary>
			/// <returns>Vector with negated components.</returns>
			constexpr Vec3 operator-() const
			{
				return {-x, -y, -z};
			}
		};


		struct Vec4
		{
			float x, y, z, w;
			const float* Data() const;
			constexpr Vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f)
				: x(x), y(y), z(z), w(w) { }
			constexpr Vec4(const Vec3& v3, float w)
				: x(v3.x), y(v3.y), z(v3.z), w(w) {}

			/// <summary>
			/// Returns the component-wise sum of this vector and another.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns>A new vector representing the sum.</returns>
			constexpr Vec4 operator+(const Vec4& other) const
			{
				return { x + other.x, y + other.y, z + other.z, w + other.w };
			}

			/// <summary>
			/// Adds another vector to this one, component-wise.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns>A reference to this vector after addition.</returns>
			constexpr Vec4& operator+=(const Vec4& other)
			{
				x += other.x;
				y += other.y;
				z += other.z;
				w += other.w;
				return *this;
			}

			/// <summary>
			/// Returns the component-wise difference between this vector and another.
			/// </summary>
			/// <param name="other">The vector to subtract.</param>
			/// <returns>A new vector representing the difference.</returns>
			constexpr Vec4 operator-(const Vec4& other) const
			{
				return { x - other.x, y - other.y, z - other.z, w - other.w };
			}

			/// <summary>
			/// Subtracts another vector from this one, component-wise.
			/// </summary>
			/// <param name="other">The vector to subtract.</param>
			/// <returns>A reference to this vector after subtraction.</returns>
			constexpr Vec4& operator-=(const Vec4& other)
			{
				x -= other.x;
				y -= other.y;
				z -= other.z;
				w -= other.w;
				return *this;
			}

			/// <summary>
			/// Multiplies each component of this vector by a scalar.
			/// </summary>
			/// <param name="scalar">The scalar value to multiply with.</param>
			/// <returns>A new scaled vector.</returns>
			constexpr Vec4 operator*(float scalar) const
			{
				return { x * scalar, y * scalar, z * scalar, w * scalar };
			}

			/// <summary>
			/// Multiplies each component of this vector by a scalar in-place.
			/// </summary>
			/// <param name="scalar">The scalar value to multiply with.</param>
			/// <returns>A reference to this vector after scaling.</returns>
			constexpr Vec4& operator*=(float scalar)
			{
				x *= scalar;
				y *= scalar;
				z *= scalar;
				w *= scalar;
				return *this;
			}

			/// <summary>
			/// Performs component-wise multiplication with another vector.
			/// </summary>
			/// <param name="other">The vector to multiply with.</param>
			/// <returns>A new vector representing the product.</returns>
			constexpr Vec4 operator*(const Vec4& other) const
			{
				return { x * other.x, y * other.y, z * other.z, w * other.w };
			}

			/// <summary>
			/// Performs component-wise multiplication with another vector in-place.
			/// </summary>
			/// <param name="other">The vector to multiply with.</param>
			/// <returns>A reference to this vector after multiplication.</returns>
			constexpr Vec4& operator*=(const Vec4& other)
			{
				x *= other.x;
				y *= other.y;
				z *= other.z;
				w *= other.w;
				return *this;
			}

			/// <summary>
			/// Divides each component of this vector by a scalar.
			/// </summary>
			/// <param name="scalar">The scalar divisor.</param>
			/// <returns>A new vector representing the quotient.</returns>
			constexpr Vec4 operator/(float scalar) const
			{
				assert(scalar != 0.0f && "Vec4 division by zero");
				return { x / scalar, y / scalar, z / scalar, w / scalar };
			}

			/// <summary>
			/// Divides each component of this vector by a scalar in-place.
			/// </summary>
			/// <param name="scalar">The scalar divisor.</param>
			/// <returns>A reference to this vector after division.</returns>
			constexpr Vec4& operator/=(float scalar)
			{
				assert(scalar != 0.0f && "Vec4 division by zero");
				x /= scalar;
				y /= scalar;
				z /= scalar;
				w /= scalar;
				return *this;
			}

			/// <summary>
			/// Performs component-wise division with another vector.
			/// </summary>
			/// <param name="other">The vector to divide by.</param>
			/// <returns>A new vector representing the result.</returns>
			constexpr Vec4 operator/(const Vec4& other) const
			{
				assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && other.w != 0.0f && "Vec4 component-wise division by zero");
				return { x / other.x, y / other.y, z / other.z, w / other.w };
			}

			/// <summary>
			/// Performs component-wise division with another vector in-place.
			/// </summary>
			/// <param name="other">The vector to divide by.</param>
			/// <returns>A reference to this vector after division.</returns>
			constexpr Vec4& operator/=(const Vec4& other)
			{
				assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && other.w != 0.0f && "Vec4 component-wise division by zero");
				x /= other.x;
				y /= other.y;
				z /= other.z;
				w /= other.w;
				return *this;
			}

			/// <summary>
			/// Returns the negation of this vector (flips the sign of all components).
			/// </summary>
			/// <returns>A new vector pointing in the opposite direction.</returns>
			constexpr Vec4 operator-() const
			{
				return {-x, -y, -z, -w};
			}

			/// <summary>
			/// Compares two vectors for equality, component-wise.
			/// </summary>
			/// <param name="other">The vector to compare with.</param>
			/// <returns>True if all components are equal, false otherwise.</returns>
			constexpr bool operator==(const Vec4& other) const
			{
				return x == other.x && y == other.y && z == other.z && w == other.w;
			}

			/// <summary>
			/// Compares two vectors for inequality, component-wise.
			/// </summary>
			/// <param name="other">The vector to compare with.</param>
			/// <returns>True if any component differs, false otherwise.</returns>
			constexpr bool operator!=(const Vec4& other) const
			{
				return !(*this == other);
			}
		};

		constexpr Vec3::Vec3(const Vec4& other)
			: x(other.x), y(other.y), z(other.z) { }

		struct UInt2
		{
			unsigned int u, v;
		};

		constexpr Vec4 ToVec4(const Vec3& v)
		{
			return { v.x, v.y, v.z, 0.0f };
		}
		constexpr Vec3 ToVec3(const Vec4& v)
		{
			return { v.x, v.y, v.z };
		}

		/// <summary>
		/// Returns the cross product of two 3D vectors.
		/// </summary>
		/// <param name="u">The first vector.</param>
		/// <param name="v">The second vector.</param>
		/// <returns>A new vector orthogonal to both input vectors.</returns>
		constexpr Vec3 Cross(const Vec3& u, const Vec3& v)
		{
			Vec3 result;
			result.x = u.y * v.z - u.z * v.y;
			result.y = u.z * v.x - u.x * v.z;
			result.z = u.x * v.y - u.y * v.x;
			return result;
		}

		/// <summary>
		/// Computes the dot product of two 4D vectors.
		/// </summary>
		/// <param name="u">The first vector.</param>
		/// <param name="v">The second vector.</param>
		/// <returns>The scalar dot product.</returns>
		constexpr float Dot(const Vec4& u, const Vec4& v)
		{
			return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
		}

		constexpr float Dot(const Vec3& u, const Vec3& v)
		{
			return u.x * v.x + u.y * v.y + u.z * v.z;
		}

		/// <summary>
		/// Computes the Euclidean length (magnitude) of a 4D vector.
		/// </summary>
		/// <param name="v">The vector to evaluate.</param>
		/// <returns>The length of the vector.</returns>
		inline float Length(const Vec4& v)
		{
			return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
		}
		
		inline float Length(const Vec3& v)
		{
			return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		}

		/// <summary>
		/// Normalizes a 4D vector to unit length.
		/// </summary>
		/// <param name="v">The vector to normalize.</param>
		/// <returns>A new vector with the same direction and length 1.</returns>
		/// <remarks>Behavior is undefined if the input vector has zero length.</remarks>
		inline Vec4 Normalize(const Vec4& v)
		{
			float len = Length(v);
			return { v.x / len, v.y / len, v.z / len, v.w / len };
		}

		inline Vec3 Normalize(const Vec3& v)
		{
			float len = Length(v);
			return { v.x / len, v.y / len, v.z / len };
		}
	}

}