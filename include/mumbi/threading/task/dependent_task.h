#pragma once

#ifndef MUMBI__THREADING__TASK__DEPENDENT_TASK__H
#define MUMBI__THREADING__TASK__DEPENDENT_TASK__H

#include "task.h"

namespace mumbi {
namespace threading {
namespace task
{
	using std::make_shared;
	using std::forward;
	using std::shared_ptr;

	template<typename R>
	using task_ptr = shared_ptr<task<R>>;

	template<typename R>
	class dependent_task
		: public task<R>
	{
	public:
		template<typename F>
		dependent_task(F&& f, task_ptr<R> dependent)
			: task(forward<F>(f))
			, _dependent(dependent)
		{
		}

	private:
		task_ptr<R>	_dependent;
	};

	template<typename F, typename D>
	auto make_dependent_task(F&& f, D&& d) -> dependent_task<decltype(f())>
	{		
		return make_task<dependent_task>(f, d);
	}

	template<typename F, typename D>
	auto make_shared_dependent_task(F&& f, D&& d)
	{
		return make_shared<decltype(make_dependent_task(f, d))>(forward<F>(f), forward<D>(d));
	}	
}}}

#endif	// MUMBI__THREADING__TASK__DEPENDENT_TASK__H