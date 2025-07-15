#pragma once
#include <vector>
#include <string>
#include "Attribute.h"

namespace ar
{
	class BufferLayout
	{
	public:
		BufferLayout();
		BufferLayout(std::initializer_list<Attribute> attribs);
		
		inline const uint32_t GetStride() const { return m_Stride; }

		std::vector<Attribute>::iterator begin() { return m_Layout.begin(); }
		std::vector<Attribute>::iterator end() { return m_Layout.end(); }

	private:
		std::vector<Attribute> m_Layout;
		uint32_t m_Stride = 0;
	};
}
