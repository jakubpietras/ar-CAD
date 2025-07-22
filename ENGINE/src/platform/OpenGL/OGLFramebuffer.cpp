#include "arpch.h"
//#include "OGLFramebuffer.h"
//
//namespace ar
//{
//
//	OGLFramebuffer::OGLFramebuffer(const FramebufferDesc& desc)
//	{
//		glCreateFramebuffers(1, &m_ID);
//		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthTextureID);
//	}
//
//	OGLFramebuffer::~OGLFramebuffer()
//	{
//		glDeleteTextures(1, &m_TextureID);
//		glDeleteTextures(1, &m_DepthTextureID);
//		glDeleteFramebuffers(1, &m_ID);
//	}
//
//	void OGLFramebuffer::Invalidate()
//	{
//		glCreateFramebuffers(1, &m_ID);
//	}
//
//	void OGLFramebuffer::CreateTexture(uint32_t& id)
//	{
//		glCreateTextures(GL_TEXTURE_2D, 1, &id);
//
//	}
//
//}