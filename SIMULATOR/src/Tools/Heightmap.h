#pragma once
#include "ARCAD.h"
#include "ARMAT.h"
#include "Milling/MaterialDesc.h"
#include "Milling/CutterType.h"
#include "Milling/MillingError.h"

class Heightmap
{
public:
	Heightmap(const MaterialDesc& material, std::vector<ar::mat::Vec3>& pathCoords);
	inline const ar::Ref<ar::Texture> GetTexture() const { return m_Texture; }
	inline const void LoadNewPath(std::vector<ar::mat::Vec3>& newCoords) const { m_PathCoords = newCoords; }
	
	void ResetMap(const MaterialDesc& material);
	ar::mat::Vec3 UpdateMap(const ar::mat::Vec3& start, float dt, float speed);
	void UpdateMapInstant(CutterType cutterType, float cutterRadius, float cutterHeight);

private:
	void InitPathBuffer();

	std::vector<ar::mat::Vec3>& m_PathCoords;
	ar::Ref<ar::Texture> m_Texture;
	ar::Ref<ar::ShaderStorageBuffer<std::vector<ar::mat::Vec3>>> m_PathBuffer;
	ar::Ref<ar::ShaderStorageBuffer<MillingError>> m_ErrorFlagsBuffer;
	ar::Ref<ar::ComputeShader> m_CompShader;

	uint32_t m_SamplesX, m_SamplesY;
	float m_SizeX, m_SizeY;
};