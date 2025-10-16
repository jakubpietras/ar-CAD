#include "arpch.h"
#include "OGLTexture.h"


namespace ar
{

	OGLTexture::OGLTexture(const TextureDesc& desc)
		: m_Description(desc)
	{
		if (m_Description.Format == TextureFormat::D24S8)
		{
			glCreateRenderbuffers(1, &m_ID);
			glNamedRenderbufferStorage(m_ID, GL_DEPTH24_STENCIL8, m_Description.Width, m_Description.Height);
			AR_GL_CHECK();
		}
		else
		{
			glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
			glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTextureStorage2D(m_ID, 1, GetGLInternalFormat(m_Description.Format),
				m_Description.Width, m_Description.Height);
			AR_GL_CHECK();
		}
	}

	OGLTexture::OGLTexture(const std::string& filepath)
	{
		stbi_set_flip_vertically_on_load(true);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		AR_GL_CHECK();

		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		AR_GL_CHECK();

		int width = 0, height = 0, channels = 0;
		unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		if (!data) {
			std::cerr << "STBI load failed for " << filepath
				<< " reason: " << stbi_failure_reason() << std::endl;

			throw std::runtime_error("Failed to load texture: " + filepath);
		}
		GLenum format = GL_RGBA;
		GLenum internalFormat = GL_RGBA8; // or GL_RGB8, GL_R8 depending on channels

		if (channels == 1) {
			format = GL_RED;
			internalFormat = GL_R8;
		}
		else if (channels == 3) {
			format = GL_RGB;
			internalFormat = GL_RGB8;
		}
		else if (channels == 4) {
			format = GL_RGBA;
			internalFormat = GL_RGBA8;
		}

		glTextureStorage2D(m_ID, 1, internalFormat, width, height);
		AR_GL_CHECK();

		glTextureSubImage2D(m_ID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
		AR_GL_CHECK();

		glGenerateTextureMipmap(m_ID);
		AR_GL_CHECK();
		m_Description = { (uint32_t)width, (uint32_t)height, TextureFormat::RGBA8 };
		stbi_image_free(data);
	}

	OGLTexture::~OGLTexture()
	{
		if (m_Description.Format == TextureFormat::D24S8)
			glDeleteRenderbuffers(1, &m_ID);
		else
			glDeleteTextures(1, &m_ID);
		AR_GL_CHECK();
	}

	void OGLTexture::SetData(void* data, uint32_t size)
	{
		if (m_Description.Format == TextureFormat::D24S8)
		{
			AR_ASSERT(false, "Not implemented yet, should not be used");
		}
		else
		{
			GLenum format = GetDataFormat(m_Description.Format);
			GLenum type = GL_UNSIGNED_BYTE;
			if (m_Description.Format == TextureFormat::R32)
				type = GL_UNSIGNED_INT;
			if (m_Description.Format == TextureFormat::R32F)
				type = GL_FLOAT;

			glTextureSubImage2D(m_ID, 0, 0, 0, m_Description.Width, m_Description.Height,
				format, type, data);
			AR_GL_CHECK();
		}
	}

	GLenum OGLTexture::GetDataFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8: return GL_RED_INTEGER;
		case TextureFormat::R32: return GL_RED_INTEGER;
		case TextureFormat::R32F: return GL_RED;
		case TextureFormat::RGBA8: return GL_RGBA;
		case TextureFormat::D24S8:
		default:
			AR_ASSERT(false, "Incorrect TextureFormat");
			return 0;
		}
	}

	GLenum OGLTexture::GetGLInternalFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8: return GL_R8;
		case TextureFormat::RGBA8: return GL_RGBA8;
		case TextureFormat::R32: return GL_R32UI;
		case TextureFormat::R32F: return GL_R32F;
		case TextureFormat::D24S8:
		default:
			AR_ASSERT(false, "Incorrect TextureFormat");
			return 0;
		}
	}

	const void OGLTexture::BindImageUnit(uint32_t unit, GLenum access)
	{
		glBindImageTexture(unit, m_ID, 0, GL_FALSE, 0, access, GetGLInternalFormat(m_Description.Format));
	}

}