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
			AR_TRACE(event);
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
