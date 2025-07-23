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
		TextureDesc texDesc = {
		m_Description.Width,				// Width
		m_Description.Height,				// Height
		TextureFormat::RGBA8				// Format
		};
		m_ColorAttachment = std::unique_ptr<Texture>(Texture::Create(texDesc));
		texDesc.Format = TextureFormat::D24S8;
		m_DepthAttachment = std::unique_ptr<Texture>(Texture::Create(texDesc));

		glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0, m_ColorAttachment->GetID(), 0);
		glNamedFramebufferTexture(m_ID, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetID(), 0);
		CheckGLFramebufferErrors(m_ID);
	}

	void OGLFramebuffer::Resize(uint32_t newWidth, uint32_t newHeight)
	{
		m_Description.Width = newWidth;
		m_Description.Height = newHeight;
		Invalidate();
	}

}