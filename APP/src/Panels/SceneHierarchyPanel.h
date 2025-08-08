#pragma once
#include "ARCAD.h"

class SceneHierarchyPanel
{
public:
	void Render();
	void SetContext(ar::Ref<ar::Scene> scene);

private:
	void DrawEntityNode();

};