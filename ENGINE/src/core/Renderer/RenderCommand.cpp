#include "arpch.h"
#include "RenderCommand.h"
#include "platform/OpenGL/OGLRendererAPI.h"

namespace ar
{
	ar::RendererAPI* RenderCommand::s_RendererAPI = new ar::OGLRendererAPI();
}