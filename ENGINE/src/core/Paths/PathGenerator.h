#pragma once
#include "ToolPath.h"
#include "HeightmapGenerator.h"

namespace ar
{
	class PathGenerator
	{
	public:
		enum class MillingOp
		{
			FACE,
			OUTLINE,
			FINAL
		};
		struct MillingConfig
		{
			MillingOp Operation = MillingOp::FACE;
			ar::mat::Vec3 StartPoint = {0.0f, 0.0f, 5.1f};
			float StepY = 1.0f;
			float StepX = 0.117f;
			ToolType Type = ToolType::K16;
		};
		static ToolPath GenerateFaceMill(MillingConfig config, std::vector<ar::Entity> objects);
		static ToolPath GenerateBaseMill(MillingConfig config, std::vector<ar::Entity> objects);
		static ToolPath GenerateOutlineMill(MillingConfig config, ar::Entity outline, ar::Entity startPoint, ar::mat::Vec3 offsetDir);

	private:
		static const float m_BaseMargin;
		static bool CheckCollision(const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc, ar::mat::Vec3 center, float toolRadius);


		static void AddFaceMillHorizontalPathRight(ToolPath& path, MillingConfig config,
			const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc);
		static void AddFaceMillHorizontalPathLeft(ToolPath& path, MillingConfig config,
			const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc);
		static bool AddBaseMillPathRight(ToolPath& path, MillingConfig config,
			const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc, float stopX);
		static bool AddBaseMillPathLeft(ToolPath& path, MillingConfig config,
			const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc, float stopX);
		static bool AddBaseMillPathVertical(ToolPath& path, MillingConfig config,
			const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc, bool goesUp);
	};
}