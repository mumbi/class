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

	using mumbi::functional::event_signal;
	using mumbi::functional::signal_connector;
	using mumbi::io::buffer_ptr;

	class network_service;	
	class session;	

	class session final		
	{
	private:
		using disconnected_signal_type = event_signal<session, void(session&, const error_code&)>;
		using received_signal_type = event_signal<session, void(session&, const error_code&, buffer_ptr)>;
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

		void send(buffer_ptr buffer);

	private:
		class impl;		
		shared_ptr<impl>	_pimpl;

		session(shared_ptr<impl> pimpl);		

	public:
		shared_ptr<impl> get_impl();
	};
}}

#endif	// MUMBI__NET__SESSION__H

