#include "Heightmap.h"

Heightmap::Heightmap(const MaterialDesc& material, std::vector<ar::mat::Vec3> pathCoords)
	: m_SamplesX(material.Samples.u), m_SamplesY(material.Samples.v),
	m_SizeX(material.Size.x), m_SizeY(material.Size.z), m_PathCoords(pathCoords),
	m_PathBuffer(std::make_shared<ar::ShaderStorageBuffer<std::vector<ar::mat::Vec3>>>()),
	m_ErrorFlagsBuffer(std::make_shared<ar::ShaderStorageBuffer<MillingError>>())
{
	ar::TextureDesc desc{};
	desc.Format = ar::TextureFormat::R32F;
	desc.Width = material.Samples.u;
	desc.Height = material.Samples.v;
	m_Texture = ar::Ref<ar::Texture>(ar::Texture::Create(desc));
	auto height = material.Size.y - material.BaseHeight;
	std::vector<float> initData(desc.Width * desc.Height, height);
	m_Texture->SetData(initData.data(), desc.Width * desc.Height);
	m_CompShader = ar::Ref<ar::ComputeShader>(ar::ComputeShader::Create("resources/shaders/OpenGL/milling.comp"));
}

void Heightmap::ResetMap(const MaterialDesc& newMaterial)
{
	m_SamplesX = newMaterial.Samples.u;
	m_SamplesY = newMaterial.Samples.v;
	m_SizeX = newMaterial.Size.x;
	m_SizeY = newMaterial.Size.z;

	auto height = newMaterial.Size.y - newMaterial.BaseHeight;
	std::vector<float> initData(newMaterial.Samples.u * newMaterial.Samples.v, height);
	m_Texture->SetData(initData.data(), newMaterial.Samples.u * newMaterial.Samples.v);
}

ar::mat::Vec3 Heightmap::UpdateMap(const ar::mat::Vec3& start, float dt, float speed)
{
	// todo sxss
	return ar::mat::Vec3();
}

void Heightmap::UpdateMapInstant(CutterType cutterType, float cutterRadius, float cutterHeight,
	float baseHeight)
{
	MillingError initial = {};
	std::vector<ar::mat::Vec4> pc;
	for (auto& coord : m_PathCoords)
		pc.push_back(ar::mat::ToVec4(coord));
	
	m_PathBuffer->UpdateData(pc.data(), pc.size() * sizeof(ar::mat::Vec4));
	m_ErrorFlagsBuffer->UpdateData(&initial, sizeof(initial));
	auto pathSegments = static_cast<unsigned int>(pc.size() - 1);
	bool isFlat = cutterType == CutterType::FLAT;

	m_Texture->BindImageUnit(0, GL_READ_WRITE);
	m_PathBuffer->Bind(1);
	m_ErrorFlagsBuffer->Bind(2);
	m_CompShader->SetUInt("u_PathSegments", pathSegments);
	m_CompShader->SetFloat("u_BaseHeight", baseHeight);
	m_CompShader->SetFloat("u_CutterRadius", cutterRadius);
	m_CompShader->SetFloat("u_CutterHeight", cutterHeight);
	m_CompShader->SetBool("u_IsCutterFlat", isFlat);
	m_CompShader->SetFloat("u_TexelWidth", m_SizeX / (m_SamplesX - 1));
	m_CompShader->SetFloat("u_TexelHeight", m_SizeY / (m_SamplesY - 1));
	m_CompShader->SetFloat("u_OffsetX", -m_SizeX / 2.0f);
	m_CompShader->SetFloat("u_OffsetY", -m_SizeY / 2.0f);

	ar::RenderCommand::DispatchCompute(m_CompShader, (m_SamplesX + 15) / 16, (m_SamplesY + 15) / 16, 1);
	ar::RenderCommand::MemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	// check errors
	MillingError result;
	m_ErrorFlagsBuffer->ReadData(&result, sizeof(result));
	if (result.DownMilling) AR_ERROR("Down milling detected");
	if (result.NonCuttingContact) AR_ERROR("Non-cutting part contact detected");
	if (result.OverPlunge) AR_ERROR("Cutter drilling into base detected");
}

void Heightmap::InitPathBuffer()
{
	m_PathBuffer->UpdateData(m_PathCoords.data(), m_PathCoords.size() * sizeof(ar::mat::Vec3));
}
