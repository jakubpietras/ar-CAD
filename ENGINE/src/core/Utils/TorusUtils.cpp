#include <arpch.h>
#include "TorusUtils.h"

namespace ar
{

	std::vector<ar::VertexPosition> TorusUtils::GenerateTorusVertices(const TorusDesc& desc)
	{
		std::vector<VertexPosition> vertices;
		auto pi = static_cast<float>(acos(-1));
		float x, y, z, u, v;
		for (uint32_t i = 0; i < desc.SamplesV; i++)
		{
			for (uint32_t j = 0; j < desc.SamplesU; j++)
			{
				v = i * (2 * pi / desc.SamplesV);
				u = j * (2 * pi / desc.SamplesU);

				x = (desc.LargeRadius + desc.SmallRadius * cos(v)) * cos(u);
				y = desc.SmallRadius * sin(v);
				z = (desc.LargeRadius + desc.SmallRadius * cos(v)) * sin(u);
				vertices.push_back({ { x, y, z } });
			}
		}
		return vertices;
	}

	std::vector<std::vector<uint32_t>> TorusUtils::GenerateTorusEdges(const TorusDesc& desc)
	{
		std::vector<std::vector<uint32_t>> edges {};

		// going around the tube
		for (uint32_t i = 0; i < desc.SamplesV; i++)
		{
			std::vector<uint32_t> h_edge;
			for (uint32_t j = 0; j < desc.SamplesU; j++)
				h_edge.push_back(i * desc.SamplesU + j);
			edges.push_back(h_edge);
		}

		// going along the tube
		for (uint32_t i = 0; i < desc.SamplesU; i++)
		{
			std::vector<uint32_t> v_edge;
			for (uint32_t j = 0; j < desc.SamplesV; j++)
				v_edge.push_back(j * desc.SamplesU + i);
			edges.push_back(v_edge);
		}
		return edges;
	}

}