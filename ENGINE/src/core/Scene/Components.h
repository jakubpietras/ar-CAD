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

#pragma region Datatypes

	enum class ShaderType
	{
		MAIN,
		PICKING
	};

	enum class PivotType
	{
		LOCAL_ORIGIN,
		CURSOR
	};

#pragma endregion
#pragma region Tags

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
		PivotType	PivotPoint = PivotType::LOCAL_ORIGIN;

		mat::Quat	Rotation;
		mat::Vec3	Translation = { 0.0f, 0.0f, 0.0f },
					PreviousTranslation = { 0.0f, 0.0f, 0.0f };
		mat::Vec3	AnglesRPY = { 0.0f, 0.0f, 0.0f }, 
					PreviousAnglesRPY = { 0.0f, 0.0f, 0.0f };
		mat::Vec3	Scale = { 1.0f, 1.0f, 1.0f }, 
					PreviousScale = { 1.0f, 1.0f, 1.0f };
		mat::Mat4	ModelMatrix = mat::Identity();

		bool		GroupTransformation = false;
		bool		IsRotationEnabled = true;
		bool		IsScaleEnabled = true;
		bool		DirtyFlag = true;
	};

	struct MeshComponent
	{
		std::shared_ptr<VertexArray>	VertexArray = nullptr;
		std::shared_ptr<Shader>			Shader = nullptr,
										PickingShader = nullptr;
		ShaderType						ShaderUsed = ShaderType::MAIN;
		mat::Vec3						PrimaryColor = {1.0f, 1.0f, 1.0f};
		Primitive						RenderPrimitive = Primitive::Triangle;
		size_t							TessellationPatchSize = 4;
		bool							DirtyFlag = false,
										AdaptiveDrawing = false;

		inline std::shared_ptr<ar::Shader> GetShader() const
		{
			switch (ShaderUsed)
			{
			case ShaderType::MAIN: { return Shader; }
			case ShaderType::PICKING: { return PickingShader; }
			default: return nullptr;
			}
		}

	};

	struct ControlPointsComponent
	{
		ControlPointsComponent();
		ControlPointsComponent(std::vector<Entity> initialPoints);
		std::vector<Entity> Points{};
		std::vector<uint32_t> Indices{};


		std::vector<VertexPositionID> GetVertexData(uint32_t id);
	};

#pragma endregion
#pragma region Geometry

	struct PointComponent 
	{
		mat::Vec3 Color{ 1.f, 1.f, 1.f };
		std::vector<Entity> Parents{};
	};

	struct TorusComponent
	{
		TorusDesc							Description;
		std::vector<VertexPositionID>		Vertices {};
		std::vector<std::vector<uint32_t>>	Edges {};
		bool								DirtyFlag = true;
	};
	
	struct ChainComponent {};

	struct CurveC0Component
	{
		bool ShowPolygon = false;
	};

#pragma endregion
}