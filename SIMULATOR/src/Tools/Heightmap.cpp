#include "Heightmap.h"

Heightmap::Heightmap(size_t sizeX, size_t sizeY)
{

}

void Heightmap::ResetMap(const MaterialDesc& material)
{
	auto effectiveHeight = material.Size.y - material.BaseHeight;
	m_HeightData = std::vector<float>(material.Samples.u * material.Samples.v, effectiveHeight);
}
