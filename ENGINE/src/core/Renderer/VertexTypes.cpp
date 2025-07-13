#include "arpch.h"
#include "VertexTypes.h"

namespace ar
{
	const std::vector<Attribute> VertexPosition::Layout = { {3, AttributeTypes::AR_FLOAT} };
	const std::vector<Attribute> VertexPositionColor::Layout = { {3, AttributeTypes::AR_FLOAT}, {3, AttributeTypes::AR_FLOAT} };
}