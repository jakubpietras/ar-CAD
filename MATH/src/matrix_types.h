#pragma once
#include <array>
#include "vector_types.h"
#include <cassert>

namespace ar
{
    namespace mat
    {
		/// <summary>
	/// Represents a 4x4 matrix in column-major order.
	/// </summary>
		template <typename T, size_t Rows, size_t Cols>
		struct Mat
		{
			std::array<T, Rows * Cols> data;

			/// <summary>
			/// Constructs a matrix with all elements set to the specified value.
			/// </summary>
			/// <param name="init_value">The value to fill all components with.</param>
			constexpr Mat(T init_value = T(0))
			{
				data.fill(init_value);
			}

			/// <summary>
			/// Accesses a matrix element by row and column (modifiable).
			/// </summary>
			/// <param name="row">Row index [0-3].</param>
			/// <param name="col">Column index [0-3].</param>
			/// <returns>A reference to the matrix element.</returns>
			constexpr T& operator()(size_t row, size_t col)
			{
				assert(row < Rows && col < Cols);
				return data[col * Rows + row];
			}

			/// <summary>
			/// Accesses a matrix element by row and column (read-only).
			/// </summary>
			/// <param name="row">Row index [0-3].</param>
			/// <param name="col">Column index [0-3].</param>
			/// <returns>A const reference to the matrix element.</returns>
			constexpr const T& operator()(size_t row, size_t col) const
			{
				assert(row < Rows && col < Cols);
				return data[col * Rows + row];
			}

			/// <summary>
			/// Multiplies this matrix by another matrix.
			/// </summary>
			/// <param name="other">The matrix to multiply with.</param>
			/// <returns>The resulting matrix product.</returns>
			constexpr Mat operator*(const Mat& other) const 
			{
				static_assert(Rows == Cols, "Only square matrices supported for *");
				Mat result(T(0));
				for (size_t r = 0; r < Rows; ++r) {
					for (size_t c = 0; c < Cols; ++c) {
						T sum = T(0);
						for (size_t k = 0; k < Cols; ++k) {
							sum += (*this)(r, k) * other(k, c);
						}
						result(r, c) = sum;
					}
				}
				return result;
			}
		};
		using Mat4f = Mat<float, 4, 4>;
		using Mat4d = Mat<double, 4, 4>;
		//using Mat4 = Mat4f;


		/// <summary>
		/// Returns the transpose of a 4x4 matrix (flips rows and columns).
		/// </summary>
		/// <param name="mat">The matrix to transpose.</param>
		/// <returns>The transposed matrix.</returns>
		template <typename T>
		constexpr Mat<T, 4, 4> Transpose(const Mat<T, 4, 4>& mat)
		{
			Mat<T, 4, 4> result(T(0));
			for (size_t r = 0; r < 4; ++r) 
			{
				for (size_t c = 0; c < 4; ++c) 
					result(r, c) = mat(c, r);
			}
			return result;
		}

		/// <summary>
		/// Creates a diagonal matrix with the specified elements on the diagonal.
		/// </summary>
		/// <param name="x">Value on the [0,0] position.</param>
		/// <param name="y">Value on the [1,1] position.</param>
		/// <param name="z">Value on the [2,2] position.</param>
		/// <param name="w">Value on the [3,3] position.</param>
		/// <returns>A 4x4 diagonal matrix.</returns>
		template <typename T>
		constexpr Mat<T, 4, 4> Diagonal(T x, T y, T z, T w)
		{
			Mat<T, 4, 4> diag(T(0));
			diag(0, 0) = x;
			diag(1, 1) = y;
			diag(2, 2) = z;
			diag(3, 3) = w;
			return diag;
		}

		/// <summary>
		/// Creates a diagonal matrix from a 4D vector.
		/// </summary>
		/// <param name="diagonal">The values to place on the matrix diagonal.</param>
		/// <returns>A 4x4 diagonal matrix.</returns>
		template <typename T>
		constexpr Mat<T, 4, 4> Diagonal(const Vec4T<T>& diagonal)
		{
			Mat<T, 4, 4> m(T(0));
			m(0, 0) = diagonal.x;
			m(1, 1) = diagonal.y;
			m(2, 2) = diagonal.z;
			m(3, 3) = diagonal.w;
			return m;
		}

		/// <summary>
		/// Creates a 4x4 identity matrix (1s on the diagonal, 0 elsewhere).
		/// </summary>
		/// <returns>The identity matrix.</returns>
		template <typename T>
		constexpr Mat<T, 4, 4> Identity()
		{
			Mat<T, 4, 4> m(T(0));
			for (int i = 0; i < 4; i++) 
				m(i, i) = T(1);
			return m;
		}

		/// <summary>
		/// Multiplies a 4D vector by a 4x4 matrix (vector on the left-hand side).
		/// </summary>
		/// <param name="vec">The vector to transform.</param>
		/// <param name="mat">The matrix to apply.</param>
		/// <returns>The transformed vector.</returns>
		template <typename T>
		constexpr Vec4T<T> operator*(const Vec4T<T>& vec, const Mat<T, 4, 4>& mat)
		{
			Vec4T<T> result;
			for (int c = 0; c < 4; ++c) 
			{
				result[c] = vec.x * mat(0, c) +
					vec.y * mat(1, c) +
					vec.z * mat(2, c) +
					vec.w * mat(3, c);
			}
			return result;
		}

		/// <summary>
		/// Multiplies a 4x4 matrix by a 4D vector (vector on the right-hand side).
		/// </summary>
		/// <param name="mat">The matrix to apply.</param>
		/// <param name="vec">The vector to transform.</param>
		/// <returns>The transformed vector.</returns>
		template <typename T>
		constexpr Vec4T<T> operator*(const Mat<T, 4, 4>& m, const Vec4T<T>& v) 
		{
			Vec4T<T> result;
			result.x = m(0, 0) * v.x + m(0, 1) * v.y + m(0, 2) * v.z + m(0, 3) * v.w;
			result.y = m(1, 0) * v.x + m(1, 1) * v.y + m(1, 2) * v.z + m(1, 3) * v.w;
			result.z = m(2, 0) * v.x + m(2, 1) * v.y + m(2, 2) * v.z + m(2, 3) * v.w;
			result.w = m(3, 0) * v.x + m(3, 1) * v.y + m(3, 2) * v.z + m(3, 3) * v.w;
			return result;
		}

		// ================== Backwards compatibility

		struct Mat4 : public Mat<float, 4, 4>
		{
			using Base = Mat<float, 4, 4>;
			// Inherit all constructors
			using Mat<float, 4, 4>::Mat;

			// Default constructor
			constexpr Mat4() : Mat<float, 4, 4>(0.0f) {}

			// Constructor from value
			constexpr Mat4(float init_value) : Mat<float, 4, 4>(init_value) {}

			// Copy constructor from base
			constexpr Mat4(const Mat<float, 4, 4>& other) : Mat<float, 4, 4>(other) {}

			constexpr float& operator()(size_t row, size_t col) 
			{
				return Base::operator()(row, col);
			}
			constexpr const float& operator()(size_t row, size_t col) const {
				return Base::operator()(row, col);
			}

			// Add explicit operator* that returns Mat4
			constexpr Mat4 operator*(const Mat4& other) const {
				Mat<float, 4, 4> result = static_cast<const Mat<float, 4, 4>&>(*this).operator*(other);
				return Mat4(result);
			}
		};

		// Add the vector multiplication operator specifically for Mat4
		inline Vec4 operator*(const Mat4& mat, const Vec4& vec) {
			return operator*<float>(static_cast<const Mat<float, 4, 4>&>(mat), vec);
		}
		inline Mat4 Identity() {
			return Identity<float>();
		}
		inline Mat4 Diagonal(const Vec4& diag) {
			return Diagonal<float>(diag);
		}
		inline Mat4 Transpose(const Mat4& mat) {
			return Transpose<float>(mat);
		}
    }
}