#include "connector.h"

#include "../asio.h"

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
		error_occurred_signal_type	error_occurred;
		connecting_signal_type		connecting;
		connected_signal_type		connected;

	public:
		impl(network_service& network_service)			
			: _network_service(network_service)
			, _resolver(get_io_service())
		{
		}						
		
		void connect(tcp::resolver::iterator endpoint_iterator)
		{
			tcp::endpoint endpoint = *endpoint_iterator;
			
			++endpoint_iterator;
			
			session session(_network_service);			
			session.get_impl()->_socket->async_connect(endpoint, bind(&impl::on_connected, this, std::placeholders::_1, endpoint_iterator, session));			
			
			error_code ec;
			connecting.emit(endpoint.address().to_string(ec));
		}		
		
		void on_resolved(const error_code& error, tcp::resolver::iterator endpoint_iterator)
		{
			if (!error)
			{
				connect(endpoint_iterator);
			}
			else
			{
				error_occurred.emit(error);
			}
		}		
		
		void on_connected(const error_code& error, tcp::resolver::iterator next_endpoint_iterator, session& session)		
		{
			if (!error)
			{
				session.get_impl()->_is_connected = true;
				
				connected.emit(session);

				// session start.				
				session.get_impl()->receive(session);				
			}
			else if (tcp::resolver::iterator() != next_endpoint_iterator)
			{
				session.get_impl()->close();
				
				connect(next_endpoint_iterator);
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
		
		network_service&	_network_service;
		tcp::resolver		_resolver;
	};	
	
	connector::connector(network_service& network_service)
		: _impl(new impl( network_service))
	{
		error_occurred.connect(&_impl->error_occurred);
		connecting.connect(&_impl->connecting);
		connected.connect(&_impl->connected);
	}

	void connector::connect(const string& host, uint16_t port)
	{
		stringstream ss;
		ss << port;

		tcp::resolver::query query(host, ss.str());				
		_impl->_resolver.async_resolve(query, bind(&connector::impl::on_resolved, _impl.get(), std::placeholders::_1, std::placeholders::_2));
	}	
}}