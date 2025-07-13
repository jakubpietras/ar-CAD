#pragma once
#include <glad/glad.h>

namespace ar
{
	enum class AttributeTypes
	{
		AR_FLOAT = GL_FLOAT,
		AR_UINT  = GL_UNSIGNED_INT,
		AR_INT   = GL_INT
	};

	struct Attribute
	{
		unsigned int Size;
		AttributeTypes Type;
	};
}