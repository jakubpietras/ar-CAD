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
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void UpdateData(void* data, uint32_t size) = 0;
		virtual void SetData(void* data, uint32_t size) = 0;
		static Texture* Create(const TextureDesc& desc);
		static Texture* Create(const std::string& filepath);
		inline const uint32_t GetID() const { return m_ID; };
		inline const void BindSlot(size_t slot) const { glBindTextureUnit(slot, m_ID); }
		virtual const void BindImageUnit(uint32_t unit, GLenum access) = 0;

	protected:
		TextureDesc m_Description;
		uint32_t m_ID;
	};
}

