#ifndef __TIME_MANAGER_INCLUDED__
#define __TIME_MANAGER_INCLUDED__

namespace Time
{
	class TimeManager
	{
	private:
		static TimeManager* instance;

		double m_start = 0.0;
		double m_frameStart = 0.0;
		double m_lastFrameTime = 0.0;
		double m_pauseTime = 0.0;
		bool m_paused = false;

	public:
		TimeManager();

		static void Reset();
		static double TimeSinceStartUp();
		static double DeltaT();
		static void Pause();
		static void Resume();
		static double SystemTime();

		static void StartFrame();
		static void EndFrame();
	};
}

#endif