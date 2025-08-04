#include "arpch.h"
#include "SceneCommands.h"
#include "core/Scene/Components.h"

namespace ar
{

	AddTorusCommand::AddTorusCommand(TorusDesc& desc, std::shared_ptr<Scene> scene,
		std::function<void(ar::Entity)> deleteFunc)
		: m_Description(desc), m_Scene(scene), m_DeleteFunction(deleteFunc)
	{ }

	void AddTorusCommand::Execute()
	{
		m_Entity = m_Scene->CreateEntity("Torus");

		// Torus component
		auto& tc = m_Entity.AddComponent<TorusComponent>();
		tc.Description = m_Description;
		tc.Vertices = TorusUtils::GenerateTorusVertices(m_Description);
		tc.Edges = TorusUtils::GenerateTorusEdges(m_Description);
	
		// Transform component
		auto& trc = m_Entity.AddComponent<TransformComponent>();
		trc.Translation = m_Scene->GetCursorPos();
		
		// Mesh (render) component
		auto& mc = m_Entity.AddComponent<MeshComponent>();
		
		// Primitive
		mc.RenderPrimitive = Primitive::LineLoop;
		
		// VertexArray
		mc.VertexArray = std::unique_ptr<VertexArray>(VertexArray::Create());
		mc.VertexArray->AddVertexBuffer(std::shared_ptr<VertexBuffer>(VertexBuffer::Create(tc.Vertices)));
		auto indexBuffers = IndexBuffer::Create(tc.Edges);
		for (auto& ib : indexBuffers) 
			mc.VertexArray->AddIndexBuffer(ib);
		
		// Shader
		mc.Shader = ar::ShaderLib::Get("Basic");
	}

	void AddTorusCommand::Undo()
	{
		// stub: not needed right now
		return;
	}
}