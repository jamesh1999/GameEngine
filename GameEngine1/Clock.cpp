#include "Clock.h"

#ifdef USE_QUERYPERFORMANCECOUNTER
#include <Windows.h>
#else
#include <chrono>
#endif

using namespace GameEngine::Time;

Clock::Clock()
{
	Reset();
}

void Clock::Reset()
{
	m_start = SystemTime();
}

double Clock::TimeSinceStartUp()
{
	return SystemTime() - m_start;
}

double Clock::DeltaT() const
{
	if (m_paused) return 0.0;

	return m_lastFrameTime;
}

void Clock::Pause()
{
	m_paused = true;
	m_frameStart = SystemTime();
}

void Clock::Resume()
{
	m_paused = false;
	m_pauseTime += SystemTime() - m_frameStart;
	m_frameStart = SystemTime();
}

void Clock::StartFrame()
{
	m_frameStart = SystemTime();
}

void Clock::EndFrame()
{
	m_lastFrameTime = SystemTime() - m_frameStart;
}

double Clock::SystemTime()
{
#ifdef USE_QUERYPERFORMANCECOUNTER
	LARGE_INTEGER seconds;
	QueryPerformanceCounter(&seconds);
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	return static_cast<double>(seconds.QuadPart) / freq.QuadPart;
#else
	auto t = std::chrono::high_resolution_clock::now();;
	return std::chrono::time_point_cast<std::chrono::nanoseconds>(t).time_since_epoch().count() / 1000000000.0;
#endif
}
