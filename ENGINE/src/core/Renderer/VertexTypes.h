#pragma once
#include "ARMAT.h"
#include <vector>
#include <glad/glad.h>
#include "BufferLayout.h"

namespace ar
{
	struct VertexPosition
	{
		mat::Vec3 Position;
		static const BufferLayout s_Layout;
	};

	struct VertexPositionColor
	{
		mat::Vec3 Position;
		mat::Vec3 Color;
		static const BufferLayout s_Layout;
	};

	struct InstancedOffsets
	{
		mat::Vec3 Offset;
		static const BufferLayout s_Layout;
	};

}