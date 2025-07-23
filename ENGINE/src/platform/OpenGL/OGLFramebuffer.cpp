#include "arpch.h"
#include "OGLFramebuffer.h"
#include "core/Renderer/Texture.h"

namespace ar
{

	OGLFramebuffer::OGLFramebuffer(const FramebufferDesc& desc)
		: m_Description(desc)
	{
		glCreateFramebuffers(1, &m_ID);
		Invalidate();
	}

	OGLFramebuffer::~OGLFramebuffer()
	{
		glDeleteFramebuffers(1, &m_ID);
	}

	void OGLFramebuffer::Invalidate()
	{
		if (m_ID)
			glDeleteFramebuffers(1, &m_ID);
		glCreateFramebuffers(1, &m_ID);

		TextureDesc texDesc = {
		m_Description.Width,				// Width
		m_Description.Height,				// Height
		TextureFormat::RGBA8				// Format
		};
		m_ColorAttachment = std::unique_ptr<Texture>(Texture::Create(texDesc));
		texDesc.Format = TextureFormat::D24S8;
		m_DepthAttachment = std::unique_ptr<Texture>(Texture::Create(texDesc));

		glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0, m_ColorAttachment->GetID(), 0);
		glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthAttachment->GetID());
		CheckGLFramebufferErrors(m_ID);
	}

	void OGLFramebuffer::Resize(uint32_t newWidth, uint32_t newHeight)
	{
		
		m_Description.Width = newWidth;
		m_Description.Height = newHeight;
		Invalidate();
	}

	void OGLFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);
		glViewport(0, 0, m_Description.Width, m_Description.Height);
	}

	void OGLFramebuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	uint32_t OGLFramebuffer::GetColorAttachment()
	{
		return m_ColorAttachment->GetID();
	}

	uint32_t OGLFramebuffer::GetHeight()
	{
		return m_Description.Height;
	}

	uint32_t OGLFramebuffer::GetWidth()
	{
		return m_Description.Width;
	}

}