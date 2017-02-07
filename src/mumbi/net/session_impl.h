#pragma once

#ifndef MUMBI__NET__SESSION_IMPL__H
#define MUMBI__NET__SESSION_IMPL__H

#include <memory>
#include <vector>

#include "../asio.h"
#include "network_service.h"
#include "network_service_impl.h"

#include "buffer.h"

namespace mumbi {
namespace net
{
	using std::error_code;	
	using std::make_unique;
	using std::enable_shared_from_this;	
	using asio::buffer;
	using asio::ip::tcp;
	using mumbi::io::make_buffer;

	class session;
	
	class acceptor;
	class connector;

	class session::impl
		: public enable_shared_from_this<session::impl>
	{
	private:
		static constexpr size_t buffer_size = 1024;

	public:
		friend class session;

		friend class acceptor;
		friend class connector;

	public:
		disconnected_signal_type	disconnected;
		received_signal_type		received;
		sent_signal_type			sent;
		
		~impl()
		{
			close();
		}

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
		
		void receive()
		{
			buffer_ptr receive_buffer = make_buffer(buffer_size);			
			_socket->async_receive(buffer(receive_buffer->data(), receive_buffer->size()), bind(&impl::on_received, this, std::placeholders::_1, std::placeholders::_2, receive_buffer, shared_from_this()));
		}		
		
		void send(buffer_ptr send_buffer)
		{
			_socket->async_send(buffer(send_buffer->data(), send_buffer->size()), bind(&impl::on_sent, this, std::placeholders::_1, std::placeholders::_2, shared_from_this()));
		}
		
		void on_received(const error_code& error, size_t bytes_transferred, buffer_ptr buffer, shared_ptr<session::impl> session_impl)
		{			
			session session(session_impl);

			if (0 == bytes_transferred)
			{
				close();

				disconnected.emit(session, error);

				return;
			}			

			buffer->resize(bytes_transferred);			

			received.emit(session, error, buffer);

			receive();
		}		
		
		void on_sent(const error_code& error, size_t bytes_transferred, shared_ptr<session::impl> session_impl)
		{
			session session(session_impl);

			sent.emit(session, error, bytes_transferred);
		}

		unique_ptr<tcp::socket>	_socket;
		bool					_is_connected;
	};
}}

#endif	// MUMBI__NET__SESSION_IMPL__H
