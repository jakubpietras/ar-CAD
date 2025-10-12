#pragma once
#include "ARMAT.h"
#include "ARCAD.h"
#include "Tools/Heightmap.h"
#include "MaterialDesc.h"

class MillingStock
{
public:
	MillingStock(const MaterialDesc& material);
	
	void UpdateMaterialDesc(const MaterialDesc& material);
	void Render(ar::mat::Mat4 vpMat, ar::mat::Vec3 cameraPos);
	inline const ar::Ref<ar::VertexArray> GetMesh() const { return m_TopMesh; }

private:
	ar::Ref<ar::VertexArray> GenerateTopMesh();
	std::vector<ar::VertexPosNormTex> GenerateVertsTop();
	std::vector<uint32_t> GenerateIndicesTop();

	ar::Ref<ar::VertexArray> GenerateSideMesh();
	std::vector<ar::VertexPosNormTex> GenerateVertsSide();
	std::vector<uint32_t> GenerateIndicesSide();
	void SetupHeightmap();
	void InitTexture();

	MaterialDesc m_Material;
	ar::Ref<ar::VertexArray> m_TopMesh, m_SideMesh;
	ar::Ref<ar::Texture> m_HMap, m_MetalTex;
};