#pragma once
#include "glad/glad.h"
#include <string>
#include "Log.h"

namespace ar
{
	GLenum CheckGLErrors();
	void CheckGLFramebufferErrors(uint32_t fbo);

}
