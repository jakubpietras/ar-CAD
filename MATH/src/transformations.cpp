#include "transformations.h"

namespace ar
{
	namespace mat
	{
		Mat4 ScaleMatrix(const Vec3& scaling_factors)
		{
			auto sm = Identity();
			sm(0, 0) = scaling_factors.x;
			sm(1, 1) = scaling_factors.y;
			sm(2, 2) = scaling_factors.z;
			sm(3, 3) = 1.0f;
			return sm;
		}
		Mat4 ScaleMatrix(float scale_x, float scale_y, float scale_z)
		{
			return ScaleMatrix(Vec3(scale_x, scale_y, scale_z));
		}
		Mat4 RotationMatrix(const Vec3& rotation_angles)
		{
			auto angles = Vec3(Radians(rotation_angles.x),
				Radians(rotation_angles.y),
				Radians(rotation_angles.z));

			Mat4 x_rotation = Diagonal(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			x_rotation(1, 1) = std::cosf(angles.x);
			x_rotation(2, 2) = std::cosf(angles.x);
			x_rotation(1, 2) = -std::sinf(angles.x);
			x_rotation(2, 1) = std::sinf(angles.x);

			Mat4 y_rotation = Diagonal(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			y_rotation(0, 0) = std::cosf(angles.y);
			y_rotation(2, 2) = std::cosf(angles.y);
			y_rotation(0, 2) = std::sinf(angles.y);
			y_rotation(2, 0) = -std::sinf(angles.y);

			Mat4 z_rotation = Diagonal(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			z_rotation(0, 0) = std::cosf(angles.z);
			z_rotation(1, 1) = std::cosf(angles.z);
			z_rotation(0, 1) = -std::sinf(angles.z);
			z_rotation(1, 0) = std::sinf(angles.z);

			return x_rotation * y_rotation * z_rotation;
		}
		Mat4 RotationMatrix(float pitch, float yaw, float roll)
		{
			return RotationMatrix(Vec3(pitch, yaw, roll));
		}
		Mat4 TranslationMatrix(const Vec3& translation_vector)
		{
			auto tm = Diagonal(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
			tm(0, 3) = translation_vector.x;
			tm(1, 3) = translation_vector.y;
			tm(2, 3) = translation_vector.z;
			return tm;
		}
		Mat4 TranslationMatrix(float dx, float dy, float dz)
		{
			return TranslationMatrix(Vec3(dx, dy, dz));
		}
		Mat4 Ortho(float left, float right, float top, float bottom, float near, float far)
		{
			return Mat4();
		}
		Mat4 Perspective(float fov, float aspect_ratio, float near, float far)
		{
			auto pm = Mat4();
			pm(0, 0) = 1.0f / (aspect_ratio * (std::tanf(Radians(fov / 2))));
			pm(1, 1) = 1.0f / (std::tanf(Radians(fov / 2)));
			pm(2, 2) = (far + near) / (far - near);
			pm(2, 3) = (-2.0f * far * near) / (far - near);
			pm(3, 2) = 1.0f;
			pm(3, 3) = 0.0f;
			return pm;
		}
		Mat4 InvPerspective(float fov, float aspect_ratio, float nearZ, float farZ)
		{
			Mat4 invPm = Identity();

			float tanHalfFov = std::tanf(Radians(fov * 0.5f));

			invPm(0, 0) = aspect_ratio * tanHalfFov;
			invPm(1, 1) = tanHalfFov;
			invPm(2, 2) = 0.0f;
			invPm(2, 3) = -1.0f;

			float C = -(farZ + nearZ) / (farZ - nearZ);
			float D = -(2.0f * farZ * nearZ) / (farZ - nearZ);

			invPm(3, 2) = 1.0f / D;
			invPm(3, 3) = C / D;

			return invPm;
		}

		Mat4 LookAt(Vec4 right, Vec4 up, Vec4 forward, Vec4 position)
		{
			Mat4 view = Identity();

			view(0, 0) = right.x;
			view(0, 1) = right.y;
			view(0, 2) = right.z;
			view(0, 3) = -Dot(Vec3(right), Vec3(position));

			view(1, 0) = up.x;
			view(1, 1) = up.y;
			view(1, 2) = up.z;
			view(1, 3) = -Dot(Vec3(up), Vec3(position));

			view(2, 0) = -forward.x;
			view(2, 1) = -forward.y;
			view(2, 2) = -forward.z;
			view(2, 3) = -Dot(Vec3(forward), Vec3(position));

			return view;
		}

		Mat4 InvLookAt(Vec4 right, Vec4 up, Vec4 forward, Vec4 position)
		{
			Mat4 invView = Identity();

			invView(0, 0) = right.x;
			invView(0, 1) = up.x;
			invView(0, 2) = -forward.x;
			invView(0, 3) = position.x;

			invView(1, 0) = right.y;
			invView(1, 1) = up.y;
			invView(1, 2) = -forward.y;
			invView(1, 3) = position.y;

			invView(2, 0) = right.z;
			invView(2, 1) = up.z;
			invView(2, 2) = -forward.z;
			invView(2, 3) = position.z;

			return invView;
		}

		Mat4 Frustum(float near, float far, float right, float left, float top, float bottom)
		{
			Mat4 m = Identity();

			m(0, 0) = (2 * near) / (right - left);
			m(0, 2) = (right + left) / (right - left);

			m(1, 1) = (2 * near) / (top - bottom);
			m(1, 2) = (top + bottom) / (top - bottom);

			m(2, 2) = -(far + near) / (far - near);
			m(2, 3) = (-2 * far * near) / (far - near);

			m(3, 2) = -1;
			m(3, 3) = 0;

			return m;
		}
	}
}