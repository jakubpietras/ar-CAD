#include "EditorApp.h"

EditorApp::EditorApp()
{
	PushLayer(new ExampleLayer());
	PushOverlay(new ar::ImGuiLayer());
}

EditorApp::~EditorApp()
{

}
