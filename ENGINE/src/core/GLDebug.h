#pragma once
#include "glad/glad.h"
#include <string>
#include "Log.h"

namespace ar
{
	[[nodiscard]] std::string CheckGLErrors();
	[[nodiscard]] std::string CheckGLFramebufferErrors(uint32_t fbo);

}
