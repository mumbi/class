#include "connector.h"

#include "asio.h"

#include "network_service.h"
#include "network_service_impl.h"

#include "session.h"
#include "session_impl.h"



namespace mumbi {
namespace net
{
	using std::stringstream;

	class connector::impl		
	{
	public:
		impl(network_service& network_service)			
			: _network_service(network_service)
			, _resolver(get_io_service())
		{
		}
		
		static void connect(connector_ptr connector, session_ptr session, tcp::resolver::iterator endpoint_iterator)
		{
			tcp::endpoint endpoint = *endpoint_iterator;
			
			++endpoint_iterator;

			session->get_impl()._socket->async_connect(endpoint, bind(&impl::on_connected, connector, std::placeholders::_1, endpoint_iterator, session));
			
			connector->connecting.emit(endpoint.address().to_string());
		}

		static void on_resolved(connector_ptr connector, const error_code& error, tcp::resolver::iterator endpoint_iterator)
		{
			if (!error)
			{
				connector::impl& impl = *connector->_pimpl;
				session_ptr session = session::create(impl._network_service);
				impl.connect(connector, session, endpoint_iterator);
			}
			else
			{
				connector->error_occurred.emit(error);
			}
		}

		static void on_connected(connector_ptr connector, const error_code& error, tcp::resolver::iterator next_endpoint_iterator, session_ptr session)
		{
			if (!error)
			{
				session->get_impl()._is_connected = true;

				connector->connected.emit(session);

				// session start.				
				session->get_impl().receive(session);
			}
			else if (tcp::resolver::iterator() != next_endpoint_iterator)
			{
				session->get_impl().close();
				connect(connector, session, next_endpoint_iterator);
			}
			else
			{
				connector->error_occurred.emit(error);
			}
		}

		io_service& get_io_service()
		{
			return _network_service.get_impl().get_io_service();
		}
		
		network_service&	_network_service;
		tcp::resolver		_resolver;
	};
	
	connector_ptr connector::create(network_service& network_service)
	{
		return connector_ptr(new connector(network_service));
	}

	connector::~connector()
	{	
	}	

	connector::connector(network_service& network_service)
		: _pimpl(new impl( network_service))
	{
	}

	void connector::connect(const string& host, uint16_t port)
	{
		stringstream ss;
		ss << port;

		tcp::resolver::query query(host, ss.str());

		_pimpl->_resolver.async_resolve(query, bind(&connector::impl::on_resolved, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}	
}}