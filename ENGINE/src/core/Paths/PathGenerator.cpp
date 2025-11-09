#include "arpch.h"
#include "PathGenerator.h"
#include <numbers>

namespace n = std::numbers;

namespace ar
{
	ar::ToolPath PathGenerator::GenerateFaceMill(MillingConfig config, std::vector<ar::Entity> objects)
	{
		ToolPath path(config.StartPoint, config.Type);
		// below -- without the base height! (base height is added inside ToolPath)
		const float upperHeight = 1.85f;
		const float lowerHeight = 0.3f;

		const ar::mat::Vec3
			forward = { 0.0f, 0.0f, -1.0f },
			down = { 0.0f, -1.0f, 0.0f },
			up = { 0.0f, 1.0f, 0.0f };
		const float limit = 8.7916f;

		// 1. move the tool away from the center and down
		path.MoveTo({ -limit, limit, config.StartPoint.z });
		path.MoveBy(forward * (config.StartPoint.z - upperHeight));

		// 2. upper path
		HeightmapGenerator::HeightmapDesc desc;
		desc.MinHeight = upperHeight;
		auto hmap = HeightmapGenerator::Generate(desc, objects);

		ar::mat::Vec3 dirX = { 1.0f, 0.0f, 0.0f }, dirY = { 0.0f, -1.0f, 0.0f };
		bool rightMovement = true;
		while (path.GetCurrentPos().y > -limit)
		{
			if (rightMovement)
				AddHorizontalPathRight(path, config, hmap, desc);
			else
				AddHorizontalPathLeft(path, config, hmap, desc);
			path.MoveBy(down * config.StepY);
			rightMovement = !rightMovement;
		}
		AddHorizontalPathRight(path, config, hmap, desc);

		// 2.5 Move down to lower height
		path.MoveBy(forward * (upperHeight - lowerHeight));

		// 3. lower path
		desc.MinHeight = lowerHeight;
		hmap = HeightmapGenerator::Generate(desc, objects);
		rightMovement = false;
		while (path.GetCurrentPos().y < limit)
		{
			if (rightMovement)
				AddHorizontalPathRight(path, config, hmap, desc);
			else
				AddHorizontalPathLeft(path, config, hmap, desc);
			path.MoveBy(up * config.StepY);
			rightMovement = !rightMovement;
		}
		AddHorizontalPathLeft(path, config, hmap, desc);
		path.MoveTo({ -limit, limit, config.StartPoint.z });
		path.MoveTo(config.StartPoint);

		return path;
	}

	void PathGenerator::AddHorizontalPathRight(ToolPath& path, MillingConfig config,
		const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc)
	{
		float toolRadius = 0.8f;
		const float limit = 8.7916f;

		while (path.GetCurrentPos().x < limit)
		{
			ar::mat::Vec3 nextPos = path.GetCurrentPos() + ar::mat::Vec3{1.0f, 0.0f, 0.0f} * config.StepX;
			
			// find position of the tool center
			ar::mat::Vec3d center = { nextPos.x, nextPos.y, 0.0f };
			auto mapped = HeightmapGenerator::MapPoint(desc, nextPos);
			if (mapped.x == -1 || mapped.y == -1) // center outside of the milling material
				center.z = desc.MinHeight + toolRadius;
			else
			{
				auto hmHeight = hmap[mapped.y * desc.SamplesX + mapped.x];
				center.z = hmHeight + toolRadius;
			}
			nextPos.z = center.z - toolRadius;

			// moving the tool up if gouging
			float maxHeight = nextPos.z;
			const int samples = 10;
			const float stepV = 2 * n::pi / samples;
			const float stepU = n::pi / 2 / samples;
			for (int ii = 0; ii < samples; ii++)
			{
				for (int jj = 0; jj < samples; jj++)
				{
					float u = (n::pi / 2) + ii * stepU;
					float v = jj * stepV;
					ar::mat::Vec3d point = center + ar::mat::Vec3d{ sin(u) * cos(v) * toolRadius, sin(u) * sin(v) * toolRadius, cos(u) * toolRadius };
					mapped = HeightmapGenerator::MapPoint(desc, point);
					if (mapped.x != -1 && mapped.y != -1)
					{
						auto height = hmap[mapped.y * desc.SamplesX + mapped.x];
						if (height > maxHeight)
							maxHeight = height;
					}
				}
			}
			nextPos.z = maxHeight;
			path.MoveTo(nextPos);
		}
	}

	void PathGenerator::AddHorizontalPathLeft(ToolPath& path, MillingConfig config,
		const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc)
	{
		float toolRadius = 0.8f;
		const float limit = 8.7916f;

		while (path.GetCurrentPos().x > -limit)
		{
			ar::mat::Vec3 nextPos = path.GetCurrentPos() + ar::mat::Vec3{ -1.0f, 0.0f, 0.0f } *config.StepX;

			// find position of the tool center
			ar::mat::Vec3d center = { nextPos.x, nextPos.y, 0.0f };
			auto mapped = HeightmapGenerator::MapPoint(desc, nextPos);
			if (mapped.x == -1 || mapped.y == -1) // center outside of the milling material
				center.z = desc.MinHeight + toolRadius;
			else
			{
				auto hmHeight = hmap[mapped.y * desc.SamplesX + mapped.x];
				center.z = hmHeight + toolRadius;
			}
			nextPos.z = center.z - toolRadius;

			// moving the tool up if gouging
			float maxHeight = nextPos.z;
			const int samples = 10;
			const float stepV = 2 * n::pi / samples;
			const float stepU = n::pi / 2 / samples;
			for (int ii = 0; ii < samples; ii++)
			{
				for (int jj = 0; jj < samples; jj++)
				{
					float u = (n::pi / 2) + ii * stepU;
					float v = jj * stepV;
					ar::mat::Vec3d point = center + ar::mat::Vec3d{ sin(u) * cos(v) * toolRadius, sin(u) * sin(v) * toolRadius, cos(u) * toolRadius };
					mapped = HeightmapGenerator::MapPoint(desc, point);
					if (mapped.x != -1 && mapped.y != -1)
					{
						auto height = hmap[mapped.y * desc.SamplesX + mapped.x];
						if (height > maxHeight)
							maxHeight = height;
					}
				}
			}
			nextPos.z = maxHeight;
			path.MoveTo(nextPos);
		}
	}

}