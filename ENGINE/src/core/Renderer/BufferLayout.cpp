#include "arpch.h"
#include "BufferLayout.h"
#include "Attribute.h"

namespace ar
{
	Attribute::Attribute(const std::string& name, AttributeType type)
		: Name(name), Type(type)
	{
		Size = GetSize();
	}

	BufferLayout::BufferLayout()
		: m_Layout(), m_Stride(0)
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
