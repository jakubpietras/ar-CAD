#include "arpch.h"
#include "SceneFactory.h"
#include "core/Utils/GeneralUtils.h"
#include "core/Utils/CurveUtils.h"

namespace ar
{

	SceneFactory::SceneFactory(Ref<Scene> scene)
		: m_Scene(scene) { }

	ar::Entity SceneFactory::CreatePoint(mat::Vec3 spawnPoint)
	{
		auto entity = m_Scene->CreateEntity("Point");
		entity.AddComponent<ar::PointComponent>();

		// Transform
		auto& trc = entity.AddComponent<ar::TransformComponent>();
		trc.Translation = spawnPoint;
		trc.IsRotationEnabled = false;
		trc.IsScaleEnabled = false;

		return entity;
	}

	ar::Entity SceneFactory::CreateTorus(mat::Vec3 spawnPoint, TorusDesc desc)
	{
		auto entity = m_Scene->CreateEntity("Torus");

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

	ar::Entity SceneFactory::CreateChain(std::vector<Entity> points)
	{
		AR_ASSERT(points.size() > 1, "Too few points to create a chain.");
		auto entity = m_Scene->CreateEntity("Chain");
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

	ar::Entity SceneFactory::CreateCurveC0(std::vector<Entity> points)
	{
		AR_ASSERT(points.size() > 1, "Too few points to create a curve.");
		auto entity = m_Scene->CreateEntity("Curve C0");
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

	ar::Entity SceneFactory::CreateCurveC2(std::vector<Entity> points)
	{
		AR_ASSERT(points.size() > 1, "Too few points to create a curve.");
		auto entity = m_Scene->CreateEntity("Curve C2");
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

	ar::Entity SceneFactory::CreateInterpolatedC2(std::vector<Entity> points)
	{
		AR_ASSERT(points.size() > 1, "Too few points to create a curve.");
		auto entity = m_Scene->CreateEntity("Interpolated C2");
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

	ar::Entity SceneFactory::CreateSurface(mat::Vec3 origin, SurfaceDesc desc)
	{
		ar::Ref<ar::Shader> shader, pickingShader;

		auto entity = m_Scene->CreateEntity("Surface");
		entity.AddComponent<SurfaceComponent>();
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
			auto point = CreatePoint(spawnPoint);
			point.AddComponent<ar::SurfacePointTagComponent>();
			points.push_back(point);
			point.GetComponent<ar::PointComponent>().Parents.push_back(entity);
		}

		// Assign generated points to a surface
		auto& cp = entity.AddComponent<ar::ControlPointsComponent>();
		cp.Points = ar::SurfaceUtils::GeneratePointReferences(desc, points);
		// In case of cylinders, some points are repeated, and so desc.Size needs to be updated
		desc = ar::SurfaceUtils::AdjustSurfaceDescription(desc);
		cp.Indices = ar::SurfaceUtils::GenerateSurfaceRefIndices(desc);;

		// Mesh
		auto& mc = entity.AddComponent<ar::MeshComponent>();
		auto vb = ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GeneralUtils::GetVertexData(cp.Points, entity.GetID())));
		mc.VertexArray->AddVertexBuffer(vb);
		mc.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(cp.Indices)));

		// Bezier net (control mesh)
		auto& cpm = entity.AddComponent<ar::ControlMeshComponent>();
		cpm.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		cpm.VertexArray->AddVertexBuffer(vb);
		cpm.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(ar::SurfaceUtils::GenerateControlMeshIndices(desc, cp.Indices))));
		cpm.Shader = ar::ShaderLib::Get("Basic");

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
			auto point = CreatePoint(spawnPoint);
			point.AddComponent<ar::SurfacePointTagComponent>();
			points.push_back(point);
			point.GetComponent<ar::PointComponent>().Parents.push_back(tempSurface);
		}

		// Assign generated points to a surface
		auto& cp = tempSurface.AddComponent<ar::ControlPointsComponent>();
		cp.Points = ar::SurfaceUtils::GeneratePointReferences(desc, points);
		// In case of cylinders, some points are repeated, and so desc.Size needs to be updated
		desc = ar::SurfaceUtils::AdjustSurfaceDescription(desc);
		cp.Indices = ar::SurfaceUtils::GenerateSurfaceRefIndices(desc);;

		// Mesh
		auto& mc = tempSurface.GetComponent<ar::MeshComponent>();
		mc.VertexArray->ClearBuffers();
		auto vb = ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(ar::GeneralUtils::GetVertexData(cp.Points, tempSurface.GetID())));
		mc.VertexArray->AddVertexBuffer(vb);
		mc.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(cp.Indices)));

		// Bezier net (control mesh)
		auto& cpm = tempSurface.AddComponent<ar::ControlMeshComponent>();
		cpm.VertexArray = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
		cpm.VertexArray->AddVertexBuffer(vb);
		cpm.VertexArray->AddIndexBuffer(ar::Ref<ar::IndexBuffer>(ar::IndexBuffer::Create(ar::SurfaceUtils::GenerateControlMeshIndices(desc, cp.Indices))));
		cpm.Shader = ar::ShaderLib::Get("Basic");

		return tempSurface;
	}

}