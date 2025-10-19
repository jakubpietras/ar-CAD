#include "SimSceneLayer.h"
#include "SimEditorConstants.h"
#include "Tools/StringTools.h"
#include "Tools/GCodeTools.h"
#include "core/Utils/GeneralUtils.h"
#include "core/Scene/DebugRenderer.h"

SimSceneLayer::SimSceneLayer(SimState& state)
	: m_State(state),
	m_Renderer(std::make_shared<SimRenderer>()),
	m_UI(std::make_shared<SimUIController>(state)),
	m_Camera(std::make_shared<ar::CameraController>(
		SimEditorCameraConstants::FOV, 0.5f,
		SimEditorCameraConstants::NearPlane, SimEditorCameraConstants::FarPlane,
		SimEditorCameraConstants::ArcballRadius)),
	m_Block(state.Material),
	m_HMap(state.Material, m_MachineCoords),
	m_Timer()
{
	m_State.Viewport = { 1920.f, 1080.f };
	m_State.ViewportResized = true;
	m_PathMesh = ar::Ref<ar::VertexArray>(ar::VertexArray::Create());
}

void SimSceneLayer::OnAttach()
{
	ar::ShaderLib::Init();
	ar::DebugRenderer::Init();
	//Debug();
}

void SimSceneLayer::OnDetach() { }

void SimSceneLayer::OnUpdate()
{
	m_Timer.Update();
	m_Camera->OnUpdate();
	ProcessStateChanges();
	auto vpMat = m_Camera->GetCamera()->GetVP();
	m_Renderer->Render(vpMat);
	if (!m_MachineCoords.empty())
	{
		if (m_State.ShouldShowPaths) 
			m_Renderer->RenderPaths(m_PathMesh, vpMat);
		m_Renderer->RenderMaterial(vpMat, ar::mat::ToVec4(m_Camera->GetOffset()) - m_Camera->GetPosition(),
			m_Block, m_HMap.GetTexture());
	}
}

void SimSceneLayer::OnEvent(ar::Event& event)
{
	m_Camera->OnEvent(event);
}

void SimSceneLayer::OnImGuiRender()
{
	m_UI->Render(m_Renderer->GetFramebuffer());
}

void SimSceneLayer::ProcessStateChanges()
{
	if (m_State.ViewportResized)
	{
		m_Camera->SetAspectRatio(m_State.Viewport.Width / m_State.Viewport.Height);
		m_Renderer->OnResize({ m_State.Viewport.Width, m_State.Viewport.Height });
		m_State.ViewportResized = false;
	}
	if (m_State.ShouldImport)
	{
		m_MachineCoords = GCodeTools::LoadCoords(m_State.Filepath);
		UpdatePathMesh();

		auto extension = StringTools::LeftTrim(m_State.Filepath.extension().string(), 1);
		m_State.CutterType = GCodeTools::GetCutterType(extension);
		m_State.CutterSize = GCodeTools::GetCutterSize(extension);
		m_State.ClearImportState();
	}
	if (m_State.ShouldMillInstant)
	{
		m_HMap.UpdateMapInstant(m_State.CutterType, m_State.CutterSize / 20,
			m_State.CutterHeight / 10, m_State.Material.BaseHeight);
		m_State.ShouldMillInstant = false;
	}
	if (m_State.StartSimulation)
	{
		m_State.StartPoint = m_MachineCoords[0];
		m_State.StartIndex = 0;
		m_State.IsSimulationRun = true;
		m_State.StartSimulation = false;
	}
	if (m_State.IsSimulationRun)
	{
		m_State.IsSimulationRun = RunSimulation();

	}
}

void SimSceneLayer::UpdatePathMesh()
{
	m_PathMesh->ClearBuffers();
	auto verts = ar::GeneralUtils::GetVertexData(m_MachineCoords);
	m_PathMesh->AddVertexBuffer(ar::Ref<ar::VertexBuffer>(ar::VertexBuffer::Create(verts)));
}

bool SimSceneLayer::RunSimulation()
{
	// returns false when simulation is halted, true if running
	std::vector<ar::mat::Vec3> stops;
	bool isRunning = true;
	ar::mat::Vec3 start = m_State.StartPoint;
	ar::mat::Vec3 end = m_MachineCoords[m_State.StartIndex + 1];

	stops.push_back(start);
	auto dt = m_Timer.GetDeltaTime();
	auto s = m_State.SimulationSpeed * dt;
	do
	{
		auto d = ar::mat::Length(end - start);
		if (s > d)
		{
			s -= d;
			m_State.StartIndex++;
			if (m_State.StartIndex == m_MachineCoords.size() - 1)
			{
				// no more segments
				stops.push_back(end);
				isRunning = false;
				break;
			}
			start = m_MachineCoords[m_State.StartIndex];
			end = m_MachineCoords[m_State.StartIndex + 1];
		}
		else
		{
			auto t = s /*/ d*/;
			auto dir = ar::mat::Normalize(end - start);
			auto q = start + dir * t;
			stops.push_back(q);
			m_State.StartPoint = q;
			s = 0.0f;
			isRunning = true;
		}
	} while (s > 0.0f);

	m_HMap.LoadNewPath(stops);
	m_HMap.UpdateMapInstant(m_State.CutterType, m_State.CutterSize / 20,
		m_State.CutterHeight / 10, m_State.Material.BaseHeight);
	return isRunning;
}

void SimSceneLayer::Debug()
{
	std::string testString = "123.456.78ab.cs";
	auto tokens = StringTools::Tokenize(testString, '.');
	for (auto& token : tokens)
		AR_TRACE("{0}", token);
	auto trimmed = StringTools::LeftTrim(testString, 2);
	AR_TRACE("{0}", trimmed);
}

