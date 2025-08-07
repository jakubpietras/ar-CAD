#pragma once
#include <string>
#include "core/Renderer/VertexArray.h"
#include "core/Renderer/Shader.h"
#include "core/Renderer/Primitive.h"
#include "core/Utils/TorusUtils.h"


namespace ar
{
	// forward declarations
	class Entity;

#pragma region Tags

	struct PointTagComponent {};		// Entities are Points (will render last, etc.)
	struct SelectedTagComponent {};		// Entities are selected
	struct VirtualTagComponent {};		// Entities won't show up in the scene hierarchy

#pragma endregion
#pragma region General

	struct IDComponent
	{
		uint32_t ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag) {
		}
	};

	struct TransformComponent
	{
		mat::Vec3	PivotPoint = { 0.0f, 0.0f, 0.0f };

		mat::Quat	Rotation;
		mat::Vec3	Translation = { 0.0f, 0.0f, 0.0f },
					PreviousTranslation = { 0.0f, 0.0f, 0.0f };
		mat::Vec3	AnglesRPY = { 0.0f, 0.0f, 0.0f }, 
					PreviousAnglesRPY = { 0.0f, 0.0f, 0.0f };
		mat::Vec3	Scale = { 1.0f, 1.0f, 1.0f }, 
					PreviousScale = { 1.0f, 1.0f, 1.0f };
		mat::Mat4	ModelMatrix = mat::Identity();


		bool		IsRotationEnabled = true;
		bool		IsScaleEnabled = true;
		bool		DirtyFlag = true;
	};

	struct MeshComponent
	{
		std::shared_ptr<VertexArray>	VertexArray = nullptr;
		std::shared_ptr<Shader>			Shader = nullptr;
		mat::Vec3						Color = {1.0f, 1.0f, 1.0f};
		Primitive						RenderPrimitive = Primitive::Triangle;
	};

	struct ControlPointsComponent
	{
		ControlPointsComponent();
		ControlPointsComponent(std::vector<Entity> initialPoints);
		std::vector<Entity> Points;
	};

#pragma endregion
#pragma region Geometry

	struct TorusComponent
	{
		TorusDesc							Description;
		std::vector<VertexPosition>			Vertices {};
		std::vector<std::vector<uint32_t>>	Edges {};
		bool								DirtyFlag = true;
	};

#pragma endregion
}