#include "solvers.h"

namespace ar
{
    namespace mat
    {
		std::vector<float> SolveTridiagonal(const std::vector<float>& lower, const std::vector<float>& main, const std::vector<float>& upper, const std::vector<float>& rhs)
		{
			auto size = rhs.size();
			auto scratch = std::vector<float>(size);
			std::vector<float> result = rhs;

			scratch[0] = upper[0] / main[0];
			result[0] /= main[0];

			// forward elimination
			for (int i = 1; i < size; i++)
			{
				if (i < size - 1)
					scratch[i] = upper[i] / (main[i] - lower[i - 1] * scratch[i - 1]);
				result[i] = (result[i] - lower[i - 1] * result[i - 1]) /
					(main[i] - lower[i - 1] * scratch[i - 1]);
			}

			// back substitution
			for (int i = static_cast<int>(size) - 2; i >= 0; i--)
			{
				result[i] -= scratch[i] * result[i + 1];
			}

			return result;
		}

		std::vector<Vec3> SolveTridiagonal(const std::vector<float>& lower, const std::vector<float>& main, const std::vector<float>& upper, const std::vector<Vec3>& rhs)
		{
			std::vector<float> rhs_x, rhs_y, rhs_z;
			for (const auto& vec : rhs) {
				rhs_x.push_back(vec.x);
				rhs_y.push_back(vec.y);
				rhs_z.push_back(vec.z);
			}

			auto result_x = SolveTridiagonal(lower, main, upper, rhs_x);
			auto result_y = SolveTridiagonal(lower, main, upper, rhs_y);
			auto result_z = SolveTridiagonal(lower, main, upper, rhs_z);

			std::vector<Vec3> result;
			for (int i = 0; i < result_x.size(); i++)
				result.push_back(Vec3(result_x[i], result_y[i], result_z[i]));

			return result;
		}

		ar::mat::Vec4 SolveLinear(Mat4 A, Vec4 b)
		{
			float M[4][5];

			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					M[i][j] = A(i, j);
				}
				M[i][4] = b[i];
			}

			for (int k = 0; k < 4; ++k) {
				int maxRow = k;
				double maxVal = abs(M[k][k]);
				for (int i = k + 1; i < 4; ++i) {
					if (abs(M[i][k]) > maxVal) {
						maxVal = abs(M[i][k]);
						maxRow = i;
					}
				}

				if (maxRow != k) {
					for (int j = k; j < 5; ++j) {
						std::swap(M[k][j], M[maxRow][j]);
					}
				}

				if (abs(M[k][k]) < 1e-12) {
					return {0.0, 0.0, 0.0, 0.0};
				}

				for (int i = k + 1; i < 4; ++i) {
					double factor = M[i][k] / M[k][k];
					for (int j = k; j < 5; ++j) {
						M[i][j] -= factor * M[k][j];
					}
				}
			}

			Vec4 x(0.0);
			x.w = 0.0;
			for (int i = 3; i >= 0; --i) {
				double sum = M[i][4];
				for (int j = i + 1; j < 4; ++j) {
					sum -= M[i][j] * x[j];
				}
				x[i] = sum / M[i][i];
			}

			return x;
		}

		ar::mat::Vec4d SolveLinear(Mat4d A, Vec4d b)
		{
			double M[4][5];

			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					M[i][j] = A(i, j);
				}
				M[i][4] = b[i];
			}

			for (int k = 0; k < 4; ++k) {
				int maxRow = k;
				double maxVal = abs(M[k][k]);
				for (int i = k + 1; i < 4; ++i) {
					if (abs(M[i][k]) > maxVal) {
						maxVal = abs(M[i][k]);
						maxRow = i;
					}
				}

				if (maxRow != k) {
					for (int j = k; j < 5; ++j) {
						std::swap(M[k][j], M[maxRow][j]);
					}
				}

				if (abs(M[k][k]) < 1e-12) {
					return { 0.0, 0.0, 0.0, 0.0 };
				}

				for (int i = k + 1; i < 4; ++i) {
					double factor = M[i][k] / M[k][k];
					for (int j = k; j < 5; ++j) {
						M[i][j] -= factor * M[k][j];
					}
				}
			}

			Vec4d x(0.0);
			x.w = 0.0;
			for (int i = 3; i >= 0; --i) {
				double sum = M[i][4];
				for (int j = i + 1; j < 4; ++j) {
					sum -= M[i][j] * x[j];
				}
				x[i] = sum / M[i][i];
			}

			return x;
		}

	}
}