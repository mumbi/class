#pragma once

#ifndef MUMBI__NET__NETWORK_SERVICE_IMPL__H
#define MUMBI__NET__NETWORK_SERVICE_IMPL__H

#include <atomic>
#include <thread>
#include <chrono>

#include "../asio.h"

#include "../threading/thread_pool.h"
#include "../threading/queue_service.h"

namespace mumbi {
namespace net
{	
	using std::bind;
	using std::atomic;
	using std::this_thread::sleep_for;
	using std::chrono::microseconds;

	using asio::io_service;
	using mumbi::threading::queue_service;
	using mumbi::threading::thread_pool;	

	class network_service::impl
	{
		friend class network_service;
	public:
		impl(size_t thread_count)
			: _queue(thread_count)
			, _thread_pool(bind(&impl::run, this), thread_count)
			, _running(false)
		{			
		}

		impl()
			: _thread_pool(bind(&impl::run, this))
			, _running(false)
		{
		}

		void restart()		
		{
			if (!_queue.stopped())
				return;

			_queue.restart();			
		}

		void stop()
		{
			if (_queue.stopped())
				return;

			_queue.stop();			
		}

		void start_running()
		{
			_running = true;
			_thread_pool.start();
		}

		void end_running()
		{
			_running = false;
			_thread_pool.join();
		}

		void run()
		{
			while (_running)
			{
				_queue.run();

				sleep_for(microseconds(1));
			}
		}

		queue_service	_queue;
		thread_pool		_thread_pool;

		atomic<bool>	_running;

	public:	
		io_service& get_io_service()
		{
			return _queue.get_io_service();
		}
	};
}}

#endif	// MUMBI__NET__NETWORK_SERVICE_IMPL__H