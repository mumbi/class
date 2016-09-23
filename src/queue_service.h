#pragma once

#ifndef QUEUE_SERVICE__H
#define QUEUE_SERVICE__H

#include <memory>
#include "asio.h"

namespace mumbi
{
	using std::unique_ptr;
	using asio::io_service;

	class queue_service
	{
	public:
		virtual ~queue_service();
		queue_service();

		bool stopped() const;
		void start();
		void stop();

		void run();
	
		io_service& get_io_service();

	private:
		io_service						_io_service;
		unique_ptr<io_service::work>	_work;
	};
}

#endif	// QUEUE_SERVICE__H
