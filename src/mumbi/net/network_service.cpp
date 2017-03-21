#include "network_service.h"

#include <thread>

#include "network_service_impl.h"

namespace mumbi {
namespace net
{
	using std::make_unique;
	using std::thread;

	network_service::~network_service()
	{
		stop();

		end_running();
	}

	network_service::network_service()
		: network_service(thread::hardware_concurrency())
	{	
	}

	network_service::network_service(size_t thread_count)
		: _impl(make_unique<impl>(thread_count))
	{
		restart();
	}

	bool network_service::stopped() const
	{
		return _impl->_queue.stopped();
	}

	void network_service::restart()
	{
		_impl->restart();
	}

	void network_service::stop()
	{
		_impl->stop();
	}

	void network_service::start_running()
	{
		_impl->start_running();
	}

	void network_service::end_running()
	{
		_impl->end_running();
	}	

	network_service::impl& network_service::get_impl()
	{
		return *_impl;
	}
}}