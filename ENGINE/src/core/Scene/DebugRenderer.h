#pragma once
#include "core/Renderer/VertexTypes.h"
#include "core/Core.h"
#include "core/Renderer/VertexArray.h"

namespace ar
{
	class DebugRenderer
	{
	public:
		static void Init();
		static void Render(ar::mat::Mat4 viewProjection);
		static void Clear();
		static void AddPoint(mat::Vec3 position, mat::Vec3 color);
		static void AddLine(mat::Vec3 end1, mat::Vec3 end2);

	private:
		static constexpr mat::Vec3 LINE_COLOR = { 1.f, 0.f, 0.f };
		static bool m_ShouldResetBuffers;

		static std::vector<ar::VertexPositionColor> m_Points;
		static std::vector<ar::VertexPositionColor> m_Lines;
		
		static ar::Ref<ar::VertexArray> m_PointsVA, m_LinesVA;

	};
}

