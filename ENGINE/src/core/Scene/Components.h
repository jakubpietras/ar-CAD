#pragma once
#include <string>
#include "core/Renderer/VertexArray.h"
#include "core/Renderer/Shader.h"
#include "core/Renderer/Primitive.h"
#include "core/Utils/TorusUtils.h"
#include "core/Utils/SurfaceUtils.h"
#include "core/Geometry/Hole.h"
#include "core/Drawing/PaintSurface.h"

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
	struct HiddenTagComponent{};
	struct SurfacePointTagComponent{};

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
		TransformComponent(mat::Vec3 translation = { 0.0f, 0.0f, 0.0f },
			mat::Quat rotation = {}, mat::Vec3 scale = { 1.0f, 1.0f, 1.0f })
			: Translation(translation), Rotation(rotation)
		{
			// todo: convert quaternion to RPY
		}

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
		float							PrimitiveSize = 1.f;

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

	struct ControlMeshComponent
	{
		std::shared_ptr<VertexArray>	VertexArray = nullptr;
		std::shared_ptr<Shader>			Shader = nullptr;
		mat::UInt2						Size = { 1, 1 };
		bool							DirtyFlag = false,
										IsVisible = true;
	};

	struct ControlPointsComponent
	{
		ControlPointsComponent();
		ControlPointsComponent(std::vector<Entity> initialPoints);
		std::vector<Entity> Points{};
		std::vector<uint32_t> Indices{};

	};

	struct TrimmingComponent
	{
		enum class TrimSide
		{
			SIDE_A,
			SIDE_B
		};

		bool ShouldTrimSurface = false;
		TrimSide Side = TrimSide::SIDE_A;
		ar::Ref<PaintSurface> TrimTexture = nullptr;
		std::vector<ar::Entity> IntersectionCurves{};
	};

#pragma endregion
#pragma region Geometry

	struct PointComponent 
	{
		mat::Vec3 Color{ 1.f, 1.f, 1.f };
		mat::Vec3 TempColor{ 1.f, 1.f, 1.f };
		bool ShouldUseTempColor = false;
		std::vector<Entity> Parents{};
	};

	struct TorusComponent
	{
		TorusDesc							Description;
		std::vector<VertexPositionUVID>		Vertices {};
		std::vector<std::vector<uint32_t>>	Edges {};
		bool								DirtyFlag = true;
	};
	
	struct ChainComponent {};

	struct CurveC0Component
	{
		bool	ShowPolygon = false;
	};

	struct CurveC2Component
	{
		bool	ShowPolygon = false,
				DirtyFlag = false;	 // true == Bezier points changed, need to regenerate de Boor + Bezier
	};

	struct InterpolatedC2Component { };

	struct SurfaceComponent
	{
		SurfaceDesc										Description;
		std::optional<std::vector<ar::mat::Vec3>>		AuxPoints = std::nullopt;	// for Bezier points in C2 surfaces
		bool											ShowNet = false;
	};

	struct GregoryPatchComponent
	{
		mat::UInt2	Samples{ 4, 4 };
		ar::Hole	HoleToFill;
		bool		ShowNet = false;
		
	};

	struct IntersectCurveComponent
	{
		std::vector<ar::mat::Vec3> Points;
		std::vector<ar::mat::Vec4> Params;
		std::vector<ar::mat::Vec3> NormalsP, NormalsQ;
		std::vector<ar::mat::Vec3> SurfaceNormalsP, SurfaceNormalsQ;
		ar::Ref<PaintSurface> ImageP, ImageQ;
		ar::Ref<PaintSurface> TrimTexP, TrimTexQ;
		ar::Entity SurfaceP;
		std::optional<ar::Entity> SurfaceQ;
		bool ShowImageP = false, ShowImageQ = false;
		bool ShowImage = false, ConvertToSpline = false;
		size_t ConversionPointsCount = 0;

		// ======= Expand/shrink
		bool ShouldResize = false;
		bool ResizeOutside = true;
		bool UseNormalsP = true;
		float ResizeLength = 0.0f;

		bool DirtyFlag = false;
	};

#pragma endregion
}