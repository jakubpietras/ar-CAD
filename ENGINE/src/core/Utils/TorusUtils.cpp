#include <arpch.h>
#include "TorusUtils.h"

namespace ar
{

	std::vector<ar::VertexPositionID> TorusUtils::GenerateTorusVertices(const TorusDesc& desc, uint32_t id)
	{
		std::vector<VertexPositionID> vertices;
		auto pi = static_cast<float>(acos(-1));
		float x, y, z, u, v;
		for (uint32_t i = 0; i < desc.Samples.v; i++)
		{
			for (uint32_t j = 0; j < desc.Samples.u; j++)
			{
				v = i * (2 * pi / desc.Samples.v);
				u = j * (2 * pi / desc.Samples.u);

				x = (desc.LargeRadius + desc.SmallRadius * cos(v)) * cos(u);
				y = desc.SmallRadius * sin(v);
				z = (desc.LargeRadius + desc.SmallRadius * cos(v)) * sin(u);
				vertices.push_back({ { x, y, z }, id });
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