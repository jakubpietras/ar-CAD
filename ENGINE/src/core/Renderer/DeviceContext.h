#pragma once

#include "GLFW/glfw3.h"

namespace ar
{
	class DeviceContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};
}