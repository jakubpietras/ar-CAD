#pragma once
#include <ARCAD.h>

class ExampleLayer : public ar::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{ }

	void OnUpdate() override
	{
		//AR_INFO("ExampleLayer::Update");
	}

	void OnEvent(ar::Event& event) override
	{
		AR_TRACE("{0}", event.ToString());
	}
};

class EditorApp : public ar::Application
{
public:
	EditorApp();
	~EditorApp();
};
