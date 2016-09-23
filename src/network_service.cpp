#include "network_service.h"

#include "network_service_impl.h"

namespace mumbi {
namespace net
{
	network_service::~network_service()
	{
		stop();
	}

	network_service::network_service()
		: _pimpl(new impl(0))
	{
		start();
	}

	network_service::network_service(size_t thread_count)
		: _pimpl(new impl(thread_count))
	{
		start();
	}

	void network_service::start()
	{
		_pimpl->start();		
	}

	void network_service::stop()
	{
		_pimpl->stop();
	}

	network_service::impl& network_service::get_impl()
	{
		return *_pimpl;
	}
}}