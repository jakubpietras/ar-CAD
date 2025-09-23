#pragma once
#include <cmath>
#include <cassert>
#include <initializer_list>
#include <stdexcept>

namespace ar
{
	namespace mat
	{
		// forward declarations
		template <typename T> struct Vec4T;

		struct UInt2
		{
			UInt2(std::initializer_list<unsigned int> l)
			{
				u = *(l.begin());
				v = *(l.begin() + 1);
			}
			unsigned int u, v;
		};

		template <typename T>
		struct Vec2T
		{
			T x, y;
			inline const T* Data() const { return &x; };
			inline T* Data() { return &x; };
			constexpr Vec2T(T x = 0.0f, T y = 0.0f) : x(x), y(y) {}

			/// <summary>
			/// Component-wise sum of two vectors.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns></returns>
			constexpr Vec2T operator+(const Vec2T& other) const
			{
				return { x + other.x, y + other.y };
			}

			/// <summary>
			/// Component-wise sum of two vectors.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns></returns>
			constexpr Vec2T& operator+=(const Vec2T& other)
			{
				x += other.x;
				y += other.y;
				return *this;
			}

			/// <summary>
			/// Component-wise difference of two vectors.
			/// </summary>
			/// <param name="other">The vector to subtract.</param>
			/// <returns></returns>
			constexpr Vec2T operator-(const Vec2T& other) const
			{
				return { x - other.x, y - other.y };
			}

			/// <summary>
			/// Component-wise difference of two vectors.
			/// </summary>
			/// <param name="other">The vector to subtract.</param>
			/// <returns></returns>
			constexpr Vec2T& operator-=(const Vec2T& other)
			{
				x -= other.x;
				y -= other.y;
				return *this;
			}

			/// <summary>
			/// Component-wise multiplication by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec2T operator*(const T scalar) const
			{
				return { x * scalar, y * scalar };
			}

			/// <summary>
			/// Multiplies a scalar with a vector (scalar on the left-hand side).
			/// </summary>
			/// <param name="scalar">The scalar multiplier.</param>
			/// <param name="v">The vector being multiplied.</param>
			/// <returns>A scaled vector.</returns>
			constexpr friend Vec2T operator*(T scalar, const Vec2T& v);

			/// <summary>
			/// Component-wise multiplication by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec2T& operator*=(const T scalar)
			{
				x *= scalar;
				y *= scalar;
				return *this;
			}

			/// <summary>
			/// Component-wise multiplication by a vector.
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			constexpr Vec2T operator*(const Vec2T& other) const
			{
				return { x * other.x, y * other.y };
			}

			/// <summary>
			/// Component-wise multiplication by a vector.
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			constexpr Vec2T& operator*=(const Vec2T& other)
			{
				x *= other.x;
				y *= other.y;
				return *this;
			}

			/// <summary>
			/// Component-wise division by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec2T operator/(const T scalar) const
			{
				assert(scalar != 0.0f && "Vec2 division by zero");
				return { x / scalar, y / scalar };
			}

			/// <summary>
			/// Component-wise division by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec2T& operator/=(const T scalar)
			{
				assert(scalar != 0.0f && "Vec2 division by zero");
				x /= scalar;
				y /= scalar;
				return *this;
			}

			/// <summary>
			/// Component-wise division by a vector.
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			constexpr Vec2T operator/(const Vec2T& other) const
			{
				assert(other.x != 0.0f && other.y != 0.0f && "Vec2 component-wise division by zero");
				return { x / other.x, y / other.y};
			}

			/// <summary>
			/// Component-wise division by a vector.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec2T& operator/=(const Vec2T& other)
			{
				assert(other.x != 0.0f && other.y != 0.0f && "Vec2 component-wise division by zero");
				x /= other.x;
				y /= other.y;
				return *this;
			}

			/// <summary>
			/// Component-wise inequality comparison operator.
			/// </summary>
			/// <param name="other">The vector to compare.</param>
			/// <returns>True if vectors differ on any component, false otherwise.</returns>
			constexpr bool operator!=(const Vec2T& other) const
			{
				return x != other.x || y != other.y;
			}

			/// <summary>
			/// Component-wise equality comparison operator.
			/// </summary>
			/// <param name="other">The vector to compare.</param>
			/// <returns>True if vectors are identical, false otherwise.</returns>
			constexpr bool operator==(const Vec2T& other) const
			{
				return x == other.x && y == other.y;
			}

			/// <summary>
			/// Unary negation.
			/// </summary>
			/// <returns>Vector with negated components.</returns>
			constexpr Vec2T operator-() const
			{
				return { -x, -y };
			}
		};
		using Vec2f = Vec2T<float>;
		using Vec2d = Vec2T<double>;
		using Vec2 = Vec2f;

		template <typename T>
		struct Vec3T
		{
			T x, y, z;
			inline const T* Data() const { return &x; };
			inline T* Data() { return &x; };
			constexpr Vec3T(T x = 0.0f, T y = 0.0f, T z = 0.0f) : x(x), y(y), z(z) {}
			constexpr Vec3T(const Vec4T<T>& other) 
				: x(other.x), y(other.y), z(other.z) {}

			/// <summary>
			/// Component-wise sum of two vectors.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns></returns>
			constexpr Vec3T operator+(const Vec3T& other) const
			{
				return {x + other.x, y + other.y, z + other.z};
			}

			/// <summary>
			/// Component-wise sum of two vectors.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns></returns>
			constexpr Vec3T& operator+=(const Vec3T& other)
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
			constexpr Vec3T operator-(const Vec3T& other) const
			{
				return { x - other.x, y - other.y, z - other.z };
			}

			/// <summary>
			/// Component-wise difference of two vectors.
			/// </summary>
			/// <param name="other">The vector to subtract.</param>
			/// <returns></returns>
			constexpr Vec3T& operator-=(const Vec3T& other)
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
			constexpr Vec3T operator*(const T scalar) const
			{
				return { x * scalar, y * scalar, z * scalar };
			}

			/// <summary>
			/// Multiplies a scalar with a vector (scalar on the left-hand side).
			/// </summary>
			/// <param name="scalar">The scalar multiplier.</param>
			/// <param name="v">The vector being multiplied.</param>
			/// <returns>A scaled vector.</returns>
			constexpr friend Vec3T operator*(T scalar, const Vec3T& v)
			{
				return { v.x * scalar, v.y * scalar, v.z * scalar };
			}

			/// <summary>
			/// Component-wise multiplication by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec3T& operator*=(const T scalar)
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
			constexpr Vec3T operator*(const Vec3T& other) const
			{
				return { x * other.x, y * other.y, z * other.z };
			}

			/// <summary>
			/// Component-wise multiplication by a vector.
			/// </summary>
			/// <param name="other"></param>
			/// <returns></returns>
			constexpr Vec3T& operator*=(const Vec3T& other)
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
			constexpr Vec3T operator/(const T scalar) const
			{
				assert(scalar != 0.0f && "Vec3 division by zero");
				return { x / scalar, y / scalar, z / scalar };
			}

			/// <summary>
			/// Component-wise division by a scalar.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec3T& operator/=(const T scalar)
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
			constexpr Vec3T operator/(const Vec3T& other) const
			{
				assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && "Vec3 component-wise division by zero");
				return { x / other.x, y / other.y, z / other.z };
			}

			/// <summary>
			/// Component-wise division by a vector.
			/// </summary>
			/// <param name="scalar"></param>
			/// <returns></returns>
			constexpr Vec3T& operator/=(const Vec3T& other)
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
			constexpr bool operator!=(const Vec3T& other) const
			{
				return x != other.x || y != other.y || z != other.z;
			}

			/// <summary>
			/// Component-wise equality comparison operator.
			/// </summary>
			/// <param name="other">The vector to compare.</param>
			/// <returns>True if vectors are identical, false otherwise.</returns>
			constexpr bool operator==(const Vec3T& other) const
			{
				return x == other.x && y == other.y && z == other.z;
			}

			/// <summary>
			/// Unary negation.
			/// </summary>
			/// <returns>Vector with negated components.</returns>
			constexpr Vec3T operator-() const
			{
				return {-x, -y, -z};
			}
		};
		using Vec3f = Vec3T<float>;
		using Vec3d = Vec3T<double>;
		using Vec3 = Vec3f;

		template <typename T>
		struct Vec4T
		{
			T x, y, z, w;
			const T* Data() const;
			T* Data();
			constexpr Vec4T(T x = 0.0f, T y = 0.0f, T z = 0.0f, T w = 0.0f)
				: x(x), y(y), z(z), w(w) { }
			constexpr Vec4T(const Vec3T<T>& v3, T w)
				: x(v3.x), y(v3.y), z(v3.z), w(w) {}

			/// <summary>
			/// Returns the component-wise sum of this vector and another.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns>A new vector representing the sum.</returns>
			constexpr Vec4T operator+(const Vec4T& other) const
			{
				return { x + other.x, y + other.y, z + other.z, w + other.w };
			}

			/// <summary>
			/// Adds another vector to this one, component-wise.
			/// </summary>
			/// <param name="other">The vector to add.</param>
			/// <returns>A reference to this vector after addition.</returns>
			constexpr Vec4T& operator+=(const Vec4T& other)
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
			constexpr Vec4T operator-(const Vec4T& other) const
			{
				return { x - other.x, y - other.y, z - other.z, w - other.w };
			}

			/// <summary>
			/// Subtracts another vector from this one, component-wise.
			/// </summary>
			/// <param name="other">The vector to subtract.</param>
			/// <returns>A reference to this vector after subtraction.</returns>
			constexpr Vec4T& operator-=(const Vec4T& other)
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
			constexpr Vec4T operator*(T scalar) const
			{
				return { x * scalar, y * scalar, z * scalar, w * scalar };
			}

			/// <summary>
			/// Multiplies each component of this vector by a scalar in-place.
			/// </summary>
			/// <param name="scalar">The scalar value to multiply with.</param>
			/// <returns>A reference to this vector after scaling.</returns>
			constexpr Vec4T& operator*=(T scalar)
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
			constexpr Vec4T operator*(const Vec4T& other) const
			{
				return { x * other.x, y * other.y, z * other.z, w * other.w };
			}

			/// <summary>
			/// Performs component-wise multiplication with another vector in-place.
			/// </summary>
			/// <param name="other">The vector to multiply with.</param>
			/// <returns>A reference to this vector after multiplication.</returns>
			constexpr Vec4T& operator*=(const Vec4T& other)
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
			constexpr Vec4T operator/(T scalar) const
			{
				assert(scalar != 0.0f && "Vec4 division by zero");
				return { x / scalar, y / scalar, z / scalar, w / scalar };
			}

			/// <summary>
			/// Divides each component of this vector by a scalar in-place.
			/// </summary>
			/// <param name="scalar">The scalar divisor.</param>
			/// <returns>A reference to this vector after division.</returns>
			constexpr Vec4T& operator/=(T scalar)
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
			constexpr Vec4T operator/(const Vec4T& other) const
			{
				assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && other.w != 0.0f && "Vec4 component-wise division by zero");
				return { x / other.x, y / other.y, z / other.z, w / other.w };
			}

			/// <summary>
			/// Performs component-wise division with another vector in-place.
			/// </summary>
			/// <param name="other">The vector to divide by.</param>
			/// <returns>A reference to this vector after division.</returns>
			constexpr Vec4T& operator/=(const Vec4T& other)
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
			constexpr Vec4T operator-() const
			{
				return {-x, -y, -z, -w};
			}

			/// <summary>
			/// Compares two vectors for equality, component-wise.
			/// </summary>
			/// <param name="other">The vector to compare with.</param>
			/// <returns>True if all components are equal, false otherwise.</returns>
			constexpr bool operator==(const Vec4T& other) const
			{
				return x == other.x && y == other.y && z == other.z && w == other.w;
			}

			/// <summary>
			/// Compares two vectors for inequality, component-wise.
			/// </summary>
			/// <param name="other">The vector to compare with.</param>
			/// <returns>True if any component differs, false otherwise.</returns>
			constexpr bool operator!=(const Vec4T& other) const
			{
				return !(*this == other);
			}

			constexpr T operator[](const size_t index) const {
				switch (index)
				{
				case 0:
					return x;
				case 1:
					return y;
				case 2:
					return z;
				case 3: 
					return w;
				default:
					throw std::runtime_error("Incorrect indexing");
				}
			}

			T& operator[](const size_t index) {
				switch (index)
				{
				case 0:
					return x;
				case 1:
					return y;
				case 2:
					return z;
				case 3:
					return w;
				default:
					throw std::runtime_error("Incorrect indexing");
				}
			}

		};
		using Vec4f = Vec4T<float>;
		using Vec4d = Vec4T<double>;
		using Vec4 = Vec4f;

		template <typename T>
		constexpr Vec4T<T> ToVec4(const Vec3T<T>& v)
		{
			return { v.x, v.y, v.z, T(0)};
		}
		template <typename T>
		constexpr Vec3T<T> ToVec3(const Vec4T<T>& v)
		{
			return { v.x, v.y, v.z };
		}

		/// <summary>
		/// Returns the cross product of two 3D vectors.
		/// </summary>
		/// <param name="u">The first vector.</param>
		/// <param name="v">The second vector.</param>
		/// <returns>A new vector orthogonal to both input vectors.</returns>
		template <typename T>
		constexpr Vec3T<T> Cross(const Vec3T<T>& u, const Vec3T<T>& v)
		{
			return { u.y * v.z - u.z * v.y,
			 u.z * v.x - u.x * v.z,
			 u.x * v.y - u.y * v.x };
		}

		/// <summary>
		/// Computes the dot product of two 4D vectors.
		/// </summary>
		/// <param name="u">The first vector.</param>
		/// <param name="v">The second vector.</param>
		/// <returns>The scalar dot product.</returns>
		
		template <typename T>
		constexpr T Dot(const Vec4T<T>& u, const Vec4T<T>& v)
		{
			return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
		}

		template <typename T>
		constexpr T Dot(const Vec3T<T>& u, const Vec3T<T>& v)
		{
			return u.x * v.x + u.y * v.y + u.z * v.z;
		}

		/// <summary>
		/// Computes the Euclidean length (magnitude) of a 4D vector.
		/// </summary>
		/// <param name="v">The vector to evaluate.</param>
		/// <returns>The length of the vector.</returns>
		
		template <typename T>
		inline T Length(const Vec4T<T>& v) 
		{
			return std::sqrt(Dot(v, v));
		}

		template <typename T>
		inline T Length(const Vec3T<T>& v) 
		{
			return std::sqrt(Dot(v, v));
		}

		template <typename T>
		inline T LengthSquared(const Vec4T<T>& v) 
		{
			return Dot(v, v);
		}

		template <typename T>
		inline T LengthSquared(const Vec3T<T>& v) 
		{
			return Dot(v, v);
		}

		/// <summary>
		/// Normalizes a 4D vector to unit length.
		/// </summary>
		/// <param name="v">The vector to normalize.</param>
		/// <returns>A new vector with the same direction and length 1.</returns>
		/// <remarks>Behavior is undefined if the input vector has zero length.</remarks>
		template <typename T>
		inline Vec4T<T> Normalize(const Vec4T<T>& v) 
		{
			T len = Length(v);
			return { v.x / len, v.y / len, v.z / len, v.w / len };
		}

		template <typename T>
		inline Vec3T<T> Normalize(const Vec3T<T>& v) 
		{
			T len = Length(v);
			return { v.x / len, v.y / len, v.z / len };
		}
	}



}