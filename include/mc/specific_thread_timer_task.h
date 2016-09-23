#pragma once

#ifndef CONE_TIMER_TASK__H
#define CONE_TIMER_TASK__H

#include "timer_task.h"

namespace mumbi {
namespace threading {

	class specific_thread_performer;

namespace task
{
	class specific_thread_timer_task
		: public timer_task
	{
		using on_timed_type = function<void(void)>;

	public:
		template<typename F>
		specific_thread_timer_task(any_thread_performer& any_thread_performer, specific_thread_performer& specific_thread_performer, F&& f, clock_type::time_point until)
			: timer_task(any_thread_performer, bind(&cone_timer_task::cone, this), until)
			, _specific_thread_performer(specific_thread_performer)
			, _on_timed(forward<F>(f))
		{
		}

	private:
		void cone();

	private:
		specific_thread_performer&	_specific_thread_performer;
		on_timed_type				_on_timed;
	};
}}}

#endif	// CONE_TIMER_TASK__H
