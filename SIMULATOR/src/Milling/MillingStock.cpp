#include "MillingStock.h"
#include "core/Renderer/Renderer.h"
#include "core/Renderer/Texture.h"
#include "core/Scene/DebugRenderer.h"
// #define DEBUG

MillingStock::MillingStock(const MaterialDesc& material)
	: m_Material(material)
{
	m_TopMesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
	m_SideMesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
	UpdateMaterialDesc(material);
	InitTexture();
	ar::DebugRenderer::AddPoint({ 0.f, 6.0f, 0.f }, { 1.f, 1.f, 0.f });
}

void MillingStock::UpdateMaterialDesc(const MaterialDesc& material)
{
	m_Material = material;
	//m_HMap->ResetMap(material);
	GenerateTopMesh();
	GenerateSideMesh();
}

void MillingStock::Render(ar::mat::Mat4 vpMat, ar::mat::Vec3 cameraPos, ar::Ref<ar::Texture> heightMap)
{

	const ar::mat::Vec3 lightPos = { 0.f, 6.0f, 0.f };
	const ar::mat::Vec3 lightColor = { 1.f, 1.f, 1.f };

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);ds

	if (!m_TopMesh)
		return;
	auto shaderTop = ar::ShaderLib::Get("MillingTop");
	shaderTop->SetMat4("u_VP", vpMat);
	shaderTop->SetVec3("u_CameraPos", cameraPos);
	shaderTop->SetVec3("u_LightPos", lightPos);
	shaderTop->SetVec3("u_LightColor", lightColor);
	ar::RenderCommand::BindTexture(heightMap, 0);
	shaderTop->SetInt("u_Heightmap", 0);
	ar::RenderCommand::BindTexture(m_MetalTex, 1);
	shaderTop->SetInt("u_Texture", 1);
	ar::Renderer::Submit(ar::Primitive::Triangle, shaderTop, m_TopMesh, m_TopMesh->IsIndexed());

	auto shaderSide = ar::ShaderLib::Get("MillingSide");
	shaderSide->SetMat4("u_VP", vpMat);
	shaderSide->SetVec3("u_CameraPos", cameraPos);
	shaderSide->SetVec3("u_LightPos", lightPos);
	shaderSide->SetVec3("u_LightColor", lightColor);
	shaderSide->SetVec3("u_SurfaceColor", { 0.5f, 0.5f, 1.0f });
	ar::RenderCommand::BindTexture(heightMap, 0);
	shaderSide->SetInt("u_Heightmap", 0);
	ar::Renderer::Submit(ar::Primitive::Triangle, shaderSide, m_SideMesh, m_SideMesh->IsIndexed());
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	ar::DebugRenderer::Render(vpMat);

#ifdef DEBUG
	ar::DebugRenderer::Render(vpMat);
#endif
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
#ifdef DEBUG
			ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
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

ar::Ref<ar::VertexArray> MillingStock::GenerateSideMesh()
{
	// Called only when the parameters change (size or samples)
	if (!m_SideMesh)
		m_SideMesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());

	m_SideMesh->ClearBuffers();
	auto topVerts = GenerateVertsSide();
	auto topIndices = GenerateIndicesSide();
	m_SideMesh->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(topVerts)));
	m_SideMesh->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(topIndices)));
	return m_SideMesh;
}

std::vector<ar::VertexPosNormTex> MillingStock::GenerateVertsSide()
{
	std::vector<ar::VertexPosNormTex> verts;
	size_t size = 4 * (m_Material.Samples.u + m_Material.Samples.v);
	verts.reserve(size);

	float offsetX = -m_Material.Size.x / 2;
	float offsetZ = -m_Material.Size.z / 2;

	float segLenU = m_Material.Size.x / (m_Material.Samples.u - 1);
	float segLenV = m_Material.Size.z / (m_Material.Samples.v - 1);
	float texLenU = 1.0f / (m_Material.Samples.u - 1);
	float texLenV = 1.0f / (m_Material.Samples.v - 1);

	// Bottom
	for (int segU = 0; segU < m_Material.Samples.u; segU++)
	{
		const int segV = 0;
		ar::mat::Vec3 pos = { offsetX + segU * segLenU, m_Material.BaseHeight, offsetZ + segV * segLenV };
		ar::mat::Vec3 normal = { 0.0f, 0.0f, -1.0f };
		ar::mat::Vec2 tex = { segU * texLenU, segV * texLenV };
		verts.push_back({ pos, normal, tex });
#ifdef DEBUG
		ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
		pos = { offsetX + segU * segLenU, 0.0, offsetZ + segV * segLenV };
		normal = { 0.0f, 0.0f, -1.0f };
		tex = { -1.0, -1.0 };
		verts.push_back({ pos, normal, tex });
#ifdef DEBUG
		ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
	}

	// Right
	for (int segV = 0; segV < m_Material.Samples.v; segV++)
	{
		const int segU = m_Material.Samples.u - 1;
		ar::mat::Vec3 pos = { offsetX + segU * segLenU, m_Material.BaseHeight, offsetZ + segV * segLenV };
		ar::mat::Vec3 normal = { 1.0f, 0.0f, 0.0f };
		ar::mat::Vec2 tex = { segU * texLenU, segV * texLenV };
		verts.push_back({ pos, normal, tex });
#ifdef DEBUG
		ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
		pos = { offsetX + segU * segLenU, 0.0, offsetZ + segV * segLenV };
		normal = { 1.0f, 0.0f, 0.0f };
		tex = { -1.0, -1.0 };
		verts.push_back({ pos, normal, tex });
#ifdef DEBUG
		ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
	}

	// Top
	for (int segU = 0; segU < m_Material.Samples.u; segU++)
	{
		const int segV = m_Material.Samples.v - 1;
		ar::mat::Vec3 pos = { offsetX + segU * segLenU, m_Material.BaseHeight, offsetZ + segV * segLenV };
		ar::mat::Vec3 normal = { 0.0f, 0.0f, 1.0f };
		ar::mat::Vec2 tex = { segU * texLenU, segV * texLenV };
		verts.push_back({ pos, normal, tex });
#ifdef DEBUG
		ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
		pos = { offsetX + segU * segLenU, 0.0, offsetZ + segV * segLenV };
		normal = { 0.0f, 0.0f, 1.0f };
		tex = { -1.0, -1.0 };
		verts.push_back({ pos, normal, tex });
#ifdef DEBUG
		ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
	}

	// Left
	for (int segV = 0; segV < m_Material.Samples.v; segV++)
	{
		const int segU = 0;
		ar::mat::Vec3 pos = { offsetX + segU * segLenU, m_Material.BaseHeight, offsetZ + segV * segLenV };
		ar::mat::Vec3 normal = { -1.0f, 0.0f, 0.0f };
		ar::mat::Vec2 tex = { segU * texLenU, segV * texLenV };
		verts.push_back({ pos, normal, tex });
#ifdef DEBUG
		ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
		pos = { offsetX + segU * segLenU, 0.0, offsetZ + segV * segLenV };
		normal = { -1.0f, 0.0f, 0.0f };
		tex = { -1.0, -1.0 };
		verts.push_back({ pos, normal, tex });
#ifdef DEBUG
		ar::DebugRenderer::AddLine(pos, pos + normal);
#endif
	}

	return verts;
}

std::vector<uint32_t> MillingStock::GenerateIndicesSide()
{
	std::vector<uint32_t> indices;
	uint32_t offset = 0;

	// Bottom side (Samples.u vertices)
	for (uint32_t u = 0; u < m_Material.Samples.u - 1; u++)
	{
		indices.push_back(offset + u * 2);
		indices.push_back(offset + u * 2 + 1);
		indices.push_back(offset + (u + 1) * 2);

		indices.push_back(offset + u * 2 + 1);
		indices.push_back(offset + (u + 1) * 2 + 1);
		indices.push_back(offset + (u + 1) * 2);
	}
	offset += m_Material.Samples.u * 2;

	// Right side (Samples.v vertices)
	for (uint32_t v = 0; v < m_Material.Samples.v - 1; v++)
	{
		indices.push_back(offset + v * 2);
		indices.push_back(offset + v * 2 + 1);
		indices.push_back(offset + (v + 1) * 2);

		indices.push_back(offset + v * 2 + 1);
		indices.push_back(offset + (v + 1) * 2 + 1);
		indices.push_back(offset + (v + 1) * 2);
	}
	offset += m_Material.Samples.v * 2;

	// Top side (Samples.u vertices)
	for (uint32_t u = 0; u < m_Material.Samples.u - 1; u++)
	{
		indices.push_back(offset + u * 2);
		indices.push_back(offset + u * 2 + 1);
		indices.push_back(offset + (u + 1) * 2);

		indices.push_back(offset + u * 2 + 1);
		indices.push_back(offset + (u + 1) * 2 + 1);
		indices.push_back(offset + (u + 1) * 2);
	}
	offset += m_Material.Samples.u * 2;

	// Left side (Samples.v vertices)
	for (uint32_t v = 0; v < m_Material.Samples.v - 1; v++)
	{
		indices.push_back(offset + v * 2);
		indices.push_back(offset + v * 2 + 1);
		indices.push_back(offset + (v + 1) * 2);

		indices.push_back(offset + v * 2 + 1);
		indices.push_back(offset + (v + 1) * 2 + 1);
		indices.push_back(offset + (v + 1) * 2);
	}

	return indices;
}

void MillingStock::InitTexture()
{
	m_MetalTex = ar::Ref<ar::Texture>(ar::Texture::Create("resources/textures/marble.jpg"));
}
