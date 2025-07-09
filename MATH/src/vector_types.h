#pragma once
#include <cmath>
#include <cassert>

namespace ar
{
	struct Vec3
	{
		float x, y, z;
		const float* Data() const;
		Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f);

		/// <summary>
		/// Component-wise sum of two vectors.
		/// </summary>
		/// <param name="other">The vector to add.</param>
		/// <returns></returns>
		Vec3 operator+(const Vec3& other) const;

		/// <summary>
		/// Component-wise sum of two vectors.
		/// </summary>
		/// <param name="other">The vector to add.</param>
		/// <returns></returns>
		Vec3& operator+=(const Vec3& other);
		
		/// <summary>
		/// Component-wise difference of two vectors.
		/// </summary>
		/// <param name="other">The vector to subtract.</param>
		/// <returns></returns>
		Vec3 operator-(const Vec3& other) const;

		/// <summary>
		/// Component-wise difference of two vectors.
		/// </summary>
		/// <param name="other">The vector to subtract.</param>
		/// <returns></returns>
		Vec3& operator-=(const Vec3& other);

		/// <summary>
		/// Component-wise multiplication by a scalar.
		/// </summary>
		/// <param name="scalar"></param>
		/// <returns></returns>
		Vec3 operator*(const float scalar) const;

		/// <summary>
		/// Multiplies a scalar with a vector (scalar on the left-hand side).
		/// </summary>
		/// <param name="scalar">The scalar multiplier.</param>
		/// <param name="v">The vector being multiplied.</param>
		/// <returns>A scaled vector.</returns>
		friend Vec3 operator*(float scalar, const Vec3& v);

		/// <summary>
		/// Component-wise multiplication by a scalar.
		/// </summary>
		/// <param name="scalar"></param>
		/// <returns></returns>
		Vec3& operator*=(const float scalar);

		/// <summary>
		/// Component-wise multiplication by a vector.
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vec3 operator*(const Vec3& other) const;

		/// <summary>
		/// Component-wise multiplication by a vector.
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vec3& operator*=(const Vec3& other);

		/// <summary>
		/// Component-wise division by a scalar.
		/// </summary>
		/// <param name="scalar"></param>
		/// <returns></returns>
		Vec3 operator/(const float scalar) const;

		/// <summary>
		/// Component-wise division by a scalar.
		/// </summary>
		/// <param name="scalar"></param>
		/// <returns></returns>
		Vec3& operator/=(const float scalar);

		/// <summary>
		/// Component-wise division by a vector.
		/// </summary>
		/// <param name="other"></param>
		/// <returns></returns>
		Vec3 operator/(const Vec3& other) const;

		/// <summary>
		/// Component-wise division by a vector.
		/// </summary>
		/// <param name="scalar"></param>
		/// <returns></returns>
		Vec3& operator/=(const Vec3& other);

		/// <summary>
		/// Component-wise inequality comparison operator.
		/// </summary>
		/// <param name="other">The vector to compare.</param>
		/// <returns>True if vectors differ on any component, false otherwise.</returns>
		bool operator!=(const Vec3& other) const;

		/// <summary>
		/// Component-wise equality comparison operator.
		/// </summary>
		/// <param name="other">The vector to compare.</param>
		/// <returns>True if vectors are identical, false otherwise.</returns>
		bool operator==(const Vec3& other) const;

		/// <summary>
		/// Unary negation.
		/// </summary>
		/// <returns>Vector with negated components.</returns>
		Vec3 operator-() const;
	};


	struct Vec4
	{
		float x, y, z, w;
		const float* Data() const;
		Vec4(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f);
		Vec4(const Vec3& v3, float w);

		/// <summary>
		/// Returns the component-wise sum of this vector and another.
		/// </summary>
		/// <param name="other">The vector to add.</param>
		/// <returns>A new vector representing the sum.</returns>
		Vec4 operator+(const Vec4& other) const;

		/// <summary>
		/// Adds another vector to this one, component-wise.
		/// </summary>
		/// <param name="other">The vector to add.</param>
		/// <returns>A reference to this vector after addition.</returns>
		Vec4& operator+=(const Vec4& other);

		/// <summary>
		/// Returns the component-wise difference between this vector and another.
		/// </summary>
		/// <param name="other">The vector to subtract.</param>
		/// <returns>A new vector representing the difference.</returns>
		Vec4 operator-(const Vec4& other) const;

		/// <summary>
		/// Subtracts another vector from this one, component-wise.
		/// </summary>
		/// <param name="other">The vector to subtract.</param>
		/// <returns>A reference to this vector after subtraction.</returns>
		Vec4& operator-=(const Vec4& other);

		/// <summary>
		/// Multiplies each component of this vector by a scalar.
		/// </summary>
		/// <param name="scalar">The scalar value to multiply with.</param>
		/// <returns>A new scaled vector.</returns>
		Vec4 operator*(float scalar) const;

		/// <summary>
		/// Multiplies each component of this vector by a scalar in-place.
		/// </summary>
		/// <param name="scalar">The scalar value to multiply with.</param>
		/// <returns>A reference to this vector after scaling.</returns>
		Vec4& operator*=(float scalar);

		/// <summary>
		/// Performs component-wise multiplication with another vector.
		/// </summary>
		/// <param name="other">The vector to multiply with.</param>
		/// <returns>A new vector representing the product.</returns>
		Vec4 operator*(const Vec4& other) const;

		/// <summary>
		/// Performs component-wise multiplication with another vector in-place.
		/// </summary>
		/// <param name="other">The vector to multiply with.</param>
		/// <returns>A reference to this vector after multiplication.</returns>
		Vec4& operator*=(const Vec4& other);

		/// <summary>
		/// Divides each component of this vector by a scalar.
		/// </summary>
		/// <param name="scalar">The scalar divisor.</param>
		/// <returns>A new vector representing the quotient.</returns>
		Vec4 operator/(float scalar) const;

		/// <summary>
		/// Divides each component of this vector by a scalar in-place.
		/// </summary>
		/// <param name="scalar">The scalar divisor.</param>
		/// <returns>A reference to this vector after division.</returns>
		Vec4& operator/=(float scalar);

		/// <summary>
		/// Performs component-wise division with another vector.
		/// </summary>
		/// <param name="other">The vector to divide by.</param>
		/// <returns>A new vector representing the result.</returns>
		Vec4 operator/(const Vec4& other) const;

		/// <summary>
		/// Performs component-wise division with another vector in-place.
		/// </summary>
		/// <param name="other">The vector to divide by.</param>
		/// <returns>A reference to this vector after division.</returns>
		Vec4& operator/=(const Vec4& other);
		
		/// <summary>
		/// Returns the negation of this vector (flips the sign of all components).
		/// </summary>
		/// <returns>A new vector pointing in the opposite direction.</returns>
		Vec4 operator-() const;

		/// <summary>
		/// Compares two vectors for equality, component-wise.
		/// </summary>
		/// <param name="other">The vector to compare with.</param>
		/// <returns>True if all components are equal, false otherwise.</returns>
		bool operator==(const Vec4& other) const;

		/// <summary>
		/// Compares two vectors for inequality, component-wise.
		/// </summary>
		/// <param name="other">The vector to compare with.</param>
		/// <returns>True if any component differs, false otherwise.</returns>
		bool operator!=(const Vec4& other) const;
	};


	struct UInt2
	{
		unsigned int u, v;
	};

	Vec4 ToVec4(const Vec3& v);
	Vec3 ToVec3(const Vec4& v);

	/// <summary>
	/// Returns the cross product of two 3D vectors.
	/// </summary>
	/// <param name="u">The first vector.</param>
	/// <param name="v">The second vector.</param>
	/// <returns>A new vector orthogonal to both input vectors.</returns>
	Vec3 Cross(const Vec3& u, const Vec3& v);
	
	/// <summary>
	/// Computes the dot product of two 4D vectors.
	/// </summary>
	/// <param name="u">The first vector.</param>
	/// <param name="v">The second vector.</param>
	/// <returns>The scalar dot product.</returns>
	float Dot(const Vec4& u, const Vec4& v);

	/// <summary>
	/// Computes the dot product of two 3D vectors.
	/// </summary>
	/// <param name="u">The first vector.</param>
	/// <param name="v">The second vector.</param>
	/// <returns>The scalar dot product.</returns>
	float Dot(const Vec3& u, const Vec3& v);

	/// <summary>
	/// Computes the Euclidean length (magnitude) of a 3D vector.
	/// </summary>
	/// <param name="v">The vector to evaluate.</param>
	/// <returns>The length of the vector.</returns>
	float Length(const Vec3& v);

	/// <summary>
	/// Computes the Euclidean length (magnitude) of a 4D vector.
	/// </summary>
	/// <param name="v">The vector to evaluate.</param>
	/// <returns>The length of the vector.</returns>
	float Length(const Vec4& v);

	/// <summary>
	/// Normalizes a 4D vector to unit length.
	/// </summary>
	/// <param name="v">The vector to normalize.</param>
	/// <returns>A new vector with the same direction and length 1.</returns>
	/// <remarks>Behavior is undefined if the input vector has zero length.</remarks>
	Vec4 Normalize(const Vec4& v);

	/// <summary>
	/// Normalizes a 3D vector to unit length.
	/// </summary>
	/// <param name="v">The vector to normalize.</param>
	/// <returns>A new vector with the same direction and length 1.</returns>
	/// <remarks>Behavior is undefined if the input vector has zero length.</remarks>
	Vec3 Normalize(const Vec3& v);
}