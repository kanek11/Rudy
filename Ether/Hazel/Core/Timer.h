#pragma once

#include <chrono>  

//chrono is a std library for time use namespace std::chrono

//concepts:
// epoch:  a point in time from which time is measured
// 
//time_point: a point in time sequence,  represented by a duration since epoch 



//a tick:  the smallest possible time unit of a clock   , different clocks may have different tick periods ,i.e. precision 
// eg: 1 second, 1 millisecond, 1 microsecond, 1 nanosecond


//duration: the time span;
//time_point. time_since_epoch()  : returns the duration.
//.count()  : returns the number of ticks of the duration.  


//clock: a source of time
//steady_clock : a clock that goes forward at a steady rate
//high_resolution_clock: a clock with the shortest tick period available  


//routine:
//declare a time_point,  given a _clock
//eg: std::chrono::time_point<std::chrono::steady_clock> start;
// 
//_clock::now() : returns a time_point representing the current point in time

//use. time_since_epoch() if you want to know, kinda relative time to the epoch, in a whole time sequence
//eg: in a large project,  keep track when the start time.

//subtract two time_points,  returns a duration,  if you want to know the elpased time between two time_points
//eg: measure CPU time.
 
namespace Hazel {

	class Timer
	{
	public:
		Timer()
		{
			Reset();
			m_Stopped = false;

		}
		
		~Timer()
		{
			if (m_Stopped == false)
			Stop();
		}

		void  Reset()
		{
			m_Start = std::chrono::high_resolution_clock::now();
		}

		float  Stop()
		{
			
		   auto endTime = std::chrono::high_resolution_clock::now();

		   //nano seconds
           auto end = std::chrono::time_point_cast<std::chrono::nanoseconds>(endTime).time_since_epoch().count();
           auto start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_Start).time_since_epoch().count();
		 
		   auto duration = end - start;
            m_Stopped = true;

			//nano = 10^-9,  micro = 10^-6, milli = 10^-3
		    return  duration * 0.001f * 0.001f * 0.001f;
		
		
		}

		float  Elapsed()
		{
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_Start).count() * 0.001f * 0.001f * 0.001f;
		}

		float  ElapsedMs()
		{
			return Elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_Start;
		bool m_Stopped = true;
	};

}
