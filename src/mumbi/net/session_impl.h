#pragma once

#ifndef MUMBI__NET__SESSION_IMPL__H
#define MUMBI__NET__SESSION_IMPL__H

#include <memory>
#include <vector>

#include <concurrentqueue.h>

#include "../asio.h"
#include "network_service.h"
#include "network_service_impl.h"

#include "buffer.h"

namespace mumbi {
namespace net
{
	using std::vector;
	using std::error_code;	
	using std::make_unique;

	using asio::ip::tcp;
	using asio::const_buffer;	

	using moodycamel::ConcurrentQueue;	

	using mumbi::io::buffer;

	class session;
	
	class acceptor;
	class connector;

	class session::impl		
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
			, _use_gathering(true)
			, _send_strand(network_service.get_impl().get_io_service())
		{
		}

		void close()
		{
			_is_connected = false;
			_socket->close();
		}

		void receive(session& session)
		{			
			buffer buffer(buffer_size);
			session.get_impl()->_socket->async_receive(asio::buffer(buffer.data()), bind(&session::on_received, std::placeholders::_1, std::placeholders::_2, buffer, session));
		}
		
		void send(session& session, const buffer& buffer)
		{
			if (_use_gathering)
			{
				_send_queue.enqueue(buffer);
				_socket->get_io_service().post(_send_strand.wrap(bind(&impl::send_buffers, this, session)));
			}
			else
				_socket->async_send(asio::buffer(buffer.data()), bind(&session::on_sent, std::placeholders::_1, std::placeholders::_2, session, buffer));
		}

		void on_sent(const error_code& /*error*/, size_t /*bytes_transferred*/, session& /*session*/, buffer& /*buffer*/)
		{	
		}

		void on_sent_buffers(const error_code& /*error*/, size_t /*bytes_transferred*/, session& session, vector<buffer>& /*buffers*/)
		{
			send_buffers(session);
		}

		void send_buffers(session& session)
		{
			vector<const_buffer> gathering_buffers;
			vector<buffer> buffers;			
			
			for (size_t i = 0, sending_buffer_count = 64; i < sending_buffer_count; ++i)
			{	
				buffer buffer;
				if (!_send_queue.try_dequeue(buffer))
					break;				
				
				gathering_buffers.emplace_back(asio::buffer(buffer.data()));
				buffers.push_back(buffer);

			}		

			if (!gathering_buffers.empty())
				_socket->async_send(gathering_buffers, _send_strand.wrap(bind(&session::on_sent_buffers, std::placeholders::_1, std::placeholders::_2, session, buffers)));
		}

		unique_ptr<tcp::socket>		_socket;
		bool						_is_connected;

		bool						_use_gathering;

		io_service::strand			_send_strand;
		ConcurrentQueue<buffer>		_send_queue;
	};
}}

#endif	// MUMBI__NET__SESSION_IMPL__H
