#include "EditorUI.h"
#include "core/ImGui/ComponentInspector.h"
#include "core/ImGui/ScopedDisable.h"
#include <nfd.h>

EditorUI::EditorUI(EditorState& state, ar::Ref<ar::Scene> scene)
	: m_State(state), 
	m_Scene(scene), 
	m_SceneHierarchyPanel(state),
	m_MenuIcon(std::unique_ptr<ar::Texture>(ar::Texture::Create("resources/icons/logo.png")))
{
	m_SceneHierarchyPanel.SetContext(scene);
}

void EditorUI::Render(ar::Ref<ar::Framebuffer> mainFB)
{
	// Panels
	if (m_State.SelectedObjectsWithTransforms.size() > 1)
		RenderGroupTransformControls();
	RenderStatsWindow();
	RenderMainMenu();
	RenderCursorControls();
	RenderInspectorWindow();
	RenderViewport(mainFB);
	
	m_SceneHierarchyPanel.Render();

	RenderPickingBox();
	RenderAddObjectPopup();

	// Modals
	RenderDeleteModal();
	RenderRenameModal();
	RenderErrorModal();
	RenderDetachModal();
	RenderAttachModal();
	RenderCollapseModal();
	RenderAddSurfaceC0Modal();
	RenderAddSurfaceC2Modal();
}

ar::mat::Vec2 EditorUI::GetClickPosition()
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
		return { 0.f, 0.f };
	}
	return { xPos, yPos };
}

void EditorUI::RenderGroupTransformControls()
{
	ImGui::Begin("Group Transform");

	// Translate
	if (ImGui::DragFloat3("dTranslate", m_State.GroupTranslation.Data(), 0.1f, -10.0f, 10.0f))
		m_State.ShouldApplyGroupTransform = true;

	if (ImGui::IsItemActivated())
		m_State.ShouldBeginGroupTransform = true;
	if (ImGui::IsItemDeactivated())
		m_State.ShouldEndGroupTransform = true;

	// Rotate
	if (ImGui::DragFloat3("dRotate", m_State.GroupAnglesRPY.Data(), 0.1f, -180.0f, 180.0f))
		m_State.ShouldApplyGroupTransform = true;
	if (ImGui::IsItemActivated())
		m_State.ShouldBeginGroupTransform = true;
	if (ImGui::IsItemDeactivated())
		m_State.ShouldEndGroupTransform = true;

	// Scale

	ImGui::Checkbox("Uniform scaling", &m_State.UniformGroupScale);
	{
		ar::ScopedDisable disabled(m_State.UniformGroupScale);

		if (ImGui::DragFloat3("dScale", m_State.GroupScale.Data(), 0.05f, 0.1f, 20.0f))
			m_State.ShouldApplyGroupTransform = true;
		if (ImGui::IsItemActivated())
			m_State.ShouldBeginGroupTransform = true;
		if (ImGui::IsItemDeactivated())
			m_State.ShouldEndGroupTransform = true;
	}
	{
		ar::ScopedDisable disabled(!m_State.UniformGroupScale);
		if (ImGui::DragFloat("dUScale", &m_State.GroupScale.x, 0.05f, 0.1f, 20.0f))
		{
			m_State.GroupScale.y = m_State.GroupScale.x;
			m_State.GroupScale.z = m_State.GroupScale.x;
			m_State.ShouldApplyGroupTransform = true;
		}
		if (ImGui::IsItemActivated())
			m_State.ShouldBeginGroupTransform = true;
		if (ImGui::IsItemDeactivated())
			m_State.ShouldEndGroupTransform = true;
	}

	ImGui::End();
}

void EditorUI::RenderPickingBox()
{
	if (m_State.IsBoxPicking)
	{
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		draw_list->AddRect(
			ImVec2(m_State.BoxStart.x, m_State.BoxStart.y),
			ImVec2(m_State.MousePosGlobal.x, m_State.MousePosGlobal.y),
			IM_COL32(255, 255, 255, 128), // Semi-transparent white
			0.0f, 0, 1.0f
		);
	}
}

void EditorUI::RenderStatsWindow()
{
	ImGui::Begin("State");
	ImGui::TextWrapped("* Entity count: %d", m_Scene->GetEntityCount());
	ImGui::TextWrapped("* Viewport size: [%f, %f]", m_State.Viewport.Width, m_State.Viewport.Height);
	ImGui::TextWrapped("* Viewport mouse position: [%f, %f]", m_State.MousePosViewport.x, m_State.MousePosViewport.y);
	ImGui::TextWrapped("* Global mouse position: [%f, %f]", m_State.MousePosGlobal.x, m_State.MousePosGlobal.y);
	ImGui::TextWrapped("* Selection mean position: [%f, %f, %f]", m_State.SelectedMeanPosition.x, m_State.SelectedMeanPosition.y, m_State.SelectedMeanPosition.z);
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
			if (ImGui::MenuItem("Save", "CTRL+S")) { OpenExportDialog(); }
			if (ImGui::MenuItem("Load", "CTRL+L")) { OpenImportDialog(); }
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "CTRL+Q")) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Add"))
		{
			RenderAddMenu();
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

	ImGui::DragFloat3("Cursor position", m_State.CursorPosition.Data(), 0.1f, -20.0f, 20.0f);

	ImGui::End();
}

void EditorUI::RenderViewport(ar::Ref<ar::Framebuffer> mainFB)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
	ImGui::Begin("Viewport");

	ar::Application::Get().ImGuiBlockEvents(!ImGui::IsWindowHovered());

	auto viewportSize = ImGui::GetContentRegionAvail();

	if (viewportSize.x != m_State.Viewport.Width || viewportSize.y != m_State.Viewport.Height)
	{
		m_State.Viewport = { viewportSize.x, viewportSize.y };
		m_State.ViewportResized = true;
	}

	ImGui::Image(
		(ImTextureID)(uintptr_t)mainFB->GetColorAttachment(),
		viewportSize,
		ImVec2(0, 1), ImVec2(1, 0)
	);

	auto mouse = ImGui::GetMousePos();
	m_State.MousePosViewport = GetClickPosition();
	m_State.MousePosGlobal = { mouse.x, mouse.y };

	ImGui::End();
	ImGui::PopStyleVar();
}

void EditorUI::RenderAddObjectPopup()
{
	if (ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT) && ar::Input::IsKeyPressed(AR_KEY_A))
		ImGui::OpenPopup("addObjectsPopup");
	if (ImGui::BeginPopup("addObjectsPopup"))
	{
		RenderAddMenu();
		ImGui::EndPopup();
	}
}

void EditorUI::RenderAddMenu()
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
		if (ImGui::MenuItem("Bezier C0"))
			RequestAddObject(ar::ObjectType::BEZIERC0);
		if (ImGui::MenuItem("Bezier C2"))
			RequestAddObject(ar::ObjectType::BEZIERC2);
		if (ImGui::MenuItem("Interpolated C2"))
			RequestAddObject(ar::ObjectType::INTERPOLATEDC2);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Surface"))
	{
		if (ImGui::MenuItem("Bezier C0"))
			m_State.ShouldShowSurfaceC0Modal = true;
		if (ImGui::MenuItem("Bezier C2"))
			m_State.ShouldShowSurfaceC2Modal = true;

		ImGui::EndMenu();
	}
}

bool EditorUI::RenderRectangleControls()
{
	bool changed = false;
	static const uint32_t rectSegmentsMin = 1, rectSegmentsMax = 64, rectSizeMin = 1, rectSizeMax = 100;
	if(ImGui::DragScalarN("segments##rect", ImGuiDataType_U32, &m_State.NewSurfaceDesc.Segments.u, 2, 1.0f, &rectSegmentsMin, &rectSegmentsMax))
		changed = true;
	if (ImGui::DragFloat2("size##rect", m_State.NewSurfaceDesc.Dimensions.Data(), 0.1f))
		changed = true;

	return changed;
}

bool EditorUI::RenderCylinderControls()
{
	bool changed = false;
	static const uint32_t cylSegmentsMin = 1, cylSegmentsMax = 64;
	ImGui::TextWrapped("Cylinder");
	if (ImGui::DragScalarN("segments##cyl", ImGuiDataType_U32, &m_State.NewSurfaceDesc.Segments.u, 2, 1.0f, &cylSegmentsMin, &cylSegmentsMax))
		changed = true;
	if (m_State.NewSurfaceDesc.Segments.u < 3)
		m_State.NewSurfaceDesc.Segments.u = 3;

	if (ImGui::DragFloat("radius##cyl", &m_State.NewSurfaceDesc.Dimensions.x, 0.1f, 0.1f, 10.0f))
		changed = true;
	if (ImGui::DragFloat("height##cyl", &m_State.NewSurfaceDesc.Dimensions.y, 0.1f, 0.1f, 10.f))
		changed = true;

	return changed;
}

void EditorUI::OpenImportDialog()
{
	auto path = OpenFileDialog();
	if (!path.empty())
	{
		m_State.Filepath = path;
		m_State.ShouldImport = true;
	}
	else
		AR_TRACE("Import canceled");
}

void EditorUI::OpenExportDialog()
{
	auto path = OpenFileDialog();
	if (!path.empty())
	{
		m_State.Filepath = path;
		m_State.ShouldExport = true;
	}
	else
		AR_TRACE("Export canceled");
}

std::string EditorUI::OpenFileDialog()
{
	std::string path;
	nfdu8char_t* outPath;
	nfdu8filteritem_t filters[2] = { { "Scene", "json" }, { "Headers", "h,hpp" } };
	nfdopendialogu8args_t args = { 0 };
	args.filterList = filters;
	args.filterCount = 2;
	nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
	if (result == NFD_OKAY)
	{
		path = std::string(static_cast<char*>(outPath));
		AR_TRACE("File {0} opened sucessfully", path);
		NFD_FreePathU8(outPath);
	}
	else if (result == NFD_CANCEL)
	{
		AR_TRACE("File dialog closed (action canceled)");
	}
	else
	{
		AR_ERROR("Error opening file: {0}", NFD_GetError());
	}
	return path;
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

void EditorUI::RenderAttachModal()
{
	const char* popupName = "Attach?";

	if (m_State.ShowAttachModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowAttachModal = false;
	}

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		std::string message = "Are you sure you want to attach selected points to selected curves?";
		ImGui::TextWrapped(message.c_str());

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			m_State.ShouldAttachPairs = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_State.ClearAttachState();
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void EditorUI::RenderAddSurfaceC0Modal()
{
	const char* popupName = "New Bezier surface C0";

	if (m_State.ShouldShowSurfaceC0Modal)
	{
		ImGui::OpenPopup(popupName);
		m_State.NewSurfaceBegin = true;
		m_State.ShouldShowSurfaceC0Modal = false;
		m_State.NewSurfaceDesc.Type = ar::SurfaceType::RECTANGLEC0;
	}

	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0));

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static int currentC0 = 0;
		static const char* options[] = { "Rectangle", "Cylinder" };
		if (ImGui::Combo("C0 Type", &currentC0, options, IM_ARRAYSIZE(options)))
		{
			switch (currentC0)
			{
			case 0: 
				m_State.NewSurfaceDesc.Type = ar::SurfaceType::RECTANGLEC0; 
				m_State.NewSurfaceDescChanged = true;
				break;
			case 1: 
				m_State.NewSurfaceDesc.Type = ar::SurfaceType::CYLINDERC0; 
				m_State.NewSurfaceDescChanged = true;
				break;
			}
		}

		if (m_State.NewSurfaceDesc.Type == ar::SurfaceType::RECTANGLEC0)
		{
			if (RenderRectangleControls())
				m_State.NewSurfaceDescChanged = true;
		}
		else
		{
			if (RenderCylinderControls())
				m_State.NewSurfaceDescChanged = true;
		}

		if (ImGui::Button("Accept", ImVec2(120, 0)))
		{
			m_State.NewSurfaceAccepted = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_State.NewSurfaceRejected = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleColor();
}

void EditorUI::RenderAddSurfaceC2Modal()
{
	const char* popupName = "New Bezier surface C2";

	if (m_State.ShouldShowSurfaceC2Modal)
	{
		ImGui::OpenPopup(popupName);
		m_State.NewSurfaceBegin = true;
		m_State.ShouldShowSurfaceC2Modal = false;
		m_State.NewSurfaceDesc.Type = ar::SurfaceType::RECTANGLEC2;
	}

	ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0, 0, 0, 0));

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static int currentC2 = 0;
		static const char* options[] = { "Rectangle", "Cylinder" };
		if (ImGui::Combo("C2 Type", &currentC2, options, IM_ARRAYSIZE(options)))
		{
			switch (currentC2)
			{
			case 0:
				m_State.NewSurfaceDesc.Type = ar::SurfaceType::RECTANGLEC2;
				m_State.NewSurfaceDescChanged = true;
				break;
			case 1:
				m_State.NewSurfaceDesc.Type = ar::SurfaceType::CYLINDERC2;
				m_State.NewSurfaceDescChanged = true;
				break;
			}
		}

		if (m_State.NewSurfaceDesc.Type == ar::SurfaceType::RECTANGLEC2)
		{
			if (RenderRectangleControls())
				m_State.NewSurfaceDescChanged = true;
		}
		else
		{
			if (RenderCylinderControls())
				m_State.NewSurfaceDescChanged = true;
		}
			
		if (ImGui::Button("Accept", ImVec2(120, 0)))
		{
			m_State.NewSurfaceAccepted = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_State.NewSurfaceRejected = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleColor();
}

void EditorUI::RenderCollapseModal()
{
	const char* popupName = "Collapse points";

	if (m_State.ShowCollapseModal)
	{
		ImGui::OpenPopup(popupName);
		m_State.ShowCollapseModal = false;
	}

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (m_State.SelectedPoints.size() != 2)
		{
			std::string message = "Exactly two unique points must be selected for collapse!";
			ImGui::TextWrapped(message.c_str());
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				ImGui::CloseCurrentPopup();
			}
		}
		else
		{
			auto& p1 = m_State.SelectedPoints[0], & p2 = m_State.SelectedPoints[1];
			std::string message = "Are you sure you want to collapse "
				+ p1.GetName() + " (ID: " + std::to_string(p1.GetID()) + ") with "
				+ p2.GetName() + " (ID: " + std::to_string(p2.GetID()) + ")?";
			ImGui::TextWrapped(message.c_str());
			if (ImGui::Button("OK", ImVec2(120, 0)))
			{
				m_State.ShouldCollapsePoints = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				m_State.ShouldCollapsePoints = false;
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::EndPopup();
	}
}

void EditorUI::RequestAddObject(ar::ObjectType type)
{
	m_State.AddObjectType = type;
	m_State.ShouldAddObject = true;
}

