#include "arpch.h"
#include "VertexTypes.h"
#include "BufferLayout.h"
#include "Attribute.h"


namespace ar
{
	const BufferLayout VertexPosition::s_Layout{ 
		{"a_Position", AttributeType::Int} 
	};
	
	const BufferLayout VertexPositionColor::s_Layout{ 
		{"a_Position", AttributeType::Float3},
		{"a_Color", AttributeType::Float3} 
	};
}