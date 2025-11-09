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
		
	private:
		static void AddHorizontalPathRight(ToolPath& path, MillingConfig config,
			const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc);
		static void AddHorizontalPathLeft(ToolPath& path, MillingConfig config,
			const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc);


	};
}