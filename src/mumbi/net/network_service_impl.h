#pragma once

#ifndef MUMBI__NET__NETWORK_SERVICE_IMPL__H
#define MUMBI__NET__NETWORK_SERVICE_IMPL__H

#include "../asio.h"

#include "../threading/thread_pool.h"
#include "../threading/queue_service.h"

namespace mumbi {
namespace net
{	
	using std::bind;
	using asio::io_service;
	using mumbi::threading::queue_service;
	using mumbi::threading::thread_pool;	

	class network_service::impl
	{
		friend class network_service;

		impl(size_t /*thread_count*/)
			: _thread_pool(bind(&queue_service::run, &_queue))
		{			
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

		queue_service	_queue;
		thread_pool		_thread_pool;

	public:	
		io_service& get_io_service()
		{
			return _queue.get_io_service();
		}
	};
}}

#endif	// MUMBI__NET__NETWORK_SERVICE_IMPL__H