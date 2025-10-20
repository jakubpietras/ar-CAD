#pragma once
#include "ARMAT.h"
#include <vector>
#include <glad/glad.h>
#include "BufferLayout.h"

namespace ar
{
	struct VertexPosition
	{
		mat::Vec3 Position;
		static const BufferLayout s_Layout;
	};

	struct VertexPositionNormal
	{
		mat::Vec3 Position;
		mat::Vec3 Normal;
		static const BufferLayout s_Layout;
	};

	struct VertexPositionID
	{
		mat::Vec3 Position;
		uint32_t ID;
		static const BufferLayout s_Layout;
	};

	struct VertexPositionUVID
	{
		mat::Vec3 Position;
		mat::Vec2 UV;
		uint32_t ID;
		static const BufferLayout s_Layout;
	};

	struct VertexPosNormTex
	{
		mat::Vec3 Position;
		mat::Vec3 Normal;
		mat::Vec2 TexCoord;
		static const BufferLayout s_Layout;
	};

	struct VertexPositionColor
	{
		mat::Vec3 Position;
		mat::Vec3 Color;
		static const BufferLayout s_Layout;
	};

	struct VertexPositionIDColor
	{
		mat::Vec3 Position;
		uint32_t ID;
		mat::Vec3 Color;
		static const BufferLayout s_Layout;
	};

	struct InstancedFloat3
	{
		mat::Vec3 Offset;
		static const BufferLayout s_Layout;
	};

	struct InstancedMat4
	{
		mat::Mat4 Matrix;
		static const BufferLayout s_Layout;
	};


}