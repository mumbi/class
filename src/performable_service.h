#pragma once

#ifndef PERFORMABLE_SERVICE__H
#define PERFORMABLE_SERVICE__H

#include <chrono>
#include <mutex>

#include "performable_queue.h"
#include "thread_pool.h"

#include "event_signal.h"

namespace mumbi {
namespace threading
{
	using std::mutex;
	using std::bind;
	using std::lock_guard;
	using std::chrono::high_resolution_clock;

	class performable_service
	{
		using lock_type = mutex;
	public:
		using clock_type = high_resolution_clock;

	public:
		~performable_service()
		{	
			_queue.post_finished.clear();

			stop();
		}

		performable_service()
			: _thread_pool(bind(&performable_queue::run, &_queue))
		{
			set_event_handlers();

			start();
		}

		performable_service(int thread_count)
			: _thread_pool(bind(&performable_queue::run, &_queue), thread_count)
		{
			set_event_handlers();

			start();
		}

		bool stopped() const
		{
			return _queue.stopped();
		}

		void start()
		{
			if (!_queue.stopped())
				return;

			_queue.start();
			_thread_pool.start();
		}

		void stop()
		{
			if (_queue.stopped())
				return;

			_queue.stop();
			_thread_pool.stop();
		}		

		void post(performable_interface_ptr performable)
		{
			_queue.post(performable);
		}

		bool busy() const
		{
			return !stopped() && _queue.post_count() >= _thread_pool.thread_count();
		}

		size_t post_count() const
		{
			return _queue.post_count();
		}

		size_t working_count() const
		{
			return _queue.working_count();
		}

		const performable_service::clock_type::time_point& idle_start_time() const
		{
			lock_guard<lock_type>	guard(_idle_start_time_lock);

			return _idle_start_time;
		}

	private:
		void on_post_finished(const std::thread::id& /*thread_id*/)
		{
			if (_queue.post_count() <= 0 && _queue.working_count() <= 0)
			{
				lock_guard<lock_type>	guard(_idle_start_time_lock);

				_idle_start_time = clock_type::now();
			}
		}		

		void set_event_handlers()
		{	
			_queue.post_finished += bind(&performable_service::on_post_finished, this, std::placeholders::_1);
		}

	private:
		performable_queue		_queue;
		thread_pool				_thread_pool;

		mutable lock_type		_idle_start_time_lock;
		clock_type::time_point	_idle_start_time;
	};
}}

#endif	// PERFORMABLE_SERVICE__H