#include "acceptor.h"

#include <functional>
#include "../asio.h"

#include "session.h"
#include "session_impl.h"

#include "exception.h"

namespace mumbi {
namespace net
{	
	using std::make_shared;	
	using mumbi::exception::network_exception;

	class acceptor::impl		
	{
	public:
		error_occurred_signal_type	error_occurred;
		accepted_signal_type		accepted;

	public:
		impl(network_service& network_service, uint16_t port)			
			: _network_service(network_service)
			, _bind_endpoint(tcp::v6(), port)
		{
		}		
		
		void accept()
		{
			if (!_acceptor)
			{
				_acceptor = make_unique<tcp::acceptor>(get_io_service());
				_acceptor->open(_bind_endpoint.protocol());				

				asio::error_code error;
				_acceptor->bind(_bind_endpoint, error);
				if (error)
					throw network_exception(error.value(), error.message().c_str());

				_acceptor->listen();
			}			
			
			session accept_session(_network_service);			
			auto accept_endpoint = make_shared<tcp::endpoint>();
			
			_acceptor->async_accept(*accept_session.get_impl()->_socket, *accept_endpoint, std::bind(&impl::on_accepted, this, std::placeholders::_1, accept_session, accept_endpoint));			
		}		
		
		void on_accepted(const error_code& error, session& session, shared_ptr<tcp::endpoint> endpoint)		
		{			
			if (!error)
			{
				session.get_impl()->_is_connected = true;				
				
				accepted.emit(session, endpoint->address().to_string());				

				// session start.								
				session.get_impl()->receive();				

				// post accept again.				
				accept();
			}
			else
			{				
				error_occurred.emit(error);
			}
		}

		io_service& get_io_service()
		{
			return _network_service.get_impl().get_io_service();
		}

		network_service&			_network_service;
		unique_ptr<tcp::acceptor>	_acceptor;
		tcp::endpoint				_bind_endpoint;
	};	

	acceptor::acceptor(network_service& network_service, uint16_t port)
		: _pimpl(make_shared<impl>(network_service, port))
	{
		error_occurred.connect(&_pimpl->error_occurred);
		accepted.connect(&_pimpl->accepted);		
	}

	void acceptor::accept()
	{	
		_pimpl->accept();
	}

	void acceptor::close()
	{
		_pimpl->_acceptor->close();
		_pimpl->_acceptor = nullptr;		
	}
}}