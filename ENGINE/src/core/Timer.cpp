#include "arpch.h"
#include "Timer.h"

namespace ar
{
	Timer::Timer()
		: m_DeltaTime(0.0f), m_Fps(0.0f), m_FrameCount(0), m_ElapsedTime(0.0f)
	{
		m_LastTime = std::chrono::high_resolution_clock::now();
	}

	void Timer::Update()
	{
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> frameDuration = currentTime - m_LastTime;
        m_DeltaTime = frameDuration.count();
        m_LastTime = currentTime;

        m_FrameCount++;
        m_ElapsedTime += m_DeltaTime;

        if (m_ElapsedTime >= 1.0f / 2)
        {
            m_Fps = static_cast<float>(m_FrameCount) / m_ElapsedTime;
            m_FrameCount = 0;
            m_ElapsedTime = 0.0f;
        }
	}

}