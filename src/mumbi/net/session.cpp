#include "session.h"

#include "session_impl.h"

namespace mumbi {
namespace net
{
	session_ptr session::create(network_service& network_service)
	{
		session_ptr create_session = session_ptr(new session(network_service));

		return create_session;
	}

	session::session(network_service& network_service)
		: _pimpl(new impl(network_service))
	{
	}

	string session::local_endpoint() const
	{
		return _pimpl->_socket->local_endpoint().address().to_string();
	}

	string session::remote_endpoint() const
	{
		return _pimpl->_socket->remote_endpoint().address().to_string();
	}
	
	bool session::is_connected() const
	{
		return _pimpl->_is_connected;
	}

	void session::disconnect()
	{
		_pimpl->_socket->shutdown(tcp::socket::shutdown_both);
	}

	void session::send(buffer_ptr buffer)
	{
		_pimpl->send(shared_from_this(), buffer);
	}

	session::impl& session::get_impl()
	{
		return *_pimpl;
	}
}}