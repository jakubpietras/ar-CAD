#pragma once

namespace ar
{
	enum class TextureFormat
	{
		R8,
		R32,
		R32F,
		RGBA8,
		D24S8
	};

	struct TextureDesc
	{
		uint32_t Width, Height;
		TextureFormat Format;
	};

	// for OpenGL, Texture class encompasses actual Textures, as well as Renderbuffers
	class Texture
	{
	public:
		virtual ~Texture() {}
		virtual void SetData(void* data, uint32_t size) = 0;
		static Texture* Create(const TextureDesc& desc);
		static Texture* Create(const std::string& filepath);
		inline const uint32_t GetID() const { return m_ID; };
		inline const void BindSlot(size_t slot) const { glBindTextureUnit(0, m_ID); }

	protected:
		uint32_t m_ID;
	};
}

