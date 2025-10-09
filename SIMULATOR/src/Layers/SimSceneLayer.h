#pragma once
#include <ARCAD.h>
#include <ARMAT.h>
#include "core/Events/Event.h"
#include "SimState.h"
#include "SimRenderer.h"
#include "Controllers/SimUIController.h"
#include "core/CameraController.h"

class SimSceneLayer : public ar::Layer
{
public:
	SimSceneLayer(SimState& state);
	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(ar::Event& event) override;
	void OnImGuiRender() override;
private:
	SimState& m_State;
	ar::Ref<SimUIController> m_UI;
	ar::Ref<SimRenderer> m_Renderer;
	ar::Ref<ar::CameraController> m_Camera;

	void ProcessStateChanges();
};