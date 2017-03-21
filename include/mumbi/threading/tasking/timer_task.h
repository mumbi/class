#pragma once

#ifndef MUMBI__THREADING__TASKING__UPDATE_TASK__H
#define MUMBI__THREADING__TASKING__UPDATE_TASK__H


#include <chrono>
#include <functional>

#include "task.h"

#include "../promise.h"
#include "../performable.h"

#include "../../time/updater.h"

namespace mumbi {
namespace threading {
namespace tasking
{
	using std::bind;
	using std::chrono::duration;
	using std::chrono::high_resolution_clock;
	using std::unique_ptr;
	using std::make_unique;

	using std::chrono::duration_cast;

	using mumbi::time::updater;

	class timer_task
		: public performable
	{
		template<class Promise>
		friend void set_timer_again(performer& performer, timer_task& timer, Promise& promise);
		
	private:
		class timer_task_invoker
			: public task_invoker<void()>
		{
		public:
			template<typename Callable>
			timer_task_invoker(Callable&& callable, bool immediately_update)
				: task_invoker<void()>(bind(&timer_task_invoker::on_elapsed, this))
				, _updater(make_unique<updater>(forward<Callable>(callable), immediately_update))
			{
			}

			template<typename Callable>
			timer_task_invoker(Callable&& callable, int fps, bool immediately_update)
				: task_invoker<void()>(bind(&timer_task_invoker::on_elapsed, this))
				, _updater(make_unique<updater>(forward<Callable>(callable), fps, immediately_update))
			{
			}

			template<typename Callable, typename Rep, typename Period>
			timer_task_invoker(Callable&& callable, const duration<Rep, Period>& d, bool immediately_update)
				: task_invoker<void()>(bind(&timer_task_invoker::on_elapsed, this))
				, _updater(make_unique<updater>(forward<Callable>(callable), d, immediately_update))
			{
			}

			bool stopped() const
			{
				return _updater->stopped();
			}

			shared_ptr<timer_task_invoker> clone()
			{
				return shared_ptr<timer_task_invoker>(new timer_task_invoker(forward<unique_ptr<updater>>(_updater)));
			}

		private:
			timer_task_invoker(unique_ptr<updater>&& i)
				: task_invoker<void()>(bind(&timer_task_invoker::on_elapsed, this))
				, _updater(forward<unique_ptr<updater>>(i))
			{
			}

			void on_elapsed()
			{
				_updater->update();
			}

		private:			
			unique_ptr<updater>	_updater;
		};

	public:
		template<typename Callable>
		explicit timer_task(Callable&& callable, bool immediately_update = false)
			: performable(make_shared<timer_task_invoker>(forward<Callable>(callable), immediately_update))
		{
		}

		template<typename Callable>
		timer_task(Callable&& callable, int fps, bool immediately_update = false)
			: performable(make_shared<timer_task_invoker>(forward<Callable>(callable), fps, immediately_update))
		{
		}

		template<typename Callable, typename Rep, typename Period>
		timer_task(Callable&& callable, const duration<Rep, Period>& d, bool immediately_update = false)
			: performable(make_shared<timer_task_invoker>(forward<Callable>(callable), d, immediately_update))
		{
		}

	private:
		timer_task(shared_ptr<timer_task_invoker> origin)
			: performable(origin->clone())
		{
		}

		timer_task clone()
		{
			auto invoker = get_invoker<timer_task_invoker>();
			return timer_task(invoker);
		}

		bool stopped() const
		{
			auto invoker = get_invoker<timer_task_invoker>();
			return invoker->stopped();
		}
	};

	template<class Promise>
	void set_timer_again(performer& performer, timer_task& timer, Promise& promise);

	template<typename Callable>
	void set_timer(performer& performer, Callable&& callable, bool immediately_update = false)
	{
		timer_task timer(forward<Callable>(callable), immediately_update);

		auto promise = make_promise(performer, [timer]() mutable
		{			
			timer.perform();
		});

		set_timer_again(performer, timer, promise);
	}

	template<typename Callable>
	void set_timer(performer& performer, Callable&& callable, int fps, bool immediately_update = false)
	{
		timer_task timer(forward<Callable>(callable), fps, immediately_update);

		auto promise = make_promise(performer, [timer]() mutable
		{
			timer.perform();
		});

		set_timer_again(performer, timer, promise);
	}

	template<typename Callable, typename Rep, typename Period>
	void set_timer(performer& performer, Callable&& callable, const duration<Rep, Period>& d, bool immediately_update = false)
	{
		timer_task timer(forward<Callable>(callable), d, immediately_update);

		auto promise = make_promise(performer, [timer]() mutable
		{
			timer.perform();
		});

		set_timer_again(performer, timer, promise);
	}	

	void set_timer(performer& performer, timer_task& timer)
	{
		auto promise = make_promise(performer, [timer]() mutable
		{
			timer.perform();
		});

		set_timer_again(performer, timer, promise);
	}

	template<class Promise>
	void set_timer_again(performer& performer, timer_task& timer, Promise& promise)
	{
		promise.then(performer, [&performer, timer]() mutable
		{
			timer_task new_timer = timer.clone();
			if (!new_timer.stopped())
				set_timer(performer, new_timer);
		});
	}
}}}

#endif	// MUMBI__THREADING__TASKING__UPDATE_TASK__H
