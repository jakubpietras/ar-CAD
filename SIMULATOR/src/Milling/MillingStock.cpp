#include "MillingStock.h"
#include "core/Renderer/Renderer.h"
#include "core/Renderer/Texture.h"

MillingStock::MillingStock(const MaterialDesc& material)
	: m_Material(material)
{
	m_TopMesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
	UpdateMaterialDesc(material);
	SetupHeightmap();
}

void MillingStock::UpdateMaterialDesc(const MaterialDesc& material)
{
	m_Material = material;
	//m_HMap->ResetMap(material);
	GenerateTopMesh();
}

void MillingStock::Render(ar::mat::Mat4 vpMat, ar::mat::Vec3 cameraPos)
{
	if (!m_TopMesh)
		return;
	auto shader = ar::ShaderLib::Get("MillingTop");
	shader->SetMat4("u_VP", vpMat);
	shader->SetVec3("u_CameraPos", cameraPos);
	shader->SetVec3("u_LightPos", { 1, 6, 0 });
	shader->SetVec3("u_LightColor", { 1.f, 1.f, 1.f });
	ar::RenderCommand::BindTexture(m_HMap, 0);
	shader->SetInt("u_Heightmap", 0);
	ar::Renderer::Submit(ar::Primitive::Triangle, shader, m_TopMesh, m_TopMesh->IsIndexed());
}

ar::Ref<ar::VertexArray> MillingStock::GenerateTopMesh()
{
	// Called only when the parameters change (size or samples)
	if (!m_TopMesh)
		m_TopMesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
	
	m_TopMesh->ClearBuffers();
	auto topVerts = GenerateVertsTop();
	auto topIndices = GenerateIndicesTop();
	m_TopMesh->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(topVerts)));
	m_TopMesh->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(topIndices)));
	return m_TopMesh;
}

std::vector<ar::VertexPosNormTex> MillingStock::GenerateVertsTop()
{
	std::vector<ar::VertexPosNormTex> verts;
	size_t size = m_Material.Samples.u * m_Material.Samples.v;
	verts.reserve(size);

	float offsetX = -m_Material.Size.x / 2;
	float offsetZ = -m_Material.Size.z / 2;

	float segLenU = m_Material.Size.x / (m_Material.Samples.u - 1);
	float segLenV = m_Material.Size.z / (m_Material.Samples.v - 1);
	float texLenU = 1.0f / (m_Material.Samples.u - 1);
	float texLenV = 1.0f / (m_Material.Samples.v - 1);

	for (int segV = 0; segV < m_Material.Samples.v; segV++)
	{
		for (int segU = 0; segU < m_Material.Samples.u; segU++)
		{
			ar::mat::Vec3 pos = { offsetX + segU * segLenU, m_Material.BaseHeight, offsetZ + segV * segLenV };
			ar::mat::Vec3 normal = { 0.0f, 1.0f, 0.0f };
			ar::mat::Vec2 tex = { segU * texLenU, segV * texLenV };
			verts.push_back({ pos, normal, tex });
		}
	}
	return verts;
}

std::vector<uint32_t> MillingStock::GenerateIndicesTop() 
{
	std::vector<uint32_t> indices;
	for (uint32_t v = 0; v < m_Material.Samples.v - 1; v++) 
	{
		for (uint32_t u = 0; u < m_Material.Samples.u - 1; u++)
		{ 
			// first triangle 
			
			indices.push_back(v * m_Material.Samples.u + u);
			indices.push_back((v + 1) * m_Material.Samples.u + u);
			indices.push_back(v * m_Material.Samples.u + u + 1);
			
			// second triangle
			indices.push_back((v + 1) * m_Material.Samples.u + u);
			indices.push_back((v + 1) * m_Material.Samples.u + u + 1);
			indices.push_back(v * m_Material.Samples.u + u + 1);
		} 
	} 
	return indices;
}

void MillingStock::SetupHeightmap()
{
	ar::TextureDesc desc{};
	desc.Format = ar::TextureFormat::R32F;
	desc.Width = m_Material.Samples.u;
	desc.Height = m_Material.Samples.v;

	m_HMap = ar::Ref<ar::Texture>(ar::Texture::Create(desc));
	auto height = m_Material.Size.y - m_Material.BaseHeight;
	std::vector<float> initData(desc.Width * desc.Height, height);
	m_HMap->SetData(initData.data(), desc.Width * desc.Height);
}
