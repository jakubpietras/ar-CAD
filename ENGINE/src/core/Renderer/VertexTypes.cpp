#include "arpch.h"
#include "VertexTypes.h"
#include "BufferLayout.h"
#include "Attribute.h"


namespace ar
{
	const BufferLayout VertexPosition::s_Layout{ 
		{"a_Position", AttributeType::Int, 0} 
	};
	
	const BufferLayout VertexPositionColor::s_Layout{ 
		{"a_Position", AttributeType::Float3, 0},
		{"a_Color", AttributeType::Float3, 0} 
	};

	const BufferLayout InstancedOffsets::s_Layout{
		{"a_Offset", AttributeType::Float3, 1}
	};
}