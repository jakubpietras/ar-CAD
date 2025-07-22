#pragma once

namespace ar
{
	enum class TextureFormat
	{
		// todo: generalize to other APIs
		R8,
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
	};
}

