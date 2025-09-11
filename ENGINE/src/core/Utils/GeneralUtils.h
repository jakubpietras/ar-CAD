#pragma once
#include <vector>
#include "core/Renderer/VertexTypes.h"
#include "core/Scene/Entity.h"
#include "core/Scene/SceneFactory.h"

namespace ar
{
	class GeneralUtils
	{
	public:
		static std::vector<VertexPositionID> GetVertexData(std::vector<Entity> points, uint32_t id);
		static std::vector<VertexPositionID> GetVertexData(std::vector<ar::mat::Vec3> positions, uint32_t id);
		static std::vector<mat::Vec3> GetPos(std::vector<ar::Entity>& objects);
		static bool IsWrappedU(ar::Entity e);
		static bool IsWrappedV(ar::Entity e);
		static std::vector<ar::Entity> CreateScenePoints(std::vector<ar::mat::Vec3> positions, ar::SceneFactory& factory);

		template <typename T>
		static std::vector<T> SampleElementsN(const std::vector<T>& elements, size_t n)
		{
			if (elements.size() < n || n == 0)
				return {};

			if (n == 1)
				return { elements.front() };

			std::vector<T> sampled;
			sampled.reserve(n);

			size_t N = elements.size();
			for (size_t k = 0; k < n; k++)
			{
				size_t index = (k * (N - 1)) / (n - 1);
				sampled.push_back(elements[index]);
			}

			return sampled;
		}
	};
}