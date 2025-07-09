#include "matrix_types.h"
#include <iostream>
#include <iomanip>

namespace ar
{
	Mat4::Mat4(float init_value)
		: data()
	{
		data.fill(init_value);
	}
	float& Mat4::operator()(int row, int col)
	{
		return data.at(col * 4 + row);
	}
	const float& Mat4::operator()(int row, int col) const
	{
		return data.at(col * 4 + row);
	}
	Mat4 Mat4::operator*(const Mat4& other) const
	{
		auto product = Mat4(0.0f);
		for (int row = 0; row < 4; row++)
			for (int col = 0; col < 4; col++)
				for (int k = 0; k < 4; k++)
					product(row, col) += (*this)(row, k) * other(k, col);
		return product;
	}
	Vec4 operator*(const Vec4& vec, const Mat4& mat)
	{
		std::array<float, 4> v = { vec.x, vec.y, vec.z, vec.w },
			product = { };
		for (int col = 0; col < 4; col++)
			for (int k = 0; k < 4; k++)
				product[col] += v[k] * mat(k, col);
		return Vec4(product[0], product[1], product[2], product[3]);
	}
	Vec4 operator*(const Mat4& mat, const Vec4& vec)
	{
		std::array<float, 4> v = { vec.x, vec.y, vec.z, vec.w },
			product = { };
		for (int row = 0; row < 4; row++)
			for (int k = 0; k < 4; k++)
				product[row] += mat(row, k) * v[k];
		return Vec4(product[0], product[1], product[2], product[3]);
	}
	
	Mat4 Transpose(const Mat4& mat)
	{
		auto transpose = Mat4();
		for (int row = 0; row < 4; row++)
			for (int col = 0; col < 4; col++)
				transpose(row, col) = mat(col, row);
		return transpose;
	}
	Mat4 Diagonal(float x, float y, float z, float w)
	{
		auto diag = Mat4(0.0f);
		diag(0, 0) = x;
		diag(1, 1) = y;
		diag(2, 2) = z;
		diag(3, 3) = w;
		return diag;
	}
	Mat4 Diagonal(const Vec4& diagonal)
	{
		auto diag = Mat4(0.0f);
		diag(0, 0) = diagonal.x;
		diag(1, 1) = diagonal.y;
		diag(2, 2) = diagonal.z;
		diag(3, 3) = diagonal.w;
		return diag;
	}
	Mat4 Identity()
	{
		return Diagonal(1.0f, 1.0f, 1.0f, 1.0f);
	}
}