#include "arpch.h"
#include "PathGenerator.h"
#include <numbers>

namespace n = std::numbers;

namespace ar
{
	const float PathGenerator::m_BaseMargin = 0.1f;
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
				AddFaceMillHorizontalPathRight(path, config, hmap, desc);
			else
				AddFaceMillHorizontalPathLeft(path, config, hmap, desc);
			path.MoveBy(down * config.StepY);
			rightMovement = !rightMovement;
		}
		AddFaceMillHorizontalPathRight(path, config, hmap, desc);

		// 2.5 Move down to lower height
		path.MoveBy(forward * (upperHeight - lowerHeight));

		// 3. lower path
		desc.MinHeight = lowerHeight;
		hmap = HeightmapGenerator::Generate(desc, objects);
		rightMovement = false;
		while (path.GetCurrentPos().y < limit)
		{
			if (rightMovement)
				AddFaceMillHorizontalPathRight(path, config, hmap, desc);
			else
				AddFaceMillHorizontalPathLeft(path, config, hmap, desc);
			path.MoveBy(up * config.StepY);
			rightMovement = !rightMovement;
		}
		AddFaceMillHorizontalPathLeft(path, config, hmap, desc);

		// 4. return the tool to original position
		path.MoveTo({ -limit, limit, config.StartPoint.z });
		path.MoveTo(config.StartPoint);

		return path;
	}

	ar::ToolPath PathGenerator::GenerateBaseMill(MillingConfig config, std::vector<ar::Entity> objects)
	{
		ToolPath path(config.StartPoint, config.Type);
		const float limit = 8.2f;

		// 1. move the tool away from the center and down
		path.MoveTo({ -limit, 7.5f, config.StartPoint.z });
		path.MoveBy(ar::mat::Vec3{ 0.0f, 0.0f, -1.0f } * (config.StartPoint.z - m_BaseMargin));

		// 2. heightmap generation
		HeightmapGenerator::HeightmapDesc desc;
		desc.MinHeight = 0.0f;
		auto hmap = HeightmapGenerator::Generate(desc, objects);

		// 3. mill left half of the base
		bool rightMovement = true;
		while (path.GetCurrentPos().y > -7.5f)
		{
			if (rightMovement)
			{
				if (!AddBaseMillPathRight(path, config, hmap, desc, 3.5f))
				{
					// early return
				}
				rightMovement = !rightMovement;
			}
			else
			{
				if (!AddBaseMillPathLeft(path, config, hmap, desc, -limit))
				{
					// early return, cannot go left
					AR_ERROR("PATHS STUCK!");
				}
				rightMovement = !rightMovement;
			}
			
			if (!AddBaseMillPathVertical(path, config, hmap, desc, false))
			{
				// cannot go down
			}
			
		}
		// 4. go to the right half
		path.MoveBy({ 2 * 8.2f, 0.0f, 0.0f });
		AddBaseMillPathVertical(path, config, hmap, desc, true);

		// 5. mill right half of the base
		rightMovement = false;
		while (path.GetCurrentPos().y < 7.5f)
		{
			if (!rightMovement)
			{
				if (!AddBaseMillPathLeft(path, config, hmap, desc, -3.5f))
				{
					// early return
				}
				rightMovement = !rightMovement;
			}
			else
			{
				if (!AddBaseMillPathRight(path, config, hmap, desc, limit))
				{
					// early return, cannot go right
					AR_ERROR("PATHS STUCK!");
				}
				rightMovement = !rightMovement;
			}

			if (!AddBaseMillPathVertical(path, config, hmap, desc, true))
			{
				// cannot go up
			}
		}

		// 6. return the tool to original position
		path.MoveTo({ -limit, 7.5f, m_BaseMargin });
		path.MoveBy(ar::mat::Vec3{ 0.0f, 0.0f, 1.0f } * config.StartPoint.z);
		path.MoveTo(config.StartPoint);
		return path;
	}

	bool PathGenerator::CheckCollision(const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc, ar::mat::Vec3 center, float toolRadius)
	{
		ar::mat::Vec2T<int> mapped;
		const int samples = 10;
		const float stepAlpha = 2 * n::pi / samples;
		const float stepR = toolRadius / samples;
		for (int ii = 0; ii < samples; ii++)
		{
			for (int jj = 0; jj < samples; jj++)
			{
				float alpha = ii * stepAlpha;
				float radius = jj * stepR;
				ar::mat::Vec3 point =
					center +
					ar::mat::Vec3{
						radius * cos(alpha),
						radius * sin(alpha),
						0.0f
				};
				mapped = HeightmapGenerator::MapPoint(desc, point);
				if (mapped.x != -1 && mapped.y != -1)
				{
					auto height = hmap[mapped.y * desc.SamplesX + mapped.x];
					if (height > 0.0f)
						return true;
				}
			}
		}
		return false;
	}

	void PathGenerator::AddFaceMillHorizontalPathRight(ToolPath& path, MillingConfig config,
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

	void PathGenerator::AddFaceMillHorizontalPathLeft(ToolPath& path, MillingConfig config,
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

	bool PathGenerator::AddBaseMillPathRight(ToolPath& path, MillingConfig config, const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc, float stopX)
	{
		float toolRadius = 0.5f;

		while (path.GetCurrentPos().x < stopX)
		{
			ar::mat::Vec3 nextPos = path.GetCurrentPos() + ar::mat::Vec3{ 1.0f, 0.0f, 0.0f } * config.StepX;
			
			// if the function didn't return early, it's safe to move
			if (!CheckCollision(hmap, desc, nextPos, 0.5f))
			{
				nextPos.z = 0.0f + m_BaseMargin;
				path.MoveTo(nextPos);
			}
			else
				return false;
		}
		return true;
	}

	bool PathGenerator::AddBaseMillPathLeft(ToolPath& path, MillingConfig config, const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc, float stopX)
	{
		float toolRadius = 0.5f;

		while (path.GetCurrentPos().x > stopX)
		{
			ar::mat::Vec3 nextPos = path.GetCurrentPos() + ar::mat::Vec3{ -1.0f, 0.0f, 0.0f } * config.StepX;
			
			// if the function didn't return early, it's safe to move
			if (!CheckCollision(hmap, desc, nextPos, 0.5f))
			{
				nextPos.z = 0.0f + m_BaseMargin;
				path.MoveTo(nextPos);
			}
			else
				return false;
		}
		return true;
	}

	bool PathGenerator::AddBaseMillPathVertical(ToolPath& path, MillingConfig config, const std::vector<float>& hmap, HeightmapGenerator::HeightmapDesc desc, bool goesUp)
	{
		float toolRadius = 0.5f;
		ar::mat::Vec3 moveDir = (goesUp) ? 
			ar::mat::Vec3{0.0f, 1.0f, 0.0f} : ar::mat::Vec3{0.0f, -1.0f, 0.0f};
		
		// next planned move
		ar::mat::Vec3 nextPos = path.GetCurrentPos() + moveDir * config.StepY;
		
		// if the function didn't return early, it's safe to move
		if (!CheckCollision(hmap, desc, nextPos, 0.5f))
		{
			nextPos.z = 0.0f + m_BaseMargin;
			path.MoveTo(nextPos);
			return true;
		}
		return false;
	}
}