#pragma once

#include <functional>
#include <time.h>
#include <stdexcept>

template <typename FUNC_T>
class profiler {
public:
	profiler(FUNC_T function) :
		m_function(function) {}

	template <typename... ARGS_T>
	void start(ARGS_T... args) {
		start_timer();
		m_function(args...);
		stop_timer();
	}

	double realtime_ms() {
		double time_s = m_realtime_end.tv_sec - m_realtime_begin.tv_sec;
		double time_ns = m_realtime_end.tv_nsec - m_realtime_begin.tv_nsec;
		return time_s * 1000.0 + time_ns / 1000000.0;
	}

	double cputime_ms() {
		double time_s = m_cputime_end.tv_sec - m_cputime_begin.tv_sec;
		double time_ns = m_cputime_end.tv_nsec - m_cputime_begin.tv_nsec;
		return time_s * 1000.0 + time_ns / 1000000.0;
	}

private:
	static const long ns_per_ms = 1000000;
	static const long ms_per_s = 1000;

	void start_timer() {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_cputime_begin);
		clock_gettime(CLOCK_REALTIME, &m_realtime_begin);
	}

	void stop_timer() {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_cputime_end);
		clock_gettime(CLOCK_REALTIME, &m_realtime_end);
	}

	FUNC_T m_function;

	timespec m_cputime_begin;
	timespec m_cputime_end;

	timespec m_realtime_begin;
	timespec m_realtime_end;
};

template <typename FUNC_T>
profiler<FUNC_T> make_profiler(FUNC_T function) {
	return profiler<FUNC_T>(function);
}

