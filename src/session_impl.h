#pragma once

#ifndef NET__SESSION_IMPL__H
#define NET__SESSION_IMPL__H

#include <memory>
#include <vector>

#include "asio.h"
#include "network_service.h"
#include "network_service_impl.h"

#include "buffer.h"

namespace mumbi {
namespace net
{
	using std::error_code;	
	using std::make_unique;
	using asio::buffer;
	using asio::ip::tcp;
	using mumbi::io::make_buffer;

	class session;
	
	class acceptor;
	class connector;

	class session::impl
	{
		static constexpr size_t buffer_size = 1024;

	public:
		friend class session;

		friend class acceptor;
		friend class connector;		
		
		impl(network_service& network_service)
			: _socket(make_unique<tcp::socket>(network_service.get_impl().get_io_service()))
			, _is_connected(false)
		{
		}

		void close()
		{
			_is_connected = false;
			_socket->close();
		}

		void receive(session_ptr session)
		{
			buffer_ptr receive_buffer = make_buffer(buffer_size);			
			_socket->async_receive(buffer(receive_buffer->data(), receive_buffer->size()), bind(&impl::on_received, std::placeholders::_1, std::placeholders::_2, receive_buffer, session));
		}

		void send(session_ptr session, buffer_ptr send_buffer)
		{
			_socket->async_send(buffer(send_buffer->data(), send_buffer->size()), bind(&impl::on_sent, std::placeholders::_1, std::placeholders::_2, session));
		}		

		static void on_received(const error_code& error, size_t bytes_transferred, buffer_ptr buffer, session_ptr session)
		{
			if (0 == bytes_transferred)
			{	
				session->get_impl().close();
				session->disconnected.emit(session, error);
				return;
			}			

			buffer->resize(bytes_transferred);			

			session->received.emit(session, error, buffer);

			session->get_impl().receive(session);
		}

		static void on_sent(const error_code& error, size_t bytes_transferred, session_ptr session)
		{
			session->sent.emit(session, error, bytes_transferred);
		}

		unique_ptr<tcp::socket>	_socket;
		bool					_is_connected;
	};
}}

#endif	// NET__SESSION_IMPL__H
