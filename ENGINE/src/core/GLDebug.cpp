#include "arpch.h"
#include "GLDebug.h"

namespace ar
{
	GLenum CheckGLErrors()
	{
		GLenum errorCode;
		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			std::string error;
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
			AR_ASSERT(errorCode == GL_NO_ERROR, "OpenGL error");
		}
		return errorCode;
	}
}