#include "arpch.h"
#include "VertexTypes.h"
#include "BufferLayout.h"
#include "Attribute.h"


namespace ar
{
	const BufferLayout VertexPosition::s_Layout{ 
		{"a_Position", AttributeType::Float3, 0}
	};

	const BufferLayout VertexPositionID::s_Layout{
		{"a_Position", AttributeType::Float3, 0},
		{"a_ID", AttributeType::UInt, 0}
	};

	const BufferLayout VertexPositionUVID::s_Layout{
		{"a_Position", AttributeType::Float3, 0},
		{"a_UV", AttributeType::Float2, 0},
		{"a_ID", AttributeType::UInt, 0}
	};
	
	const BufferLayout VertexPositionColor::s_Layout{ 
		{"a_Position", AttributeType::Float3, 0},
		{"a_Color", AttributeType::Float3, 0} 
	};

	const BufferLayout VertexPositionIDColor::s_Layout{
		{"a_Position", AttributeType::Float3, 0},
		{"a_ID", AttributeType::UInt, 0},
		{"a_Color", AttributeType::Float3, 0}
	};

	const BufferLayout InstancedFloat3::s_Layout{
		{"a_Offset", AttributeType::Float3, 1}
	};

	const BufferLayout InstancedMat4::s_Layout{
		{"a_Matrix1", AttributeType::Float4, 1},
		{"a_Matrix2", AttributeType::Float4, 1},
		{"a_Matrix3", AttributeType::Float4, 1},
		{"a_Matrix4", AttributeType::Float4, 1}
	};
}