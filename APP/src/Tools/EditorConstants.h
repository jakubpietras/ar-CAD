#pragma once
#include <imgui.h>
#include <ARMAT.h>

namespace EditorUI
{
	constexpr ImVec2 ModalButtonSize{ 120, 0 };
	constexpr ar::mat::Vec4 BackgroundColor{ 0.18f, 0.18f, 0.24f, 1.0f };
} 

namespace EditorCamera
{
	constexpr float FOV = 45.0f;
	constexpr float NearPlane = 0.1f;
	constexpr float FarPlane = 100.0f;
	constexpr float ArcballRadius = 5.0f;
}