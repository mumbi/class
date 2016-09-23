#pragma once

#ifndef TIMER_TASK__H
#define TIMER_TASK__H

#include "task.h"
#include <chrono>

namespace mumbi {
namespace threading {

	class any_thread_performer;

namespace task
{
	using std::enable_shared_from_this;
	using std::chrono::high_resolution_clock;

	class timer_task
		: public task<void>
		, public enable_shared_from_this<timer_task>
	{
	private:
		using on_timed_type = function<void()>;		

	public:	
		using clock_type = high_resolution_clock;

		template<typename F>
		timer_task(any_thread_performer& performer, F&& f, const clock_type::time_point& until)
			: task(bind(&timer_task::wait, this))
			, _performer(performer)
			, _on_timed(forward<F>(f))
			, _until(until)
		{
		}
	
		void wait();

	private:
		any_thread_performer&			_performer;
		on_timed_type					_on_timed;

		const clock_type::time_point	_until;
	};
}}}

#endif	// TIMER_TASK__H