#include "session.h"

#include "session_impl.h"
#include "exception.h"

namespace mumbi {
namespace net
{
	using std::make_shared;
	using mumbi::exception::invalid_object_exception;

	session::session()
	{
	}

	session::session(network_service& network_service)
		: _pimpl(make_shared<impl>(network_service))
	{
		disconnected.connect(&_pimpl->disconnected);
		received.connect(&_pimpl->received);
		sent.connect(&_pimpl->sent);
	}

	session::session(shared_ptr<impl> pimpl)
		: _pimpl(pimpl)
	{
		disconnected.connect(&_pimpl->disconnected);
		received.connect(&_pimpl->received);
		sent.connect(&_pimpl->sent);
	}

	bool session::is_vaild() const
	{
		return static_cast<bool>(_pimpl);
	}

	string session::local_endpoint() const
	{
		if (!_pimpl)
			throw invalid_object_exception("invalid session.");

		return _pimpl->_socket->local_endpoint().address().to_string();
	}

	string session::remote_endpoint() const
	{
		if (!_pimpl)
			throw invalid_object_exception("invalid session.");

		return _pimpl->_socket->remote_endpoint().address().to_string();
	}
	
	bool session::is_connected() const
	{
		if (!_pimpl)
			throw invalid_object_exception("invalid session.");

		return _pimpl->_is_connected;
	}

	bool session::disconnect()
	{
		if (!_pimpl)
			throw invalid_object_exception("invalid session.");

		asio::error_code error;
		_pimpl->_socket->shutdown(tcp::socket::shutdown_both, error);

		return !error;
	}

	void session::send(buffer_ptr buffer)
	{
		if (!_pimpl)
			throw invalid_object_exception("invalid session.");

		_pimpl->send(buffer);
	}

	shared_ptr<session::impl> session::get_impl()
	{
		return _pimpl;
	}
}}