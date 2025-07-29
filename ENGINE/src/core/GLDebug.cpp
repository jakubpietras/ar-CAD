#include "arpch.h"
#include "GLDebug.h"

namespace ar
{
	std::string CheckGLErrors()
	{
		std::string errorString, error;
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			switch (errorCode)
			{
				case GL_INVALID_ENUM:                  error = "enumeration parameter not legal."; break;
				case GL_INVALID_VALUE:                 error = "value parameter not legal."; break;
				case GL_INVALID_OPERATION:             error = "command not legal for its given parameters."; break;
				case GL_STACK_OVERFLOW:                error = "operation caused a stack overflow."; break;
				case GL_STACK_UNDERFLOW:               error = "stack popping operation occurred with an empty stack."; break;
				case GL_OUT_OF_MEMORY:                 error = "memory allocation operation cannot allocate enough memory."; break;
				case GL_INVALID_FRAMEBUFFER_OPERATION: error = "reading or writing to an incomplete framebuffer."; break;
			}
			if (!error.empty())
			{
				errorString += (error + "\n");
				error = "";
			}
		}
		return errorString;
	}

	std::string CheckGLFramebufferErrors(uint32_t fbo)
	{
		GLenum status =  glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER);
		std::string error;
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			switch (status)
			{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
			case GL_FRAMEBUFFER_UNSUPPORTED:
				return "GL_FRAMEBUFFER_UNSUPPORTED";
			default:
				return "Unknown framebuffer error";
			}
		}
		return "";
	}

}