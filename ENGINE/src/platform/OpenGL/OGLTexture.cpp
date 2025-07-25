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
			CheckGLErrors();
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
			CheckGLErrors();
		}
	}

	OGLTexture::OGLTexture(const std::string& filepath)
	{
		stbi_set_flip_vertically_on_load(true);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		int width, height, channels;
		auto data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
		AR_ASSERT(data, "Failed to load texture");
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
		glTextureSubImage2D(m_ID, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		CheckGLErrors();
		m_Description = { (uint32_t)width, (uint32_t)height, TextureFormat::RGBA8 };
		stbi_image_free(data);
	}

	OGLTexture::~OGLTexture()
	{
		if (m_Description.Format == TextureFormat::D24S8)
			glDeleteRenderbuffers(1, &m_ID);
		else
			glDeleteTextures(1, &m_ID);
		CheckGLErrors();
	}

	void OGLTexture::SetData(void* data, uint32_t size)
	{
		if (m_Description.Format == TextureFormat::D24S8)
		{
			// todo (not needed for now)
			AR_ASSERT(false, "Not implemented yet, should not be used");
		}
		else
		{
			// texture
			glTextureStorage2D(m_ID, 1, GetGLInternalFormat(m_Description.Format),
				m_Description.Width, m_Description.Height);
			glTextureSubImage2D(m_ID, 0, 0, 0, m_Description.Width, m_Description.Height,
				GetDataFormat(m_Description.Format), GL_UNSIGNED_BYTE, data);
			CheckGLErrors();
		}
	}

	GLenum OGLTexture::GetDataFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8: return GL_RED_INTEGER;
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
		case TextureFormat::D24S8:
		default:
			AR_ASSERT(false, "Incorrect TextureFormat");
			return 0;
		}
	}

}