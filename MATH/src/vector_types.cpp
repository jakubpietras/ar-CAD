#include "vector_types.h"
#include <ostream>

namespace ar
{
#pragma region Vec3
	const float* Vec3::Data() const
	{
		return &x;
	}
	Vec3::Vec3(float x, float y, float z)
		: x(x), y(y), z(z)
	{ }
	Vec3 Vec3::operator+(const Vec3& other) const
	{
		return Vec3(x + other.x, y + other.y, z + other.z);
	}
	Vec3 Vec3::operator-(const Vec3& other) const
	{
		return Vec3(x - other.x, y - other.y, z - other.z);
	}
	Vec3& Vec3::operator-=(const Vec3& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	Vec3& Vec3::operator+=(const Vec3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	Vec3 Vec3::operator*(const float scalar) const
	{
		return Vec3(x * scalar, y * scalar, z * scalar);
	}
	Vec3& Vec3::operator*=(const float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}
	Vec3 Vec3::operator*(const Vec3& other) const
	{
		return Vec3(x * other.x, y * other.y, z * other.z);
	}
	Vec3& Vec3::operator*=(const Vec3& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}
	Vec3 Vec3::operator/(const float scalar) const
	{
		assert(scalar != 0.0f && "Vec3 division by zero");
		return Vec3(x / scalar, y / scalar, z / scalar);
	}
	Vec3& Vec3::operator/=(const float scalar)
	{
		assert(scalar != 0.0f && "Vec3 division by zero");
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}
	Vec3 Vec3::operator/(const Vec3& other) const
	{
		assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && "Vec3 component-wise division by zero");
		return Vec3(x / other.x, y / other.y, z / other.z);
	}
	Vec3& Vec3::operator/=(const Vec3& other)
	{
		assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && "Vec3 component-wise division by zero");
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}
	bool Vec3::operator!=(const Vec3& other) const
	{
		return x != other.x || y != other.y || z != other.z;
	}
	bool Vec3::operator==(const Vec3& other) const
	{
		return x == other.x && y == other.y && z == other.z;
	}
	Vec3 Vec3::operator-() const
	{
		return Vec3(-x, -y, -z);
	}
	float Dot(const Vec3& u, const Vec3& v)
	{
		return u.x * v.x + u.y * v.y + u.z * v.z;
	}
	float Length(const Vec3& v)
	{
		return std::sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	Vec3 Normalize(const Vec3& v)
	{
		float len = Length(v);
		return Vec3(v.x / len, v.y / len, v.z / len);
	}
	Vec3 operator*(float scalar, const Vec3& v)
	{
		return Vec3(v.x * scalar, v.y * scalar, v.z * scalar);
	}
	Vec4 ToVec4(const Vec3& v)
	{
		return Vec4(v.x, v.y, v.z, 0.0f);
	}
	Vec3 ToVec3(const Vec4& v)
	{
		return Vec3(v.x, v.y, v.z);
	}
	Vec3 Cross(const Vec3& u, const Vec3& v)
	{
		Vec3 result;
		result.x = u.y * v.z - u.z * v.y;
		result.y = u.z * v.x - u.x * v.z;
		result.z = u.x * v.y - u.y * v.x;
		return result;
	}
#pragma endregion Vec3

#pragma region Vec4

	const Vec4 Vec4::s_WorldForward = { 0.0f, 0.0f, 1.0f, 0.0f },
		Vec4::s_WorldUp = { 0.0f, 1.0f, 0.0f, 0.0f },
		Vec4::s_WorldRight = { 1.0f, 0.0f, 0.0f, 0.0f };

	const float* Vec4::Data() const
	{
		return &x;
	}
	Vec4::Vec4(float x, float y, float z, float w)
		: x(x), y(y), z(z), w(w)
	{ }
	Vec4::Vec4(const Vec3 & v3, float w)
		: x(v3.x), y(v3.y), z(v3.z), w(w)
	{ }
	Vec4 Vec4::operator+(const Vec4& other) const
	{
		return Vec4(x + other.x, y + other.y, z + other.z, w + other.w);
	}
	Vec4 Vec4::operator-(const Vec4& other) const
	{
		return Vec4(x - other.x, y - other.y, z - other.z, w - other.w);
	}
	Vec4& Vec4::operator-=(const Vec4& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
		return *this;
	}
	Vec4& Vec4::operator+=(const Vec4& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
	}
	Vec4 Vec4::operator*(const float scalar) const
	{
		return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
	}
	Vec4& Vec4::operator*=(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		w *= scalar;
		return *this;
	}
	Vec4 Vec4::operator*(const Vec4& other) const
	{
		return Vec4(x * other.x, y * other.y, z * other.z, w * other.w);
	}
	Vec4& Vec4::operator*=(const Vec4& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
		return *this;
	}
	Vec4 Vec4::operator/(const float scalar) const
	{
		assert(scalar != 0.0f && "Vec4 division by zero");
		return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
	}
	Vec4& Vec4::operator/=(float scalar)
	{
		assert(scalar != 0.0f && "Vec4 division by zero");
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
		return *this;
	}
	Vec4 Vec4::operator/(const Vec4& other) const
	{
		assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && other.w != 0.0f && "Vec4 component-wise division by zero");
		return Vec4(x / other.x, y / other.y, z / other.z, w / other.w);
	}
	Vec4& Vec4::operator/=(const Vec4& other)
	{
		assert(other.x != 0.0f && other.y != 0.0f && other.z != 0.0f && other.w != 0.0f && "Vec4 component-wise division by zero");
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
	}
	Vec4 Vec4::operator-() const 
	{
		return Vec4(-x, -y, -z, -w);
	}
	bool Vec4::operator==(const Vec4& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}
	bool Vec4::operator!=(const Vec4& other) const
	{
		return !(*this == other);
	}
	float Dot(const Vec4& u, const Vec4& v)
	{
		return u.x * v.x + u.y * v.y + u.z * v.z + u.w * v.w;
	}
	float Length(const Vec4& v)
	{
		return std::sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}
	Vec4 Normalize(const Vec4& v)
	{
		float len = Length(v);
		return Vec4(v.x / len, v.y / len, v.z / len, v.w / len);
	}
#pragma endregion Vec4
}
