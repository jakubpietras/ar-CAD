#include "arpch.h"
#include "BufferLayout.h"
#include "Attribute.h"

namespace ar
{
	Attribute::Attribute(const std::string& name, AttributeType type, uint32_t divisor)
		: Name(name), Type(type), Divisor(divisor)
	{
		Size = GetSize();
	}

	BufferLayout::BufferLayout()
		: m_Stride(0)
	{ }

	BufferLayout::BufferLayout(std::initializer_list<Attribute> attribs)
		: m_Layout(attribs)
	{
		// initialize strides and offsets
		uint32_t offset = 0;
		for (auto& attribute : m_Layout)
		{
			attribute.Offset = offset;
			m_Stride += attribute.GetSize();
			offset += attribute.GetSize();
		}
	}
}
