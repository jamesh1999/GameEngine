#include "TimeManager.h"
#include <chrono>

using namespace Time;

TimeManager* TimeManager::instance = nullptr;

TimeManager::TimeManager()
{
	instance = this;
	Reset();
}

void TimeManager::Reset()
{
	instance->m_start = SystemTime();
}

double TimeManager::TimeSinceStartUp()
{
	return SystemTime() - instance->m_start;
}

double TimeManager::DeltaT()
{
	if (instance->m_paused) return 0.0;

	return instance->m_lastFrameTime;
}

void TimeManager::Pause()
{
	instance->m_paused = true;
	instance->m_frameStart = SystemTime();
}

void TimeManager::Resume()
{
	instance->m_paused = false;
	instance->m_pauseTime += SystemTime() - instance->m_frameStart;
	instance->m_frameStart = SystemTime();
}

void TimeManager::StartFrame()
{
	instance->m_frameStart = SystemTime();
}

void TimeManager::EndFrame()
{
	instance->m_lastFrameTime = SystemTime() - instance->m_frameStart;
}

double TimeManager::SystemTime()
{
	auto t = std::chrono::high_resolution_clock::now();;
	return std::chrono::time_point_cast<std::chrono::nanoseconds>(t).time_since_epoch().count() / 1000000000.0;
}
