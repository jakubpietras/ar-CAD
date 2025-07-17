#include <arpch.h>
#include "EditorApp.h"

EditorApp::EditorApp()
{
	PushLayer(new ExampleLayer());
}

EditorApp::~EditorApp()
{

}

void ExampleLayer::OnEvent(ar::Event& event)
{
	ar::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<ar::KeyPressedEvent>(AR_BIND_EVENT_FN(ExampleLayer::OnKeyPressed));
	dispatcher.Dispatch<ar::MouseMovedEvent>(AR_BIND_EVENT_FN(ExampleLayer::OnMouseMoved));
	dispatcher.Dispatch<ar::MouseScrolledEvent>(AR_BIND_EVENT_FN(ExampleLayer::OnMouseScrolled));

}

bool ExampleLayer::OnKeyPressed(ar::KeyPressedEvent& event)
{
	// returns false if event is to be handled further, true if we want to block it here

	if (event.GetKeyCode() == AR_KEY_W)
		camera->Rotate(0.0f, 10.0f, 0.0f);
	else if (event.GetKeyCode() == AR_KEY_A)
		camera->Rotate(10.0f, 0.0f, 0.0f);

	return false;
}

bool ExampleLayer::OnMouseMoved(ar::MouseMovedEvent& event)
{
	if (ar::Input::IsMouseButtonPressed(AR_MOUSE_BUTTON_MIDDLE))
	{
		camera->Rotate(event.GetYOffset(), event.GetXOffset(), 0.0f);
	}
	return false;
}

bool ExampleLayer::OnMouseScrolled(ar::MouseScrolledEvent& event)
{
	camera->UpdateArcballRadius(event.GetYOffset());
	return false;
}
