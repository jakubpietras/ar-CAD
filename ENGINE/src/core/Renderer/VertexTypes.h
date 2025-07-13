#pragma once
#include "ARMAT.h"
#include <vector>
#include <glad/glad.h>
#include "Attribute.h"

namespace ar
{
	struct VertexPosition
	{
		Vec3 Position;
		static const std::vector<Attribute> Layout;
	};

	struct VertexPositionColor
	{
		Vec3 position;
		Vec3 color;
		static const std::vector<Attribute> Layout;
	};

}