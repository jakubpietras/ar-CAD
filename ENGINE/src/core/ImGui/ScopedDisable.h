#pragma once
#include <imgui.h>

namespace ar
{
	class ScopedDisable
	{
	public:
		explicit ScopedDisable(bool should_disable)
		{
			if (should_disable)
			{
				ImGui::BeginDisabled();
				m_Disabled = true;
			}
		}

		~ScopedDisable()
		{
			if (m_Disabled)
			{
				ImGui::EndDisabled();
			}
		}

		ScopedDisable(const ScopedDisable&) = delete;
		ScopedDisable& operator=(const ScopedDisable&) = delete;

	private:
		bool m_Disabled = false;

	};
}