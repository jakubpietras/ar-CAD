//#pragma once
//
//namespace ar
//{
//	struct FramebufferDesc
//	{
//		uint32_t Width, Height;
//		uint32_t Samples = 1;
//	};
//
//	enum class DepthAttachmentFormat
//	{
//		// todo: generalize to other APIs
//		AR_D24_S8 = GL_DEPTH24_STENCIL8
//	};
//
//	class Framebuffer
//	{
//	public:
//		virtual ~Framebuffer() {}
//		static Framebuffer* Create(const FramebufferDesc& desc);
//
//		virtual void Invalidate() = 0;
//		virtual void AddColorAttachment() = 0;
//		
//	};
//}
//
