#ifndef __CLOCK_INCLUDED__
#define __CLOCK_INCLUDED__

#define USE_QUERYPERFORMANCECOUNTER

namespace GameEngine
{
	namespace Time
	{
		class Clock
		{
		private:
			double m_start = 0.0;
			double m_frameStart = 0.0;
			double m_lastFrameTime = 0.0;
			double m_pauseTime = 0.0;
			bool m_paused = false;

		public:
			Clock();

			void Reset();
			double TimeSinceStartUp();
			double DeltaT();
			void Pause();
			void Resume();
			double SystemTime();

			void StartFrame();
			void EndFrame();
		};
	}
}

#endif
