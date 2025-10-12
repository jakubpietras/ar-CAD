#pragma once
#include "ARCAD.h"
#include "ARMAT.h"
#include "Milling/MaterialDesc.h"

class Heightmap
{
public:
	Heightmap(size_t sizeX, size_t sizeY);
	inline const ar::Ref<ar::Texture> GetTexture() const { return m_Texture; }
	void ResetMap(const MaterialDesc& material);

private:
	std::vector<float> m_HeightData;
	ar::Ref<ar::Texture> m_Texture;
};