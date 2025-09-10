#include "arpch.h"
#include "SceneFactory.h"
#include "core/Utils/GeneralUtils.h"
#include "core/Utils/CurveUtils.h"
#include "core/Scene/Components.h"
#include "core/Utils/IntersectUtils.h"

namespace ar
{

	SceneFactory::SceneFactory(Ref<Scene> scene)
		: m_Scene(scene) { }

	ar::Entity SceneFactory::CreatePoint(mat::Vec3 spawnPoint, std::optional<uint32_t> id, const std::string& name)
	{
		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);

		entity.AddComponent<ar::PointComponent>();

		// Transform
		auto& trc = entity.AddComponent<ar::TransformComponent>();
		trc.Translation = spawnPoint;
		trc.IsRotationEnabled = false;
		trc.IsScaleEnabled = false;

		return entity;
	}

	ar::Entity SceneFactory::CreateTorus(mat::Vec3 spawnPoint, TorusDesc desc, std::optional<uint32_t> id, const std::string& name)
	{
		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);

		// Torus component
		auto& tc = entity.AddComponent<ar::TorusComponent>();
		tc.Description = desc;
		tc.Vertices = ar::TorusUtils::GenerateTorusVertices(desc, entity.GetID());
		tc.Edges = ar::TorusUtils::GenerateTorusEdges(desc);

		// Transform component
		auto& trc = entity.AddComponent<ar::TransformComponent>();
		trc.Translation = spawnPoint;

		// Mesh (render) component
		auto& mc = entity.AddComponent<ar::MeshComponent>();

		// Primitive
		mc.RenderPrimitive = ar::Primitive::LineLoop;

		// VertexArray
		mc.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		mc.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(tc.Vertices)));
		auto indexBuffers = ar::IndexBuffer::Create(tc.Edges);
		for (auto& ib : indexBuffers)
			mc.VertexArray->AddIndexBuffer(ib);

		// Shader
		mc.Shader = ar::ShaderLib::Get("Basic");
		mc.PickingShader = ar::ShaderLib::Get("Picking");

		return entity;
	}

	ar::Entity SceneFactory::CreateChain(std::vector<Entity> points, std::optional<uint32_t> id, const std::string& name)
	{
		AR_ASSERT(points.size() > 1, "Too few points to create a chain.");
		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);
		entity.AddComponent<ar::ChainComponent>();

		auto& cp = entity.AddComponent<ar::ControlPointsComponent>(points);
		for (auto& point : points)
			point.GetComponent<ar::PointComponent>().Parents.push_back(entity);

		auto& mesh = entity.AddComponent<ar::MeshComponent>();

		mesh.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		mesh.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GeneralUtils::GetVertexData(cp.Points, entity.GetID()))));
		mesh.Shader = ar::ShaderLib::Get("Basic");
		mesh.PickingShader = ar::ShaderLib::Get("Picking");
		mesh.RenderPrimitive = ar::Primitive::LineStrip;
		return entity;

	}

	ar::Entity SceneFactory::CreateCurveC0(std::vector<Entity> points, std::optional<uint32_t> id, const std::string& name)
	{
		AR_ASSERT(points.size() > 1, "Too few points to create a curve.");
		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);
		entity.AddComponent<ar::CurveC0Component>();

		auto& cp = entity.AddComponent<ar::ControlPointsComponent>(points);
		for (auto& point : points)
			point.GetComponent<ar::PointComponent>().Parents.push_back(entity);

		auto& mesh = entity.AddComponent<ar::MeshComponent>();
		mesh.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		mesh.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GeneralUtils::GetVertexData(cp.Points, entity.GetID()))));


		mesh.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(ar::CurveUtils::GenerateC0Indices(points.size()))));
		mesh.Shader = ar::ShaderLib::Get("CurveC0");
		mesh.PickingShader = ar::ShaderLib::Get("CurveC0Picking");
		mesh.RenderPrimitive = ar::Primitive::Patch;
		mesh.TessellationPatchSize = 4;
		mesh.AdaptiveDrawing = true;

		return entity;
	}

	ar::Entity SceneFactory::CreateCurveC2(std::vector<Entity> points, std::optional<uint32_t> id, const std::string& name)
	{
		AR_ASSERT(points.size() > 1, "Too few points to create a curve.");
		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);
		entity.AddComponent<ar::CurveC2Component>();

		auto& cp = entity.AddComponent<ar::ControlPointsComponent>(points);
		for (auto& point : points)
			point.GetComponent<ar::PointComponent>().Parents.push_back(entity);

		auto& mesh = entity.AddComponent<ar::MeshComponent>();
		mesh.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		mesh.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GeneralUtils::GetVertexData(cp.Points, entity.GetID()))));

		mesh.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(ar::CurveUtils::GenerateC2Indices(points.size()))));
		mesh.Shader = ar::ShaderLib::Get("CurveC2");
		mesh.PickingShader = ar::ShaderLib::Get("CurveC2Picking");
		mesh.RenderPrimitive = ar::Primitive::Patch;
		mesh.TessellationPatchSize = 4;
		mesh.AdaptiveDrawing = true;

		return entity;

	}

	ar::Entity SceneFactory::CreateInterpolatedC2(std::vector<Entity> points, std::optional<uint32_t> id, const std::string& name)
	{
		AR_ASSERT(points.size() > 1, "Too few points to create a curve.");
		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);
		entity.AddComponent<ar::InterpolatedC2Component>();

		auto& cp = entity.AddComponent<ar::ControlPointsComponent>(points);
		for (auto& point : points)
			point.GetComponent<ar::PointComponent>().Parents.push_back(entity);

		auto& mesh = entity.AddComponent<ar::MeshComponent>();
		mesh.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		mesh.VertexArray->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::CurveUtils::GetIntC2VertexData(cp.Points, entity.GetID()))));

		mesh.Shader = ar::ShaderLib::Get("CurveC0");
		mesh.PickingShader = ar::ShaderLib::Get("CurveC0Picking");
		mesh.RenderPrimitive = ar::Primitive::Patch;
		mesh.TessellationPatchSize = 4;
		mesh.AdaptiveDrawing = true;

		return entity;

	}

	ar::Entity SceneFactory::CreateSurface(mat::Vec3 origin, SurfaceDesc desc, ar::Entity tempSurface)
	{
		ar::Ref<ar::Shader> shader, pickingShader;

		AR_ASSERT(tempSurface.HasComponent<SurfaceComponent>(), "Cannot accept a non-surface!");

		if (desc.Type == ar::SurfaceType::RECTANGLEC0 || desc.Type == ar::SurfaceType::CYLINDERC0)
		{
			shader = ar::ShaderLib::Get("SurfaceC0");
			pickingShader = ar::ShaderLib::Get("SurfaceC0Picking");
		}
		else
		{
			shader = ar::ShaderLib::Get("SurfaceC2");
			pickingShader = ar::ShaderLib::Get("SurfaceC2Picking");
		}

		// Generate control points
		std::vector<ar::Entity> points;
		auto positions = ar::SurfaceUtils::GenerateSurfaceData(desc, origin);
		for (auto& spawnPoint : positions)
		{
			auto point = CreatePoint(spawnPoint, std::nullopt, "Point");
			point.AddComponent<ar::SurfacePointTagComponent>();
			points.push_back(point);
			point.GetComponent<ar::PointComponent>().Parents.push_back(tempSurface);
		}

		// Assign generated points to a surface
		auto& cp = tempSurface.AddComponent<ar::ControlPointsComponent>();
		cp.Points = ar::SurfaceUtils::GeneratePointReferences(desc, points);
		// In case of cylinders, some points are repeated, and so desc.Size needs to be updated
		desc = ar::SurfaceUtils::AdjustSurfaceDescription(desc);
		cp.Indices = ar::SurfaceUtils::GenerateSurfaceRefIndices(desc);
		auto& d = tempSurface.GetComponent<SurfaceComponent>();
		d.Description = desc;
		if (desc.Type == SurfaceType::RECTANGLEC2 || desc.Type == SurfaceType::CYLINDERC2)
			d.AuxPoints = ar::SurfaceUtils::GetBezierFromDeBoor(tempSurface);

		// Mesh
		auto& mc = tempSurface.GetComponent<ar::MeshComponent>();
		mc.VertexArray->ClearBuffers();
		auto vb = ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GeneralUtils::GetVertexData(cp.Points, tempSurface.GetID())));
		mc.VertexArray->AddVertexBuffer(vb);
		mc.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(cp.Indices)));
		mc.Shader = shader;
		mc.PickingShader = pickingShader;

		// Bezier net (control mesh)
		auto& cpm = tempSurface.AddComponent<ar::ControlMeshComponent>();
		cpm.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		cpm.VertexArray->AddVertexBuffer(vb);
		cpm.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(ar::SurfaceUtils::GenerateControlMeshIndices(desc))));
		cpm.Shader = ar::ShaderLib::Get("Basic");

		return tempSurface;
	}

	ar::Entity SceneFactory::CreateSurface(std::vector<Entity>& points, SurfaceType type, 
		mat::UInt2 size, mat::UInt2 samples, std::optional<uint32_t> id, const std::string& name)
	{
		ar::Ref<ar::Shader> shader, pickingShader;

		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);
		auto& sc = entity.AddComponent<SurfaceComponent>();

		if (type == SurfaceType::RECTANGLEC0 || type == SurfaceType::CYLINDERC0)
		{
			shader = ar::ShaderLib::Get("SurfaceC0");
			pickingShader = ar::ShaderLib::Get("SurfaceC0Picking");
		}
		else
		{
			shader = ar::ShaderLib::Get("SurfaceC2");
			pickingShader = ar::ShaderLib::Get("SurfaceC2Picking");
		}

		SurfaceDesc desc;
		desc.Size = size;
		if (type == SurfaceType::RECTANGLEC0 || type == SurfaceType::CYLINDERC0)
			desc.Segments = { (size.u - 1) / 3, (size.v - 1) / 3 };
		else
			desc.Segments = { size.u - 3, size.v - 3 };
		desc.Samples = samples;
		desc.Type = type;
		sc.Description = desc;

		// Assign generated points to a surface
		for (auto& point : points)
		{
			if (!point.HasComponent<ar::SurfacePointTagComponent>())
				point.AddComponent<ar::SurfacePointTagComponent>();
			auto& parents = point.GetComponent<ar::PointComponent>().Parents;
			if (std::find(parents.begin(), parents.end(), entity) == parents.end())
				parents.push_back(entity);
		}
		auto& cp = entity.AddComponent<ar::ControlPointsComponent>();
		cp.Points = points;
		cp.Indices = ar::SurfaceUtils::GenerateSurfaceRefIndices(desc);
		if (desc.Type == SurfaceType::RECTANGLEC2 || desc.Type == SurfaceType::CYLINDERC2)
			sc.AuxPoints = ar::SurfaceUtils::GetBezierFromDeBoor(entity);

		// Mesh
		auto& mc = entity.AddComponent<ar::MeshComponent>();
		mc.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		auto vb = ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GeneralUtils::GetVertexData(cp.Points, entity.GetID())));
		mc.VertexArray->AddVertexBuffer(vb);
		mc.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(cp.Indices)));
		mc.Shader = shader;
		mc.PickingShader = pickingShader;
		mc.RenderPrimitive = ar::Primitive::Patch;
		mc.TessellationPatchSize = 16;

		// Bezier net (control mesh)
		auto& cpm = entity.AddComponent<ar::ControlMeshComponent>();
		cpm.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		cpm.VertexArray->AddVertexBuffer(vb);
		cpm.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(ar::SurfaceUtils::GenerateControlMeshIndices(desc))));
		cpm.Shader = ar::ShaderLib::Get("Basic");

		return entity;
	}

	ar::Entity SceneFactory::CreateGregoryPatch(ar::Hole holeToFill, std::optional<uint32_t> id, const std::string& name)
	{
		ar::Ref<ar::Shader> shader, pickingShader;
		shader = ShaderLib::Get("Gregory");
		pickingShader = ShaderLib::Get("GregoryPicking");

		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);
		auto& gp = entity.AddComponent<GregoryPatchComponent>();
		gp.HoleToFill = holeToFill;

		// Point references (edges)
		std::vector<ar::Entity> points;
		for (auto& edge : holeToFill.Edges)
		{
			for (auto& point : edge.Points)
				points.push_back(point);
			for (auto& neighbor : edge.Neighbors)
				points.push_back(neighbor);
		}
		for (auto& point : points)
		{
			auto& parents = point.GetComponent<ar::PointComponent>().Parents;
			if (std::find(parents.begin(), parents.end(), entity) == parents.end())
				parents.push_back(entity);
		}
		auto& cp = entity.AddComponent<ar::ControlPointsComponent>();
		cp.Points = points;

		// Mesh
		auto& mc = entity.AddComponent<ar::MeshComponent>();
		mc.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		auto vb = ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GregoryFill::GetGregoryVertexData(holeToFill, entity.GetID())));
		mc.VertexArray->AddVertexBuffer(vb);
		mc.Shader = shader;
		mc.PickingShader = pickingShader;
		mc.RenderPrimitive = ar::Primitive::Patch;
		mc.TessellationPatchSize = 20;

		// Control mesh
		auto& cpm = entity.AddComponent<ar::ControlMeshComponent>();
		cpm.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		cpm.VertexArray->AddVertexBuffer(vb);
		cpm.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(GregoryFill::GetGregoryControlMesh())));
		cpm.Shader = ar::ShaderLib::Get("Basic");

		return entity;
	}

	ar::Entity SceneFactory::CreateIntersectionCurve(std::vector<ar::mat::Vec3> points,
		std::vector<ar::mat::Vec4> params, ar::Entity firstSurface, std::optional<ar::Entity> secondSurface,
		std::optional<uint32_t> id, const std::string& name)
	{
		constexpr mat::Vec3 intersectLineColor = { 0.5f, 0.5f, 1.f };

		ar::Ref<ar::Shader> shader, pickingShader;
		shader = ShaderLib::Get("Point");
		pickingShader = ShaderLib::Get("PointPicking");

		Entity entity;
		if (id.has_value())
			entity = m_Scene->CreateEntity(*id, name);
		else
			entity = m_Scene->CreateEntity(std::nullopt, name);
		auto& ic = entity.AddComponent<IntersectCurveComponent>();
		ic.Points = points;
		ic.Params = params;
		ic.SurfaceP = firstSurface;
		ic.ImageP = ar::IntersectUtils::CreateParameterImage(ic.Params, true, 512, 512);
		if (secondSurface)
		{
			ic.SurfaceQ = secondSurface;
			ic.ImageQ = ar::IntersectUtils::CreateParameterImage(ic.Params, false, 512, 512);
		}
		else
			ic.ImageQ = ic.ImageP;

		// Mesh
		std::vector<VertexPositionIDColor> pointVerts;
		for (auto& point : ic.Points)
			pointVerts.push_back({ point, entity.GetID(), intersectLineColor });

		auto& mesh = entity.AddComponent<ar::MeshComponent>();
		mesh.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		mesh.VertexArray->AddVertexBuffer(Ref<VertexBuffer>(VertexBuffer::Create(pointVerts)));
		mesh.Shader = shader;
		mesh.PickingShader = pickingShader;
		mesh.RenderPrimitive = ar::Primitive::LineStrip;
		mesh.PrimitiveSize = 5.f;
	}

}