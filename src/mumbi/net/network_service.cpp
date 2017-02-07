#include "network_service.h"

#include "network_service_impl.h"

namespace mumbi {
namespace net
{
	using std::make_unique;

	network_service::~network_service()
	{
		stop();

		end_running();
	}

	network_service::network_service()
		: _pimpl(make_unique<impl>(0))
	{
		restart();
	}

	network_service::network_service(size_t thread_count)
		: _pimpl(make_unique<impl>(thread_count))
	{
		restart();
	}

	bool network_service::stopped() const
	{
		return _pimpl->_queue.stopped();
	}

	void network_service::restart()
	{
		_pimpl->restart();		
	}

	void network_service::stop()
	{
		_pimpl->stop();
	}

	void network_service::start_running()
	{
		_pimpl->start_running();
	}

	void network_service::end_running()
	{
		_pimpl->end_running();
	}	

	network_service::impl& network_service::get_impl()
	{
		return *_pimpl;
	}
}}