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
		: _impl(make_shared<impl>(network_service))
	{
		disconnected.connect(&_impl->disconnected);
		received.connect(&_impl->received);
		sent.connect(&_impl->sent);
	}

	bool session::is_vaild() const
	{
		return static_cast<bool>(_impl);
	}

	string session::local_endpoint() const
	{
		if (!_impl)
			throw invalid_object_exception("invalid session.");

		error_code ec;
		return _impl->_socket->local_endpoint().address().to_string(ec);
	}

	string session::remote_endpoint() const
	{
		if (!_impl)
			throw invalid_object_exception("invalid session.");	

		error_code ec;
		return _impl->_socket->remote_endpoint().address().to_string(ec);;
	}
	
	bool session::is_connected() const
	{
		if (!_impl)
			throw invalid_object_exception("invalid session.");

		return _impl->_is_connected;
	}

	bool session::disconnect()
	{
		if (!_impl)
			throw invalid_object_exception("invalid session.");

		error_code error;
		_impl->_socket->shutdown(tcp::socket::shutdown_both, error);

		return !error;
	}
	
	void session::send(const buffer& buffer)
	{
		if (!_impl)
			throw invalid_object_exception("invalid session.");

		_impl->send(*this, buffer);
	}	
	
	void session::on_received(const error_code& error, size_t bytes_transferred, buffer& buffer, session& session)
	{
		if (0 == bytes_transferred)
		{
			session.get_impl()->close();

			session.get_impl()->disconnected.emit(session, error);

			return;
		}
		
		buffer.data().resize(bytes_transferred);

		session.get_impl()->received.emit(session, error, buffer);

		session.get_impl()->receive(session);
	}
	
	void session::on_sent(const error_code& error, size_t bytes_transferred, session& session, buffer& buffer)
	{
		session.get_impl()->on_sent(error, bytes_transferred, session, buffer);

		session.get_impl()->sent.emit(session, error, bytes_transferred);		
	}

	void session::on_sent_buffers(const error_code& error, size_t bytes_transferred, session& session, vector<buffer>& buffers)
	{
		session.get_impl()->on_sent_buffers(error, bytes_transferred, session, buffers);

		session.get_impl()->sent.emit(session, error, bytes_transferred);
	}

	shared_ptr<session::impl> session::get_impl()
	{
		return _impl;
	}
}}