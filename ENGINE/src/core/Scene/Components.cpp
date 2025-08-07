#pragma once
#include "arpch.h"
#include "Components.h"
#include "Entity.h"

namespace ar
{

	ControlPointsComponent::ControlPointsComponent()
		: Points({}) { }


	ControlPointsComponent::ControlPointsComponent(std::vector<Entity> initialPoints)
		: Points(initialPoints) { }

}