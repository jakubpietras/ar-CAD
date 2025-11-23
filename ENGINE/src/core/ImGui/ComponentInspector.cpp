#include "arpch.h"
#include "core/Utils/TorusUtils.h"
#include "ComponentInspector.h"
#include "PropertyInspector.h"
#include "core/ImGui/ScopedDisable.h"
#define DEBUG

void ar::ComponentInspector::ShowInspector(Entity entity)
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;

	std::string line = entity.GetName() + " [ID: " + std::to_string(entity.GetID()) + "]";


	ImGui::SeparatorText(line.c_str());
#ifdef DEBUG
	std::string debugMsg = "[DEBUG] entt ID: " + std::to_string(static_cast<uint32_t>(entity.GetEnttHandle()));
	ImGui::Text(debugMsg.c_str());
#endif

	if (ImGui::BeginTabBar("Inspector", tab_bar_flags))
	{
		ShowComponentInspector<ar::TorusComponent>(entity, "Props");
		ShowComponentInspector<ar::TransformComponent>(entity, "Transform");
		ShowComponentInspector<ar::CurveC0Component>(entity, "Props");
		ShowComponentInspector<ar::CurveC2Component>(entity, "Props");
		ShowComponentInspector<ar::SurfaceComponent>(entity, "Props");
		ShowComponentInspector<ar::GregoryPatchComponent>(entity, "Props");
		ShowComponentInspector<ar::IntersectCurveComponent>(entity, "Props");

		// rest...
		ImGui::EndTabBar();
	}
}

void ar::ComponentInspector::InspectComponent(TorusComponent& torus)
{
	if (PropertyInspector::InspectProperty("Small Radius", torus.Description.SmallRadius, 0.1f, 10.f))
		torus.DirtyFlag = true;
	if (PropertyInspector::InspectProperty("Large Radius", torus.Description.LargeRadius, 0.1f, 10.f))
		torus.DirtyFlag = true;
	if (PropertyInspector::InspectProperty("Samples", torus.Description.Samples, 3u, 128u))
		torus.DirtyFlag = true;

}

void ar::ComponentInspector::InspectComponent(IntersectCurveComponent& intcurve)
{
	std::string message;
	if (!intcurve.SurfaceQ)
		message = "Self-intersecting " + intcurve.SurfaceP.GetName() + "(ID: " + std::to_string(intcurve.SurfaceP.GetID()) + ")";
	else
		message = "Intersecting " + intcurve.SurfaceP.GetName() + "(ID: " + std::to_string(intcurve.SurfaceP.GetID()) + ") "
		+ "with " + (*intcurve.SurfaceQ).GetName() + "(ID: " + std::to_string((*intcurve.SurfaceQ).GetID()) + ")";
	ImGui::TextWrapped(message.c_str());
	ImGui::SeparatorText("Curve conversion");
	ImGui::TextWrapped("To convert this intersection curve into an interpolatory C2 spline, enter the desired number of control points.");
	PropertyInspector::InspectProperty("points count", intcurve.ConversionPointsCount, 3u, intcurve.Points.size());
	if (ImGui::Button("Convert"))
	{
		intcurve.ConvertToSpline = true;
	}

	ImGui::SeparatorText("Resize");
	PropertyInspector::InspectProperty("length", intcurve.ResizeLength, 0.0f, 10.0f);
	ImGui::Checkbox("Outward", &intcurve.ResizeOutside);
	if (ImGui::Button("Flatten normals"))
	{
		
	}
	if (ImGui::Button("Resize (P normals)"))
	{
		intcurve.UseNormalsP = true;
		intcurve.ShouldResize = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Resize (Q normals)"))
	{
		intcurve.UseNormalsP = false;
		intcurve.ShouldResize = true;
	}

	ImGui::SeparatorText("Images");
	if (ImGui::Button("Surface P"))
	{
		intcurve.ShowImageP = true;
		intcurve.ShowImageQ = false;
		intcurve.ShowImage = true;
	}
	if (intcurve.SurfaceQ)
	{
		ImGui::SameLine();
		if (ImGui::Button("Surface Q"))
		{
			intcurve.ShowImageP = false;
			intcurve.ShowImageQ = true;
			intcurve.ShowImage = true;
		}
	}
}

void ar::ComponentInspector::InspectComponent(GregoryPatchComponent& gregory)
{
	ImGui::Checkbox("Show net", &gregory.ShowNet);
	PropertyInspector::InspectProperty("Samples", gregory.Samples, 4u, 64u);
}

void ar::ComponentInspector::InspectComponent(SurfaceComponent& surface)
{
	ImGui::Checkbox("Show net", &surface.ShowNet);
	PropertyInspector::InspectProperty("Samples", surface.Description.Samples, 4u, 64u);
}

void ar::ComponentInspector::InspectComponent(CurveC2Component& curve)
{
	ImGui::Checkbox("Show polygon", &curve.ShowPolygon);
}

void ar::ComponentInspector::InspectComponent(CurveC0Component& curve)
{
	ImGui::Checkbox("Show polygon", &curve.ShowPolygon);
}

void ar::ComponentInspector::InspectComponent(TransformComponent& transform)
{
	if (PropertyInspector::InspectProperty("Translation", transform.Translation, transform.PreviousTranslation, -20.0f, 20.0f))
	{
		transform.GroupTransformation = false;
		transform.DirtyFlag = true;
	}
	{
		ScopedDisable disabled(!transform.IsRotationEnabled && !transform.IsScaleEnabled);
		if (PropertyInspector::InspectProperty("Pivot", transform.PivotPoint))
		{
			transform.GroupTransformation = false;
			transform.DirtyFlag = true;
		}
	}
	{
		ScopedDisable disabled(!transform.IsRotationEnabled);
		if (PropertyInspector::InspectProperty("Rotation", transform.AnglesRPY, transform.PreviousAnglesRPY, -180.0f, 180.0f))
		{
			transform.GroupTransformation = false;
			transform.DirtyFlag = true;
		}
	}
	{
		ScopedDisable disabled(!transform.IsScaleEnabled);
		if (PropertyInspector::InspectProperty("Scale", transform.Scale, transform.PreviousScale, 0.1f, 10.0f))
		{
			transform.GroupTransformation = false;
			transform.DirtyFlag = true;
		}
	}
}

void ar::ComponentInspector::InspectComponent(MeshComponent& mesh)
{
	ImGui::Text("Hey this is mesh");
}
