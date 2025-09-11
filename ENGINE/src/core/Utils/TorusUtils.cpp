#include <arpch.h>
#include "TorusUtils.h"
#include "core/Scene/Components.h"
#include <numbers>

namespace ar
{

	std::vector<ar::VertexPositionUVID> TorusUtils::GenerateTorusVertices(const TorusDesc& desc, uint32_t id)
	{
		std::vector<VertexPositionUVID> vertices;
		auto pi = static_cast<float>(acos(-1));
		float x, y, z, u, v;
		for (uint32_t i = 0; i < desc.Samples.v; i++)
		{
			for (uint32_t j = 0; j < desc.Samples.u; j++)
			{
				auto twoPi = 2 * std::numbers::pi;
				v = static_cast<float>(i) / desc.Samples.v;
				u = static_cast<float>(j) / desc.Samples.u;

				x = (desc.LargeRadius + desc.SmallRadius * cos(v * twoPi)) * cos(u * twoPi);
				y = desc.SmallRadius * sin(v * twoPi);
				z = (desc.LargeRadius + desc.SmallRadius * cos(v * twoPi)) * sin(u * twoPi);
				vertices.push_back({ { x, y, z }, {u, v}, id});
			}
		}
		return vertices;
	}

	std::vector<std::vector<uint32_t>> TorusUtils::GenerateTorusEdges(const TorusDesc& desc)
	{
		std::vector<std::vector<uint32_t>> edges {};

		// going around the tube
		for (uint32_t i = 0; i < desc.Samples.v; i++)
		{
			std::vector<uint32_t> h_edge;
			for (uint32_t j = 0; j < desc.Samples.u; j++)
				h_edge.push_back(i * desc.Samples.u + j);
			edges.push_back(h_edge);
		}

		// going along the tube
		for (uint32_t i = 0; i < desc.Samples.u; i++)
		{
			std::vector<uint32_t> v_edge;
			for (uint32_t j = 0; j < desc.Samples.v; j++)
				v_edge.push_back(j * desc.Samples.u + i);
			edges.push_back(v_edge);
		}
		return edges;
	}

}