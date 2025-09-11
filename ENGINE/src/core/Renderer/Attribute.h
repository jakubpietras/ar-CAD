#pragma once

namespace ar
{
	enum class AttributeType
	{
		Float2,
		Float3,
		Float4,
		Int,
		UInt
	};

	struct Attribute
	{
		Attribute(const std::string& name, AttributeType type, uint32_t divisor = 0);

		std::string Name;
		AttributeType Type;
		bool Normalized = false;
		uint32_t Size = 0;
		uint32_t Offset = 0;
		uint32_t Divisor = 0;

		inline uint32_t GetCount()
		{
			switch (Type)
			{
			case AttributeType::Int: return 1;
			case AttributeType::UInt: return 1;
			case AttributeType::Float2: return 2;
			case AttributeType::Float3: return 3;
			case AttributeType::Float4: return 4;
			default:
				return 0;
			}
		}

		inline uint32_t GetSize()
		{
			switch (Type)
			{
			case AttributeType::Int: return 1 * sizeof(int);
			case AttributeType::UInt: return 1 * sizeof(unsigned int);
			case AttributeType::Float2: return 2 * sizeof(float);
			case AttributeType::Float3: return 3 * sizeof(float);
			case AttributeType::Float4: return 4 * sizeof(float);
			default:
				return 0;
			}
		}

		inline GLenum GetOpenGLType()	// TODO: specialize the Attribute for different APIs inst.
		{
			switch (Type)
			{
			case AttributeType::Int: return GL_INT;
			case AttributeType::UInt: return GL_UNSIGNED_INT;
			case AttributeType::Float2: return GL_FLOAT;
			case AttributeType::Float3: return GL_FLOAT;
			case AttributeType::Float4: return GL_FLOAT;
			default:
				return 0; // TODO: fix this
			}
		}
	};
}