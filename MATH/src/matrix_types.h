#pragma once
#include <array>
#include "vector_types.h"

namespace ar
{
    namespace mat
    {
		/// <summary>
	/// Represents a 4x4 matrix in column-major order.
	/// </summary>
		struct Mat4
		{
			std::array<float, 16> data;

			/// <summary>
			/// Constructs a matrix with all elements set to the specified value.
			/// </summary>
			/// <param name="init_value">The value to fill all components with.</param>
			Mat4(float init_value = 0.0f);

			/// <summary>
			/// Accesses a matrix element by row and column (modifiable).
			/// </summary>
			/// <param name="row">Row index [0-3].</param>
			/// <param name="col">Column index [0-3].</param>
			/// <returns>A reference to the matrix element.</returns>
			float& operator()(int row, int col);

			/// <summary>
			/// Accesses a matrix element by row and column (read-only).
			/// </summary>
			/// <param name="row">Row index [0-3].</param>
			/// <param name="col">Column index [0-3].</param>
			/// <returns>A const reference to the matrix element.</returns>
			const float& operator()(int row, int col) const;

			/// <summary>
			/// Multiplies this matrix by another matrix.
			/// </summary>
			/// <param name="other">The matrix to multiply with.</param>
			/// <returns>The resulting matrix product.</returns>
			Mat4 operator*(const Mat4& other) const;
		};

		/// <summary>
		/// Returns the transpose of a 4x4 matrix (flips rows and columns).
		/// </summary>
		/// <param name="mat">The matrix to transpose.</param>
		/// <returns>The transposed matrix.</returns>
		Mat4 Transpose(const Mat4& mat);

		/// <summary>
		/// Creates a diagonal matrix with the specified elements on the diagonal.
		/// </summary>
		/// <param name="x">Value on the [0,0] position.</param>
		/// <param name="y">Value on the [1,1] position.</param>
		/// <param name="z">Value on the [2,2] position.</param>
		/// <param name="w">Value on the [3,3] position.</param>
		/// <returns>A 4x4 diagonal matrix.</returns>
		Mat4 Diagonal(float x, float y, float z, float w);

		/// <summary>
		/// Creates a diagonal matrix from a 4D vector.
		/// </summary>
		/// <param name="diagonal">The values to place on the matrix diagonal.</param>
		/// <returns>A 4x4 diagonal matrix.</returns>
		Mat4 Diagonal(const Vec4& diagonal);

		/// <summary>
		/// Creates a 4x4 identity matrix (1s on the diagonal, 0 elsewhere).
		/// </summary>
		/// <returns>The identity matrix.</returns>
		Mat4 Identity();

		/// <summary>
		/// Multiplies a 4D vector by a 4x4 matrix (vector on the left-hand side).
		/// </summary>
		/// <param name="vec">The vector to transform.</param>
		/// <param name="mat">The matrix to apply.</param>
		/// <returns>The transformed vector.</returns>
		Vec4 operator*(const Vec4& vec, const Mat4& mat);

		/// <summary>
		/// Multiplies a 4x4 matrix by a 4D vector (vector on the right-hand side).
		/// </summary>
		/// <param name="mat">The matrix to apply.</param>
		/// <param name="vec">The vector to transform.</param>
		/// <returns>The transformed vector.</returns>
		Vec4 operator*(const Mat4& mat, const Vec4& vec);
    }
}