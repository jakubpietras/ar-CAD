#pragma once
#include <chrono>

namespace ar
{
	class Timer {
	public:
		Timer();
		void Update();
		inline const float GetDeltaTime() const { return m_DeltaTime; }
		inline const float GetFPS() const { return m_Fps; }
	private:
		std::chrono::high_resolution_clock::time_point m_StartTime, m_LastTime;
		float m_DeltaTime, m_ElapsedTime, m_Fps;
		int m_FrameCount;
	};
}
