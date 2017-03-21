#pragma once

#ifndef MUMBI__NET__SESSION__H
#define MUMBI__NET__SESSION__H

#include <memory>
#include "../functional/event_signal.h"
#include "../io/buffer.h"

namespace mumbi {
namespace net
{	
	using std::string;	
	using std::error_code;
	using std::shared_ptr;
	using std::vector;

	using mumbi::functional::event_signal;
	using mumbi::functional::signal_connector;	
	using mumbi::io::buffer;

	class network_service;	
	class session;	

	class session final		
	{
	private:
		using disconnected_signal_type = event_signal<session, void(session&, const error_code&)>;		
		using received_signal_type = event_signal<session, void(session&, const error_code&, const buffer&)>;
		using sent_signal_type = event_signal<session, void(session&, const error_code&, size_t)>;

	public:
		signal_connector<session, disconnected_signal_type>	disconnected;
		signal_connector<session, received_signal_type>		received;
		signal_connector<session, sent_signal_type>			sent;

	public:
		session();
		session(network_service& network_service);

		bool is_vaild() const;

		string local_endpoint() const;
		string remote_endpoint() const;
		
		bool is_connected() const;
		bool disconnect();
		
		void send(const buffer& buff);

	private:		
		static void on_received(const error_code& error, size_t bytes_transferred, buffer& buffer, session& session);
		static void on_sent(const error_code& error, size_t bytes_transferred, session& session, buffer& buffer);
		static void on_sent_buffers(const error_code& error, size_t bytes_transferred, session& session, vector<buffer>& buffers);

	private:
		class impl;		
		shared_ptr<impl>	_impl;

	public:
		shared_ptr<impl> get_impl();
	};
}}

#endif	// MUMBI__NET__SESSION__H

