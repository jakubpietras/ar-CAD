#pragma once
#include "ARMAT.h"
#include <vector>
#include <glad/glad.h>
#include "BufferLayout.h"

namespace ar
{
	struct VertexPosition
	{
		Vec3 Position;
		static const BufferLayout s_Layout;
	};

	struct VertexPositionColor
	{
		Vec3 Position;
		Vec3 Color;
		static const BufferLayout s_Layout;
	};

}