#include "arpch.h"
#include "GLDebug.h"

namespace ar
{
	std::string CheckGLErrors(const char* file, int line)
	{
		std::string errorString;
		GLenum errorCode;
		bool hasError = false;
		int errorCount = 0;

		while ((errorCode = glGetError()) != GL_NO_ERROR)
		{
			hasError = true;
			errorCount++;
			std::string error;

			switch (errorCode)
			{
			case GL_INVALID_ENUM:
				error = "GL_INVALID_ENUM (0x0500): An unacceptable value is specified for an enumerated argument";
				break;
			case GL_INVALID_VALUE:
				error = "GL_INVALID_VALUE (0x0501): A numeric argument is out of range";
				break;
			case GL_INVALID_OPERATION:
				error = "GL_INVALID_OPERATION (0x0502): The specified operation is not allowed in the current state";
				break;
			case GL_STACK_OVERFLOW:
				error = "GL_STACK_OVERFLOW (0x0503): This command would cause a stack overflow";
				break;
			case GL_STACK_UNDERFLOW:
				error = "GL_STACK_UNDERFLOW (0x0504): This command would cause a stack underflow";
				break;
			case GL_OUT_OF_MEMORY:
				error = "GL_OUT_OF_MEMORY (0x0505): There is not enough memory left to execute the command";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				error = "GL_INVALID_FRAMEBUFFER_OPERATION (0x0506): The framebuffer object is not complete";
				break;
			default:
				error = fmt::format("UNKNOWN_GL_ERROR (0x{:04X}): Unknown OpenGL error code", errorCode);
				break;
			}

			if (!errorString.empty()) {
				errorString += "\n";
			}

			// Add error number if multiple errors
			if (errorCount > 1) {
				errorString += fmt::format("[Error #{}] ", errorCount);
			}

			errorString += error;
		}

		if (hasError) {
			// Extract just the filename from the full path
			std::string filename = file;
			size_t lastSlash = filename.find_last_of("/\\");
			if (lastSlash != std::string::npos) {
				filename = filename.substr(lastSlash + 1);
			}

			errorString += fmt::format("\n  Location: {}:{}", filename, line);

			if (errorCount > 1) {
				errorString += fmt::format("\n  Total errors: {}", errorCount);
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