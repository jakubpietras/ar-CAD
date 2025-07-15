#pragma once
#include <ARCAD.h>
#include <ARMAT.h>

class ExampleLayer : public ar::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{ 
	}

	void OnUpdate() override
	{
			
	}

	void OnEvent(ar::Event& event) override
	{
		if (event.GetEventType() == ar::EventType::KeyPressed)
		{
			AR_TRACE(event);
			std::vector<ar::VertexPosition> verts = { { {1.0f, 1.0f, 1.0f} } };
			ar::VertexBuffer* vb = ar::VertexBuffer::Create(verts);
			auto layout = vb->GetLayout();
			ar::VertexArray* va = ar::VertexArray::Create();
			va->AddVertexBuffer(std::shared_ptr<ar::VertexBuffer>(vb));
		}
	}

	void OnImGuiRender() override
	{
		
	}
};

class EditorApp : public ar::Application
{
public:
	EditorApp();
	~EditorApp();
};
