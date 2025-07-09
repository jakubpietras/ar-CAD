#pragma once
#include <ARCAD.h>
#include <ARMAT.h>

class ExampleLayer : public ar::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{ 
		ar::Vec3 v(1, 2, 3);
		AR_TRACE(v);
	}

	void OnUpdate() override
	{
		
	}

	void OnEvent(ar::Event& event) override
	{
		if (event.GetEventType() == ar::EventType::KeyPressed)
			AR_TRACE("{0}", event.ToString());
	}
};

class EditorApp : public ar::Application
{
public:
	EditorApp();
	~EditorApp();
};
