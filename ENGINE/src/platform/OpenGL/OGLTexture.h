#pragma once
#include <glad/glad.h>
#include "core/Renderer/Texture.h"

namespace ar
{
	class OGLTexture : public Texture
	{
	public:
		OGLTexture(const TextureDesc& desc);
		OGLTexture(const std::string& filepath);

		virtual ~OGLTexture();
		void Resize(uint32_t width, uint32_t height) override;
		void UpdateData(void* data, uint32_t size = 0) override;
		void SetData(void* data, uint32_t size) override;

	private:
		GLenum GetDataFormat(TextureFormat format);
		GLenum GetGLInternalFormat(TextureFormat format);

		TextureDesc m_Description;

		// Inherited via Texture
		const void BindImageUnit(uint32_t unit, GLenum access) override;
	};
}

