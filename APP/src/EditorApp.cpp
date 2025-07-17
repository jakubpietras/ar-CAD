#include "EditorApp.h"
#include "Layers/TestLayer.h"

EditorApp::EditorApp()
{
	PushLayer(new TestLayer());
}

EditorApp::~EditorApp()
{
}
