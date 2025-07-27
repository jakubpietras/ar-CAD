#pragma once
#include <ARCAD.h>
#include <memory>

class SceneHierarchyPanel
{
public:
	SceneHierarchyPanel() = default;
	SceneHierarchyPanel(std::shared_ptr<ar::Scene> scene, std::shared_ptr<ar::CommandQueue> commandQueue);
	~SceneHierarchyPanel();

	void SetContext(std::shared_ptr<ar::Scene> scene);
	void OnImGuiRender();
private:
	std::shared_ptr<ar::Scene> m_Context;
	std::shared_ptr<ar::CommandQueue> m_CommandQueue;

	void DrawTreeNode(ar::Entity& object);
};

