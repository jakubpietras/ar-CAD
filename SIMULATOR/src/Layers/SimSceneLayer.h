#pragma once
#include <ARCAD.h>
#include <ARMAT.h>
#include "core/Events/Event.h"
#include "SimState.h"
#include "SimRenderer.h"
#include "Controllers/SimUIController.h"
#include "core/CameraController.h"
#include "Milling/MillingStock.h"

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
	std::vector<ar::mat::Vec3> m_MachineCoords;
	ar::Ref<ar::VertexArray> m_PathMesh;
	MillingStock m_Block;
	Heightmap m_HMap;

	void ProcessStateChanges();
	void UpdatePathMesh();
	void Debug();
};