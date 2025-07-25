#pragma once
#include "core/Renderer/Texture.h"

namespace ar
{
	class OGLTexture : public Texture
	{
	public:
		OGLTexture(const TextureDesc& desc);
		OGLTexture(const std::string& filepath);

		virtual ~OGLTexture();
		void SetData(void* data, uint32_t size) override;
	private:
		GLenum GetDataFormat(TextureFormat format);
		GLenum GetGLInternalFormat(TextureFormat format);

		TextureDesc m_Description;
	};
}

