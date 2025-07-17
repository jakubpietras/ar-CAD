#pragma once
#include <cmath>
#include "vector_types.h"
#include "trigonometry.h"
#include "matrix_types.h"
#include <ostream>

namespace ar
{
	struct Quat
	{
		Quat(float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f);
		Quat(const Vec3& vec);
		Quat operator+(const Quat& r) const;
		Quat& operator+=(const Quat& other);

		Quat operator-(const Quat& other) const;
		Quat& operator-=(const Quat& other);

		Quat operator*(const Quat& r) const;
		Quat operator*(const float scalar) const;
		Quat& operator*=(const Quat& other);

		Quat operator/(float scalar) const;
		Quat& operator/=(const Quat& other);

		bool operator==(const Quat& q) const;
		bool operator!=(const Quat& q) const;

		Quat operator-() const;

		float w, x, y, z;


	};
	float Norm(const Quat& q);
	Quat Normalize(const Quat& q);
	Quat Conjugate(const Quat& q);
	Quat Inverse(const Quat& q);

	// construction helpers
	Quat FromAxisAngle(float angle, const Vec3& axis_dir);	// axis angle
	Quat RPYToQuat(const Vec3& angles_deg);
	Vec3 ToEulerXYZ(const Quat& q);
	Mat4 ToMat4(const Quat& q); // ToMat4

	// quaternion rotation
	Vec3 Rotate(const Vec3& v, const Quat& q); // Rotates
	Vec4 Rotate(const Vec4& v, const Quat& q); // Rotates
	Quat Vec3ToQuat(const Vec3& v);	// c-tor
	Vec3 QuatToVec3(const Quat& q);	// ToEulerXYZ
}