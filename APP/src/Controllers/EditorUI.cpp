#include "EditorUI.h"
#include "core/ImGui/ComponentInspector.h"
#include "core/ImGui/ScopedDisable.h"
#include <nfd.h>
#include "core/Scene/DebugRenderer.h"
#include "fmt/core.h"

EditorUI::EditorUI(EditorState& state, ar::Ref<ar::Scene> scene)
	: m_State(state), 
	m_Scene(scene), 
	m_SceneHierarchyPanel(state),
	m_MenuIcon(std::unique_ptr<ar::Texture>(ar::Texture::Create("resources/icons/logo.png"))),
	m_Paint(200, 200)
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
	RenderParameterImage();
	RenderTrimmingWindow();
	RenderMillingWindow();
	

	m_SceneHierarchyPanel.Render();

	RenderPickingBox();
	RenderAddObjectPopup();
	RenderObjectContextPopup();

	// Modals
	RenderDeleteModal();
	RenderRenameModal();
	RenderErrorModal();
	RenderDetachModal();
	RenderAttachModal();
	RenderCollapseModal();
	RenderAddSurfaceC0Modal();
	RenderAddSurfaceC2Modal();
	RenderAddGregoryModal();
	RenderIntersectModal();
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
	if (ImGui::DragFloat3("dTranslate", m_State.GroupTranslation.Data(), 0.01f, -10.0f, 10.0f))
		m_State.ShouldApplyGroupTransform = true;

	if (ImGui::IsItemActivated())
		m_State.ShouldBeginGroupTransform = true;
	if (ImGui::IsItemDeactivated())
		m_State.ShouldEndGroupTransform = true;

	// Rotate
	if (ImGui::DragFloat3("dRotate", m_State.GroupAnglesRPY.Data(), 0.01f, -180.0f, 180.0f))
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

		if (ImGui::MenuItem("ClearDR"))
		{
			ar::DebugRenderer::Clear();
		}
		if (ImGui::MenuItem("DEBUG"))
		{
			// Place for debug functions
			m_State.ShouldRunDebug = true;


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
	m_State.MousePrevPosViewport = m_State.MousePosViewport;
	m_State.MousePosViewport = GetClickPosition();
	m_State.MousePosChange = m_State.MousePosViewport - m_State.MousePrevPosViewport;
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

void EditorUI::RenderObjectContextPopup()
{
	if (ar::Input::IsKeyPressed(AR_KEY_LEFT_SHIFT) && ar::Input::IsKeyPressed(AR_KEY_C))
		ImGui::OpenPopup("objectContextPopup");
	if (ImGui::BeginPopup("objectContextPopup"))
	{
		auto& lastSelected = m_State.SelectedObjects.back();
		RenderContextMenu(lastSelected);
		ImGui::EndPopup();
	}
}

bool EditorUI::RenderPaintWindow(ar::PaintSurface& paintSurface)
{
	auto handle = paintSurface.GetHandle();
	std::string label = "DrawSurface##" + std::to_string(handle);
	auto width = static_cast<float>(paintSurface.GetWidth());
	auto height = static_cast<float>(paintSurface.GetHeight());
	float titleBarHeight = ImGui::GetFrameHeight();

	ImGuiWindowFlags flags = 
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::SetNextWindowSize(ImVec2(width, height + titleBarHeight), ImGuiCond_Always);

	bool windowOpen = true;
	if (ImGui::Begin(label.c_str(), &windowOpen, flags))
	{
		ImGui::Image(
			(ImTextureID)(uintptr_t)handle,
			ImVec2(width, height),
			ImVec2(0, 1), ImVec2(1, 0)
		);
	}
	ImGui::End();

	ImGui::PopStyleVar(2); // Pop both style vars
	return windowOpen;
}


void EditorUI::RenderParameterImage()
{
	if (m_State.ShouldDisplayParameterImage)
	{
		m_State.ShouldDisplayParameterImage = RenderPaintWindow(*m_State.ImageToDisplay);
		if (!m_State.ShouldDisplayParameterImage)
		{
			if (m_State.SelectedIntersectionCurve)
			{
				(*m_State.SelectedIntersectionCurve).GetComponent<ar::IntersectCurveComponent>().ShowImage = false;
			}
		}
	}
}

void EditorUI::RenderTrimmingWindow()
{
	ImGui::Begin("Trimming");
	if (m_State.SelectedObjects.empty())
	{
		ImGui::TextWrapped("You must choose an intersected surface (Bezier patch or torus) to trim!");
		ImGui::End();
		return;
	}

	auto& surface = m_State.SelectedObjects.back();
	if (surface.HasComponent<ar::TrimmingComponent>() && !surface.GetComponent<ar::TrimmingComponent>().IntersectionCurves.empty())
	{
		auto& t = surface.GetComponent<ar::TrimmingComponent>();

		{
			ar::ScopedDisable disable(t.TrimTexture == nullptr);
			ImGui::Checkbox("Trim surface", &t.ShouldTrimSurface);
		}
		
		// Trimming side
		ImGui::TextWrapped("Trimmed side");
		ImGui::SameLine();
		int side = static_cast<int>(t.Side);
		if (ImGui::RadioButton("Side A", side == 0)) side = 0;
		ImGui::SameLine();
		if (ImGui::RadioButton("Side B", side == 1)) side = 1;

		// Intersection curves list
		t.Side = static_cast<ar::TrimmingComponent::TrimSide>(side);
		// Track which curve is currently bound to TrimTexture
		int selectedCurve = -1;
		for (size_t i = 0; i < t.IntersectionCurves.size(); i++)
		{
			auto& curve = t.IntersectionCurves[i];
			auto& tc = curve.GetComponent<ar::IntersectCurveComponent>();

			// Check if this curve's texture is currently active
			if ((tc.SurfaceP == surface && t.TrimTexture == tc.TrimTexP) ||
				(tc.SurfaceQ == surface && t.TrimTexture == tc.TrimTexQ))
			{
				selectedCurve = static_cast<int>(i);
			}
		}

		for (size_t i = 0; i < t.IntersectionCurves.size(); i++)
		{
			auto& curve = t.IntersectionCurves[i];
			std::string label = curve.GetName() + "##" + std::to_string(i);

			bool isSelected = (selectedCurve == static_cast<int>(i));

			if (ImGui::Selectable(label.c_str(), isSelected))
			{
				auto& tc = curve.GetComponent<ar::IntersectCurveComponent>();
				if (tc.SurfaceP == surface)
					t.TrimTexture = tc.TrimTexP;
				else if (tc.SurfaceQ == surface)
					t.TrimTexture = tc.TrimTexQ;
			}
		}
	}
	else
	{
		ImGui::TextWrapped("You must choose an intersected surface (Bezier patch or torus) to trim!");
	}

	ImGui::End();
}

void EditorUI::RenderMillingWindow()
{
	ImGui::Begin("Milling");
	ImGui::SeparatorText("Setup");
	
	std::string pathsLabel = (m_State.GCodeRoot.empty()) ? "N/A" : m_State.GCodeRoot.string();
	ImGui::TextWrapped(fmt::format("Paths directory: {}", pathsLabel).c_str());
	ImGui::SameLine();
	if (ImGui::Button("Load##paths"))
	{
		auto path = OpenFolderDialog();
		if (!path.empty())
		{
			m_State.GCodeRoot = path;
		}
		else
			AR_TRACE("Import canceled");
	}

	// BASE
	std::string baseName = (m_State.BaseSurface.has_value()) ?
		(m_State.BaseSurface->GetName() + " (ID: " + std::to_string(m_State.BaseSurface->GetID()) + ")")
		: "N/A";
	ImGui::TextWrapped(fmt::format("* Base: {}", baseName).c_str());
	ImGui::SameLine();
	{
		ar::ScopedDisable disable(m_State.SelectedIntersectableSurfaces.empty());
		if (ImGui::Button("Set##base"))
		{
			// sets the most recently selected intersectable surface
			m_State.BaseSurface = m_State.SelectedIntersectableSurfaces.back();
		}
	}
	// OUTLINE
	ImGui::TextWrapped(fmt::format("* Loaded outline surfaces: {}", m_State.OutlineSurfaces.size()).c_str());
	ImGui::SameLine();
	{
		ar::ScopedDisable disable(m_State.SelectedIntersectableSurfaces.empty());
		if (ImGui::Button("Load##outline"))
		{
			m_State.OutlineSurfaces.clear();
			m_State.OutlineSurfaces.insert(
				m_State.OutlineSurfaces.end(),  
				m_State.SelectedIntersectableSurfaces.begin(),
				m_State.SelectedIntersectableSurfaces.end()
			);
		}
	}

	// MODEL
	ImGui::TextWrapped(fmt::format("* Loaded model surfaces: {}", m_State.ModelSurfaces.size()).c_str());
	ImGui::SameLine();
	{
		ar::ScopedDisable disable(m_State.SelectedIntersectableSurfaces.empty());
		if (ImGui::Button("Load##model"))
		{
			m_State.ModelSurfaces.clear();
			m_State.ModelSurfaces.insert(
				m_State.ModelSurfaces.end(),
				m_State.SelectedIntersectableSurfaces.begin(),
				m_State.SelectedIntersectableSurfaces.end()
			);
		}
	}

	ImGui::SeparatorText("Path generation");
	if (ImGui::CollapsingHeader("Heightmap (preview only)"))
	{
		static bool showImage = false;
		const uint32_t minSurfaceSamples = 1, maxSurfaceSamples = 2000;
		ImGui::TextWrapped(fmt::format("Currently selected {} surfaces",
			m_State.SelectedIntersectableSurfaces.size()).c_str());
		ImGui::DragFloat("Base height", &m_State.HMDescription.MinHeight, 0.01f, 0.0f, 10.0f);
		ImGui::DragFloat("Width [cm]", &m_State.HMDescription.RealWidth, 0.1f, 0.1f, 10.0f);
		ImGui::DragFloat("Height [cm]", &m_State.HMDescription.RealHeight, 0.1f, 0.1f, 10.0f);
		ImGui::DragScalar("HMap Sampling (X)", ImGuiDataType_U32, &m_State.HMDescription.SamplesX, 1, &minSurfaceSamples, &maxSurfaceSamples);
		ImGui::DragScalar("HMap Sampling (Y)", ImGuiDataType_U32, &m_State.HMDescription.SamplesY, 1, &minSurfaceSamples, &maxSurfaceSamples);
		ImGui::DragScalar("Surface Sampling", ImGuiDataType_U32, &m_State.HMDescription.SurfaceSamples, 1, &minSurfaceSamples, &maxSurfaceSamples);
		ImGui::Text(fmt::format("Lower left corner [X-Z plane]: ({:.2f}, {:.2f})",
			m_State.HMDescription.LowerLeftCorner.x, m_State.HMDescription.LowerLeftCorner.y).c_str());
		ImGui::SameLine();
		if (ImGui::Button("Get Cursor"))
			m_State.HMDescription.LowerLeftCorner = { m_State.CursorPosition.x, m_State.CursorPosition.z };
		if (ImGui::Button("Compute height map"))
		{
			m_State.ShouldComputeHeightmap = true;
		}
		ImGui::SameLine();
		{
			ar::ScopedDisable disable(!m_State.HeightmapImage);
			if (ImGui::Button("Show preview"))
			{
				showImage = !showImage;
			}
		}
		if (showImage)
		{
			auto size = ImGui::GetContentRegionAvail();
			ImGui::Image(
				(ImTextureID)(uintptr_t)m_State.HeightmapImage->GetID(),
				ImVec2(size.x, size.x),
				ImVec2(0, 1), ImVec2(1, 0)
			);
		}
	}

	if (ImGui::CollapsingHeader("Rough milling"))
	{
		if (ImGui::Button("Generate"))
		{
			m_State.ShouldGenerateFaceMillPaths = true;
		}
	}
	if (ImGui::CollapsingHeader("Base milling"))
	{
		if (ImGui::TreeNode("Base mill"))
		{
			if (ImGui::Button("Generate"))
			{
				m_State.ShouldGenerateBaseMillPaths = true;
			}
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Outline mill"))
		{
			if (ImGui::TreeNode("Outline curves"))
			{
				ImGui::TextWrapped("Generate all intersection curves between chosen BASE and all OUTLINE surfaces.");
				if (ImGui::Button("Generate curves"))
				{
					m_State.ShouldComputeAllIntCurves = true;
				}
				ImGui::TextWrapped(fmt::format("Resize all {} curves (default: 0.5cm, away from the model)", m_State.OutlineCurves.size()).c_str());
				{
					ar::ScopedDisable disable(m_State.OutlineCurves.empty());
					if (ImGui::Button("Resize curves"))
					{
						m_State.ShouldResizeOutlineCurves = true;
					}
				}
				
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Stitching"))
			{
				ImGui::TextWrapped("Stitch outline curves into one");
				std::string outlineName = (m_State.FirstOutline.has_value()) ? m_State.FirstOutline->GetName() : "N/A";
				ImGui::TextWrapped(fmt::format("Base outline: {}", outlineName).c_str());
				ImGui::SameLine();
				{
					ar::ScopedDisable disable(m_State.SelectedIntersectionCurves.empty());
					if (ImGui::Button("Set"))
					{
						m_State.FirstOutline = m_State.SelectedIntersectionCurves.back();
					}
					if (ImGui::Button("Stitch outline curves"))
					{
						m_State.ShouldStitchOutlineCurves = true;
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Outline path generation"))
			{
				ImGui::Text(fmt::format("Interpolated outline curve: {}", (m_State.InterpolatedOutline.has_value()) ? m_State.InterpolatedOutline->GetName() : "N/A").c_str());
				ImGui::SameLine();
				{
					ar::ScopedDisable disable(m_State.SelectedCurves.empty());
					if (ImGui::Button("Set interpolated outline curve"))
					{
						m_State.InterpolatedOutline = m_State.SelectedCurves.back();
					}
				}

				{
					ar::ScopedDisable disable(!m_State.InterpolatedOutline.has_value());
					ImGui::Text(fmt::format("Start point: {}", (m_State.OutlineStartPoint.has_value()) ? std::to_string(m_State.OutlineStartPoint->GetID()) : "N/A").c_str());
					ImGui::SameLine();
					if (ImGui::Button("Set point"))
					{
						m_State.OutlineStartPoint = m_State.SelectedPoints.back();
					}
					ImGui::DragFloat3("Outline offset dir", &m_State.OutlineStartOffset.x, 0.1f, -1.0, 1.0);
					if (ImGui::Button("Generate paths"))
					{
						m_State.ShouldGenerateOutlineMillPaths = true;
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("[DEBUG] Check intersections"))
			{
				if (ImGui::Button("Intersect intersection curves"))
				{
					if (m_State.SelectedIntersectionCurves.size() == 2)
					{
						auto idxs = ar::Intersection::IntersectCurves(m_State.SelectedIntersectionCurves[0], m_State.SelectedIntersectionCurves[1]);
						auto& p = m_State.SelectedIntersectionCurves.front().GetComponent<ar::IntersectCurveComponent>();
						ar::DebugRenderer::AddPoint(p.Points[idxs.x], {1, 0 ,0});
						ar::DebugRenderer::AddPoint(p.Points[idxs.z], { 1, 0 ,0 });
					}
				}

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("[DEBUG] Check stitching"))
			{
				if (ImGui::Button("Stitch"))
				{
					if (m_State.SelectedIntersectionCurves.size() == 2)
					{
						ar::Intersection::StitchIntersectionCurves(m_State.SelectedIntersectionCurves[0], m_State.SelectedIntersectionCurves[1]);
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("[DEBUG] Check normals"))
			{
				if (m_State.SelectedIntersectionCurve)
				{
					ImGui::TextWrapped(fmt::format("Visualize normal vectors of the curve: {} (ID: {})", m_State.SelectedIntersectionCurve->GetName(), m_State.SelectedIntersectionCurve->GetID()).c_str());
					if (ImGui::Button("Show/Hide P"))
					{
						m_State.ShowSelectedIntCurveNormals = !m_State.ShowSelectedIntCurveNormals;
						if (m_State.ShowSelectedIntCurveNormals)
						{
							auto& data = *m_State.SelectedIntersectionCurve;
							auto& intComponent = data.GetComponent<ar::IntersectCurveComponent>();
							for (int i = 0; i < intComponent.Points.size(); i++)
							{
								ar::DebugRenderer::AddLine(intComponent.Points[i], intComponent.Points[i] + intComponent.NormalsP[i] * 0.1f);
							}
						}
						else
							ar::DebugRenderer::Clear();
					}
					if (ImGui::Button("Show/Hide Q"))
					{
						m_State.ShowSelectedIntCurveNormals = !m_State.ShowSelectedIntCurveNormals;
						if (m_State.ShowSelectedIntCurveNormals)
						{
							auto& data = *m_State.SelectedIntersectionCurve;
							auto& intComponent = data.GetComponent<ar::IntersectCurveComponent>();
							for (int i = 0; i < intComponent.Points.size(); i++)
							{
								ar::DebugRenderer::AddLine(intComponent.Points[i], intComponent.Points[i] + intComponent.NormalsQ[i] * 0.1f);
							}
						}
						else
							ar::DebugRenderer::Clear();
					}
				}
				else
					ImGui::TextWrapped("No intersection curve chosen");
				ImGui::TreePop();
			}
			if (ImGui::Button("Generate"))
			{
				m_State.ShouldGenerateOutlineMillPaths = true;
			}


			ImGui::TreePop();
		}
	}

	ImGui::End();
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
	ImGui::Separator();
	if (ImGui::BeginMenu("Curves"))
	{
		if (ImGui::MenuItem("Chain"))
			RequestAddObject(ar::ObjectType::CHAIN);
		if (ImGui::MenuItem("C0 Bezier Curve"))
			RequestAddObject(ar::ObjectType::BEZIERC0);
		if (ImGui::MenuItem("C2 Bezier Curve"))
			RequestAddObject(ar::ObjectType::BEZIERC2);
		if (ImGui::MenuItem("Interpolated C2"))
			RequestAddObject(ar::ObjectType::INTERPOLATEDC2);
		ImGui::EndMenu();
	}
	{
		ar::ScopedDisable disable(m_State.ShouldShowIntersectModal || m_State.SelectedIntersectableSurfaces.size() == 0 ||
			m_State.SelectedIntersectableSurfaces.size() > 2);
		if (ImGui::MenuItem("Intersection Curve"))
			m_State.ShouldShowIntersectModal = true;
	}
	ImGui::Separator();
	if (ImGui::BeginMenu("Surfaces"))
	{
		if (ImGui::MenuItem("C0 Bezier Surface"))
			m_State.ShouldShowSurfaceC0Modal = true;
		if (ImGui::MenuItem("C2 Bezier Surface"))
			m_State.ShouldShowSurfaceC2Modal = true;
		ImGui::EndMenu();
	}
	{
		ar::ScopedDisable disable(m_State.ShouldShowGregoryModal);
		if (ImGui::MenuItem("Gregory patch"))
			m_State.ShouldShowGregoryModal = true;
	}
}

void EditorUI::RenderContextMenu(ar::Entity object)
{
	if (object.HasComponent<ar::PointComponent>())
	{
		if (ImGui::MenuItem("Collapse"))
		{
			m_State.ShowCollapseModal = true;
		}
	}
	if (object.HasComponent<ar::SurfaceComponent>())
	{
		if (ImGui::MenuItem("Select all points"))
		{
			m_State.SelectionChangeMode = SelectionMode::Replace;
			m_State.SelectionCandidates.clear();
			auto& children = object.GetComponent<ar::ControlPointsComponent>().Points;
			m_State.SelectionCandidates.insert(
				m_State.SelectionCandidates.begin(),
				children.begin(),
				children.end()
			);
			m_State.ShouldUpdateSelection = true;
		}
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

std::string EditorUI::OpenFolderDialog()
{
	std::string path;
	nfdu8char_t* outPath;
	nfdresult_t result = NFD_PickFolder(&outPath, NULL);
	if (result == NFD_OKAY)
	{
		path = std::string(static_cast<char*>(outPath));
		AR_TRACE("Folder {0} opened sucessfully", path);
		NFD_FreePathU8(outPath);
	}
	else if (result == NFD_CANCEL)
	{
		AR_TRACE("Folder dialog closed (action canceled)");
	}
	else
	{
		AR_ERROR("Error opening folder: {0}", NFD_GetError());
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
			currentC0 = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_State.NewSurfaceRejected = true;
			ImGui::CloseCurrentPopup();
			currentC0 = 0;
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
			currentC2 = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			m_State.NewSurfaceRejected = true;
			ImGui::CloseCurrentPopup();
			currentC2 = 0;
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleColor();
}

void EditorUI::RenderAddGregoryModal()
{
	if (m_State.ShouldShowGregoryModal)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiCond_Once);
		ImGui::Begin("Gregory", &m_State.ShouldShowGregoryModal,
			ImGuiWindowFlags_NoCollapse);

		float halfWidth = ImGui::GetContentRegionAvail().x * 0.5f;
		float height = 200.0f;

		ImGui::BeginChild("SelectedPatches", ImVec2(halfWidth, height), true);
		ImGui::TextWrapped("Selected C0 Patches");
		ImGui::Separator();
		for (auto& patch : m_State.SelectedSurfacesC0)
		{
			std::string label = patch.GetName() + " (ID: " + std::to_string(patch.GetID()) + ")";
			ImGui::Text(label.c_str());
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild("DetectedHoles", ImVec2(halfWidth, height), true);

		ImGui::TextWrapped("Detected Holes");
		ImGui::Separator();

		for (auto& hole : m_State.FillCandidates)
		{
			if (ImGui::Selectable(hole.ToString().c_str()))
			{
				m_State.HoleToFill = hole;
				m_State.HoleSelectionChanged = true;
			}
		}

		ImGui::EndChild();

		float buttonWidth = 100.0f;
		float spacing = 20.0f;
		float totalWidth = 3 * buttonWidth + 2 * spacing;
		float offset = (ImGui::GetContentRegionAvail().x - totalWidth) * 0.5f;

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
		if (ImGui::Button("Detect Holes", ImVec2(buttonWidth, 0))) { m_State.ShouldScanForHoles = true; }
		ImGui::SameLine();
		if (ImGui::Button("Add Fill-in", ImVec2(buttonWidth, 0))) 
		{ 
			RequestAddObject(ar::ObjectType::GREGORY);
			m_State.ShouldShowGregoryModal = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) 
		{
			m_State.ClearFillState();
			m_State.ShouldShowGregoryModal = false; 
			m_State.HoleSelectionChanged = true;
		}

		ImGui::End();
	}
	
}

void EditorUI::RenderCollapseModal()
{
	const char* popupName = "Collapse points";

	if (m_State.ShowCollapseModal)
	{
		// Set popup position near mouse cursor
		ImVec2 mousePos = ImGui::GetMousePos();
		ImVec2 popupPos = ImVec2(mousePos.x + 10, mousePos.y + 10); // Small offset
		ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing);

		ImGui::OpenPopup(popupName);
		m_State.ShowCollapseModal = false;
	}

	if (ImGui::BeginPopupModal(popupName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		// Rest of your code remains the same...
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

void EditorUI::RenderIntersectModal()
{
	if (m_State.ShouldShowIntersectModal)
	{
		ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiCond_Once);
		ImGui::Begin("Intersection", &m_State.ShouldShowIntersectModal,
			ImGuiWindowFlags_NoCollapse);

		float halfWidth = ImGui::GetContentRegionAvail().x * 0.5f;
		float height = 200.0f;

		ImGui::TextWrapped("Adding Intersection Curve");

		std::string label = "Intersecting: ";
		auto& objects = m_State.SelectedIntersectableSurfaces;

		if (objects.size() == 1)
			label += objects[0].GetName() + " (ID: " + std::to_string(objects[0].GetID()) + ") [Self-intersection]";
		else
			label += objects[0].GetName() + " (ID: " + std::to_string(objects[0].GetID()) + ") with " + objects[1].GetName() + " (ID: " + std::to_string(objects[1].GetID()) + ")";

		ImGui::TextWrapped(label.c_str());

		ImGui::Checkbox("Cursor Assist", &m_State.ShouldUseCursorAssist);

		ImGui::SeparatorText("Precision controls");
		ImGui::DragFloat("Step size", &m_State.StepDistance, 0.001f, 0.0001f, 0.5f);

		if (ImGui::Button("Add")) 
		{ 
			m_State.ShouldComputeIntersection = true;
			m_State.ShouldShowIntersectModal = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
		{
			m_State.ShouldShowIntersectModal = false;
		}
		ImGui::End();
	}
}

void EditorUI::RequestAddObject(ar::ObjectType type)
{
	m_State.AddObjectType = type;
	m_State.ShouldAddObject = true;
}

