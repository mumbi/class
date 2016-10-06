#include "acceptor.h"

#include <functional>
#include "../asio.h"

#include "session.h"
#include "session_impl.h"

namespace mumbi {
namespace net
{
	using std::exception;
	using std::make_shared;

	class acceptor::impl		
	{
	public:
		impl(network_service& network_service, uint16_t port)			
			: _network_service(network_service)
			, _bind_endpoint(tcp::v6(), port)
		{
		}

		bool accept(acceptor_ptr acceptor)
		{
			if (!_acceptor)
			{
				_acceptor = make_unique<tcp::acceptor>(get_io_service());
				_acceptor->open(_bind_endpoint.protocol());

				asio::error_code error;
				_acceptor->bind(_bind_endpoint, error);
				if (error)
				{
					//cerr <<  << endl;
					throw exception(error.message().c_str());
					return false;
				}

				_acceptor->listen();
			}
			
			session_ptr accept_session = session::create(_network_service);
			auto accept_endpoint = make_shared<tcp::endpoint>();

			_acceptor->async_accept(*accept_session->get_impl()._socket, *accept_endpoint, std::bind(&impl::on_accepted, acceptor, std::placeholders::_1, accept_session));

			return true;
		}

		static void on_accepted(acceptor_ptr acceptor, const error_code& error, session_ptr session)
		{
			if (!error)
			{
				session->get_impl()._is_connected = true;

				acceptor->accepted.emit(session);

				// session start.				
				session->get_impl().receive(session);

				// post accept again.
				acceptor->accept();
			}
			else
			{
				acceptor->error_occurred.emit(error);
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

	acceptor_ptr acceptor::create(network_service& network_service, uint16_t port)
	{
		return acceptor_ptr(new acceptor(network_service, port));
	}

	acceptor::~acceptor()
	{
	}	

	acceptor::acceptor(network_service& network_service, uint16_t port)
		: _pimpl(new impl(network_service, port))
	{
	}

	bool acceptor::accept()
	{
		return _pimpl->accept(shared_from_this());
	}

	void acceptor::close()
	{
		_pimpl->_acceptor = nullptr;
	}	
}}