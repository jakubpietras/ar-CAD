#pragma once
#include <vector>
#include "ARMAT.h"
#include "core/Renderer/VertexTypes.h"

struct ShapeData
{
	std::vector<ar::VertexPositionNormal> Vertices;
	std::vector<uint32_t> Indices;
};

namespace ar
{
	class Shapes
	{
	public:
		ShapeData GetCylinder(float radius, float height, ar::mat::Vec3 origin, uint32_t sectorCount = 100);
		ShapeData GetHemisphere(float radius, ar::mat::Vec3 origin);
		ShapeData GetRoundCylinder(float radius, float height);
	private:
	};
}