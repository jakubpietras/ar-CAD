#include "EditorApp.h"
#include "Layers/EditorLayer.h"

EditorApp::EditorApp()
{
	PushLayer(new EditorLayer());
}

EditorApp::~EditorApp()
{
}
