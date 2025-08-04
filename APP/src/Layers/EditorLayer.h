#pragma once
#include <ARCAD.h>
#include <ARMAT.h>
#include "core/ImGui/ComponentInspector.h"

class EditorLayer : public ar::Layer
{

public:
	struct SelectionState
	{
		ar::Entity CurrentlySelected = {entt::null, nullptr};
		std::vector<ar::Entity> SelectedPoints{};
	};

	EditorLayer(float aspectRatio);

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate() override;
	void OnEvent(ar::Event& event) override;
	void OnImGuiRender() override;

	// Event handlers
	bool OnMouseMoved(ar::MouseMovedEvent& event);
	bool OnMouseScrolled(ar::MouseScrolledEvent& event);

	// GUI functions
	void ShowStats();
	void ShowMenu();
	void ShowViewport();

	void ShowSceneHierarchy();
	void ShowInspector();
	void DrawTreeNode(ar::Entity& object);

	// Commands
	void AddObject(ar::ObjectType type);
	void DeleteObject(ar::Entity object);

	// Selection
	void SelectObject(ar::Entity object);
	void DeselectObject(ar::Entity object);
	void DeselectAll();
	

private:
	// Selection
	SelectionState m_Selection;

	// Logic
	ar::Ref<ar::Scene> m_Scene;
	ar::Ref<ar::CameraController> m_CameraController;
	std::pair<float, float> m_ViewportSize;
	
	// Rendering
	ar::Ref<ar::Framebuffer> m_ViewportFramebuffer;

	// Textures
	ar::Scope<ar::Texture> m_MenuIcon;


	void AddTorus(ar::TorusDesc desc);
};


