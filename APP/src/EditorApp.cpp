#include "EditorApp.h"
#include "Layers/EditorLayer.h"

EditorApp::EditorApp()
{
	PushLayer(new EditorLayer(GetAspectRatio()));
}

EditorApp::~EditorApp()
{
}
