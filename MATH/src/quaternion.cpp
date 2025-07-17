#include "quaternion.h"

namespace ar
{
	Quat::Quat(float w, float x, float y, float z)
		: x(x), y(y), z(z), w(w)
	{ }
	Quat Quat::operator+(const Quat & r) const
	{
		return Quat(w + r.w, x + r.x, y + r.y, z + r.z);
	}
	Quat Quat::operator*(const Quat& r) const
	{
		return Quat(
			r.w * w - r.x * x - r.y * y - r.z * z,
			r.w * x + r.x * w - r.y * z + r.z * y,
			r.w * y + r.x * z + r.y * w - r.z * x,
			r.w * z - r.x * y + r.y * x + r.z * w
		);
	}
	Quat Quat::operator/(float scalar) const
	{
		return Quat(w / scalar, x / scalar, y / scalar, z / scalar);
	}
	float Norm(const Quat& q)
	{
		return std::sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
	}
	Quat Normalize(const Quat& q)
	{
		return Quat(q / Norm(q));
	}
	Quat Conjugate(const Quat& q)
	{
		return Quat(q.w, -q.x, -q.y, -q.z);
	}
	Quat Inverse(const Quat& q)
	{
		float n = Norm(q);
		return Conjugate(q) * Quat(1.0f / (n * n));
	}
	Quat FromAxisAngle(float angle_deg, const Vec3& axis_dir)
	{
		auto q = Quat();
		float angle = Radians(angle_deg) / 2.0f;
		q.w = std::cosf(angle);
		q.x = std::sinf(angle) * axis_dir.x;
		q.y = std::sinf(angle) * axis_dir.y;
		q.z = std::sinf(angle) * axis_dir.z;
		return q;
	}
	Quat RPYToQuat(const Vec3& angles)
	{
		float angle_x = Radians(angles.x),
			angle_y = Radians(angles.y),
			angle_z = Radians(angles.z);

		Quat qx, qy, qz;

		qx.w = std::cosf(angle_x / 2.0f);
		qx.x = std::sinf(angle_x / 2.0f);
		qx.y = 0.0f;
		qx.z = 0.0f;

		qy.w = std::cosf(angle_y / 2.0f);
		qy.x = 0.0f;
		qy.y = std::sinf(angle_y / 2.0f);
		qy.z = 0.0f;

		qz.w = std::cosf(angle_z / 2.0f);
		qz.x = 0.0f;
		qz.y = 0.0f;
		qz.z = std::sinf(angle_z / 2.0f);

		return qz * qy * qx;
	}
	Quat Vec3ToQuat(const Vec3& v)
	{
		return Quat(0.0f, v.x, v.y, v.z);
	}
	Vec3 QuatToVec3(const Quat& q)
	{
		return Vec3(q.x, q.y, q.z);
	}
	Mat4 ToMat4(const Quat& q)
	{
		float w = q.w, x = q.x, y = q.y, z = q.z;

		Mat4 mat = Identity();
		mat(0,0) = 1 - 2 * (y * y + z * z);
		mat(0,1) = 2 * (x * y - w * z);
		mat(0,2) = 2 * (x * z + w * y);

		mat(1,0) = 2 * (x * y + w * z);
		mat(1,1) = 1 - 2 * (x * x + z * z);
		mat(1,2) = 2 * (y * z - w * x);

		mat(2,0) = 2 * (x * z - w * y);
		mat(2,1) = 2 * (y * z + w * x);
		mat(2,2) = 1 - 2 * (x * x + y * y);

		return mat;
	}
	Vec3 Rotate(const Vec3& v, const Quat& q)
	{
		Quat vecQuat = Vec3ToQuat(v);
		Quat result = q * vecQuat * Inverse(q);
		return QuatToVec3(result);
	}
	Vec4 Rotate(const Vec4& v, const Quat& q)
	{
		Quat vecQuat = Vec3ToQuat(Vec3(v.x, v.y, v.z));
		Quat result = q * vecQuat * Inverse(q);
		auto v3 = QuatToVec3(result);
		return Vec4(v3.x, v3.y, v3.z, 0.0f);
	}
}