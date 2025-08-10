#include "EditorUI.h"
#include "core/ImGui/ComponentInspector.h"
#include "core/ImGui/ScopedDisable.h"

EditorUI::EditorUI(EditorState& state, ar::Ref<ar::Scene> scene)
	: m_State(state), 
	m_Scene(scene), 
	m_SceneHierarchyPanel(state),
	m_MenuIcon(std::unique_ptr<ar::Texture>(ar::Texture::Create("resources/icons/logo.png"))),
	m_ViewportFramebuffer(std::shared_ptr<ar::Framebuffer>(ar::Framebuffer::Create({ 1920, 1080 })))
{
	m_SceneHierarchyPanel.SetContext(scene);
}

void EditorUI::Render()
{
	// Panels
	RenderStatsWindow();
	RenderMainMenu();
	RenderCursorControls();
	RenderInspectorWindow();
	RenderViewport();
	m_SceneHierarchyPanel.Render();

	// Modals
	RenderDeleteModal();
	RenderRenameModal();
	RenderErrorModal();
	RenderDetachModal();
}

const ar::Ref<ar::Framebuffer>& EditorUI::GetFramebuffer()
{
	return m_ViewportFramebuffer;
}

void EditorUI::RenderStatsWindow()
{
	ImGui::Begin("Stats");
	ImGui::TextWrapped("Current entity count: %d", m_Scene->GetEntityCount());
	ImGui::End();
}

void EditorUI::RenderMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		ImGui::Image(
			(ImTextureID)(uintptr_t)m_MenuIcon->GetID(),
			ImVec2(48, 16),
			ImVec2(0, 1), ImVec2(1, 0)
		);

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save", "CTRL+S")) {}
			if (ImGui::MenuItem("Load", "CTRL+L")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "CTRL+Q")) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Add"))
		{
			if (ImGui::MenuItem("Point")) 
				RequestAddObject(ar::ObjectType::POINT);
			if (ImGui::BeginMenu("Mesh"))
			{
				if (ImGui::MenuItem("Torus")) 
					RequestAddObject(ar::ObjectType::TORUS);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Curve"))
			{
				if (ImGui::MenuItem("Chain")) 
					RequestAddObject(ar::ObjectType::CHAIN);
					
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Surface"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

void EditorUI::RenderInspectorWindow()
{
	ImGui::Begin("Inspector");

	if (!m_State.SelectedObjects.empty())
	{
		auto object = m_State.SelectedObjects.back();
		if (object)
			ar::ComponentInspector::ShowInspector(object);
	}

	ImGui::End();
}

void EditorUI::RenderCursorControls()
{
	ImGui::Begin("Cursor");

	ar::mat::Vec3 position = m_Cursor.GetPosition();
	if (ImGui::DragFloat3("Cursor position", position.Data(), 0.1f, -20.0f, 20.0f))
	{
		m_Cursor.SetPosition(position);
	}

	ImGui::End();
}

void EditorUI::RenderViewport()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	ar::Application::Get().ImGuiBlockEvents(!ImGui::IsWindowHovered());

	auto viewportSize = ImGui::GetContentRegionAvail();

	if (viewportSize.x != m_State.Viewport.Width || viewportSize.y != m_State.Viewport.Height)
	{
		m_State.Viewport = { viewportSize.x, viewportSize.y };
		m_State.ViewportResized = true;
		m_ViewportFramebuffer->Resize(static_cast<uint32_t>(viewportSize.x), static_cast<uint32_t>(viewportSize.y));
	}

	ImGui::Image(
		(ImTextureID)(uintptr_t)m_ViewportFramebuffer->GetColorAttachment(),
		viewportSize,
		ImVec2(0, 1), ImVec2(1, 0)
	);

		
	if (ar::Input::IsMouseButtonPressed(AR_MOUSE_BUTTON_RIGHT) && ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT))
		RequestPlaceCursor();

	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorUI::RenderDeleteModal()
{
	const char* popupName = "Delete?";

	if (m_State.ShowDeleteModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowDeleteModal = false;
	}

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::string message = "Are you sure you want to delete "
			+ std::to_string(m_State.ObjectsToDelete.size())
			+ " object(s)?";
		ImGui::TextWrapped(message.c_str());

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_State.ShouldDeleteObjects = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
			m_State.ClearDeleteState();
		}
		ImGui::EndPopup();
	}
}

void EditorUI::RenderRenameModal()
{
	const char* popupName = "Rename?";

	if (m_State.ShowRenameModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowRenameModal = false;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputTextWithHint("##new_name", "Enter new name here", m_State.RenameBuffer, IM_ARRAYSIZE(m_State.RenameBuffer));

		std::string message = "Are you sure you want to change the name?";
		ImGui::TextWrapped(message.c_str());
		{
			ar::ScopedDisable disabledOK(m_State.RenameBuffer[0] == '\0');
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				m_State.ShouldRenameObject = true;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_State.ClearRenameState();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void EditorUI::RenderErrorModal()
{
	const char* popupName = "Error";

	if (m_State.ShowErrorModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowErrorModal = false;
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped(m_State.ErrorMessages[0].c_str());

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void EditorUI::RenderDetachModal()
{
	const char* popupName = "Detach?";

	if (m_State.ShowDetachModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowDetachModal = false;
	}

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::string message = "Are you sure you want to detach this point?";
		ImGui::TextWrapped(message.c_str());

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_State.ShouldDetachPairs = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_State.ClearDetachState();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void EditorUI::RequestAddObject(ar::ObjectType type)
{
	m_State.AddObjectType = type;
	m_State.ShouldAddObject = true;
}

void EditorUI::RequestPlaceCursor()
{
	ImVec2 viewport_pos = ImGui::GetItemRectMin();
	ImVec2 viewport_size = ImGui::GetItemRectSize();
	ImVec2 mouse_pos = ImGui::GetIO().MousePos;

	float xPos = mouse_pos.x - viewport_pos.x;
	float yPos = mouse_pos.y - viewport_pos.y;

	if (xPos < 0 || yPos < 0 ||
		xPos >= viewport_size.x ||
		yPos >= viewport_size.y)
	{
		return;
	}

	m_State.ClickPosition = { xPos, yPos, 0.0f };
	m_State.ShouldPlaceCursor = true;
}

