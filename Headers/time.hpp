#ifndef WF_UTP_TIME_H
#define WF_UTP_TIME_H

#include <sstream>
#include <chrono>
#include <ctime>

	std::string GetNowTime()
	{
		//Get current time
		std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		//create stream object
		std::stringstream timestream;
		//load time to stream
		timestream << std::ctime(&now_time);
		//move data to passable object
		std::string timestring = timestream.str();
		if (timestring.back() == '\n')
		{
			timestring.pop_back();
		}
		return timestring;
	}

	std::time_t GetNowTime_t()
	{
		//Get current time
		std::time_t now_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		return now_time;
	}

	std::chrono::_V2::system_clock::time_point GetNowTime_v2()
	{
		return std::chrono::high_resolution_clock::now();
		//auto begin = std::chrono::high_resolution_clock::now();
		//auto end = std::chrono::high_resolution_clock::now();

		//std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns" << std::endl;
	}

	class time_machine
	{
	public:
		static std::chrono::_V2::system_clock::time_point last_time;
		//time_machine() :last_time(std::chrono::high_resolution_clock::now()) {}

		// start the timer running, clearing any prior time points and creating a new reference time point
		static void Start()
		{
			last_time = std::chrono::high_resolution_clock::now();
		}

		// start the timer running, clearing any prior time points and creating a new reference time point
		static void Reset()
		{
			last_time = std::chrono::high_resolution_clock::now();
		}
		
		// return the duration in milliseconds since the last Start, Reset, or Lap. Time point of read is pushed to last_time, previous last_time is discarded
		static long long Lap()
		{
			//get the time NOW
			std::chrono::_V2::system_clock::time_point now_time = std::chrono::high_resolution_clock::now();
			
			//measure the difference between now and last time, cast to milliseconds
			//std::chrono::milliseconds lap_time = std::chrono::duration_cast<std::chrono::milliseconds>(now_time - last_time).count();
			long long lap_time = std::chrono::duration_cast<std::chrono::milliseconds>(now_time - last_time).count();

			//now time becomes last_time for next call
			last_time = now_time;

			return lap_time;
		}

		//gives the duration since the last update of the recorded time, but does NOT change the read-from time
		static long long Read()
		{
			//get the time NOW
			std::chrono::_V2::system_clock::time_point now_time = std::chrono::high_resolution_clock::now();

			//measure the difference between now and last time, cast to milliseconds
			return std::chrono::duration_cast<std::chrono::milliseconds>(now_time - last_time).count();
		}

		static std::chrono::_V2::system_clock::time_point Source()
		{
			return last_time;
		}
	};

	class non_static_time_machine
	{
	public:
		std::chrono::_V2::system_clock::time_point last_time;
		//default constructor
		non_static_time_machine()
		{
			Start();
		}

		non_static_time_machine(const std::chrono::_V2::system_clock::time_point& source) :last_time(source) {}

		//copy cons
		non_static_time_machine(const non_static_time_machine& source) :last_time(source.last_time) {}
		//"move" cons
		non_static_time_machine(non_static_time_machine&& source) :last_time(source.last_time)
		{
			source.Reset();
		}
		~non_static_time_machine() {}
		//copy assign op
		non_static_time_machine& operator =(const non_static_time_machine& source)
		{
			if (&source == this)
			{
				return *this;
			}
			last_time = source.last_time;
			return *this;
		}
		
		/*
		//"move" assign op
		non_static_time_machine&& operator =(non_static_time_machine& source)
		{
			if (&source == this)
			{
				return *this;
			}
			last_time = source.last_time;
			source.Reset();
			return *this;
		}
		*/
			

		// start the timer running, clearing any prior time points and creating a new reference time point
		void Start()
		{
			last_time = std::chrono::high_resolution_clock::now();
		}

		// start the timer running, clearing any prior time points and creating a new reference time point
		void Reset()
		{
			last_time = std::chrono::high_resolution_clock::now();
		}

		// return the duration in milliseconds since the last Start, Reset, or Lap. Time point of read is pushed to last_time, previous last_time is discarded
		long long Lap()
		{
			//get the time NOW
			std::chrono::_V2::system_clock::time_point now_time = std::chrono::high_resolution_clock::now();

			//measure the difference between now and last time, cast to milliseconds
			//std::chrono::milliseconds lap_time = std::chrono::duration_cast<std::chrono::milliseconds>(now_time - last_time).count();
			long long lap_time = std::chrono::duration_cast<std::chrono::milliseconds>(now_time - last_time).count();

			//now time becomes last_time for next call
			last_time = now_time;

			return lap_time;
		}

		//gives the duration since the last update of the recorded time, but does NOT change the read-from time
		long long Read()
		{
			//get the time NOW
			std::chrono::_V2::system_clock::time_point now_time = std::chrono::high_resolution_clock::now();

			//measure the difference between now and last time, cast to milliseconds
			return std::chrono::duration_cast<std::chrono::milliseconds>(now_time - last_time).count();
		}

	
		// Synchronize a time machine to another time machine or measure from an existing time point
		void Synch(const std::chrono::_V2::system_clock::time_point& source)
		{
			last_time = source;
		}
		void Synch(const non_static_time_machine& source)
		{
			last_time = source.last_time;
		}

		std::chrono::_V2::system_clock::time_point Source()
		{
			return last_time;
		}
	};

	//initialize the static member variable of time_machine
	std::chrono::_V2::system_clock::time_point time_machine::last_time = std::chrono::high_resolution_clock::now();
	/*
	class pulsar
	{
	public:
		int period;
		std::chrono::duration units;
		bool& blinker;
		std::chrono::_V2::system_clock::time_point last_time;

		pulsar(int i_period, bool& i_blinker, std::chrono::duration i_units = std::chrono::milliseconds) : period(i_period), blinker(i_blinker), units(i_units) {}

		void Wait()
		{
			//start counting
			blinker = false;
			last_time = std::chrono::high_resolution_clock::now();
			while (blinker == false)
			{
				auto new_time = std::chrono::high_resolution_clock::now();
				if (period < std::chrono::duration_cast<units>(new_time - last_time).count())
				{
					last_time = new_time;
					blinker = true;
				}

				//optional wait of some sort here
			}
		}
	};

	*/
#endif
