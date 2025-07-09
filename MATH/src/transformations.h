#pragma once
#include "vector_types.h"
#include "matrix_types.h"
#include "trigonometry.h"

namespace ar
{
	/// <summary>
	/// Builds a 4x4 scaling matrix from individual scale values.
	/// </summary>
	/// <param name="scale_x">Scale factor along the X axis.</param>
	/// <param name="scale_y">Scale factor along the Y axis.</param>
	/// <param name="scale_z">Scale factor along the Z axis.</param>
	/// <returns>A 4x4 scaling matrix.</returns>
	Mat4 ScaleMatrix(float scale_x, float scale_y, float scale_z);

	/// <summary>
	/// Builds a 4x4 scaling matrix from a scale vector.
	/// </summary>
	/// <param name="scaling_factors">A vector containing scale factors for X, Y, and Z.</param>
	/// <returns>A 4x4 scaling matrix.</returns>
	Mat4 ScaleMatrix(const Vec3& scaling_factors);

	/// <summary>
	/// Builds a 4x4 rotation matrix from Euler angles in degrees (XYZ order).
	/// </summary>
	/// <param name="rotation_angles">A vector of angles (pitch, yaw, roll) in degrees.</param>
	/// <returns>A 4x4 rotation matrix.</returns>
	Mat4 RotationMatrix(const Vec3& rotation_angles);

	/// <summary>
	/// Builds a 4x4 rotation matrix from individual Euler angles in degrees (XYZ order).
	/// </summary>
	/// <param name="pitch">Rotation around the X axis, in degrees.</param>
	/// <param name="yaw">Rotation around the Y axis, in degrees.</param>
	/// <param name="roll">Rotation around the Z axis, in degrees.</param>
	/// <returns>A 4x4 rotation matrix.</returns>
	Mat4 RotationMatrix(float pitch, float yaw, float roll);

	/// <summary>
	/// Builds a 4x4 translation matrix from a translation vector.
	/// </summary>
	/// <param name="translation_vector">A vector containing X, Y, and Z translation values.</param>
	/// <returns>A 4x4 translation matrix.</returns>
	Mat4 TranslationMatrix(const Vec3& translation_vector);

	/// <summary>
	/// Builds a 4x4 translation matrix from individual translation values.
	/// </summary>
	/// <param name="dx">Translation along the X axis.</param>
	/// <param name="dy">Translation along the Y axis.</param>
	/// <param name="dz">Translation along the Z axis.</param>
	/// <returns>A 4x4 translation matrix.</returns>
	Mat4 TranslationMatrix(float dx, float dy, float dz);

	/// <summary>
	/// Builds an orthographic projection matrix.
	/// </summary>
	/// <param name="left">Left clipping plane.</param>
	/// <param name="right">Right clipping plane.</param>
	/// <param name="top">Top clipping plane.</param>
	/// <param name="bottom">Bottom clipping plane.</param>
	/// <param name="near">Near clipping plane.</param>
	/// <param name="far">Far clipping plane.</param>
	/// <returns>A 4x4 orthographic projection matrix.</returns>
	Mat4 Ortho(float left, float right, float top, float bottom, float near, float far);

	/// <summary>
	/// Builds a perspective projection matrix using vertical field of view.
	/// </summary>
	/// <param name="fov">Vertical field of view, in degrees.</param>
	/// <param name="aspect_ratio">Aspect ratio (width / height).</param>
	/// <param name="near">Near clipping plane.</param>
	/// <param name="far">Far clipping plane.</param>
	/// <returns>A 4x4 perspective projection matrix.</returns>
	Mat4 Perspective(float fov, float aspect_ratio, float near, float far);

	/// <summary>
	/// Builds the inverse of a perspective projection matrix.
	/// </summary>
	/// <param name="fov">Vertical field of view, in degrees.</param>
	/// <param name="aspect_ratio">Aspect ratio (width / height).</param>
	/// <param name="near">Near clipping plane.</param>
	/// <param name="far">Far clipping plane.</param>
	/// <returns>The inverse of the perspective projection matrix.</returns>
	Mat4 InvPerspective(float fov, float aspect_ratio, float near, float far);

	Mat4 Frustum(float near, float far, float right, float left, float top, float bottom);
}