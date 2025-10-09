#include "SimApp.h"
#include "Layers/SimSceneLayer.h"

SimApp::SimApp()
{
	m_State = SimState();
	PushLayer(new SimSceneLayer(m_State));
}

SimApp::~SimApp() { }