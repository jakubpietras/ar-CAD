#pragma once
#include "glad/glad.h"
#include <string>
#include "Log.h"

namespace ar
{
	[[nodiscard]] std::string CheckGLErrors(const char* file = __FILE__, int line = __LINE__);
	[[nodiscard]] std::string CheckGLFramebufferErrors(uint32_t fbo);

}
