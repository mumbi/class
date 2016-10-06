#pragma once

#ifndef MUMBI__NET__SESSION__H
#define MUMBI__NET__SESSION__H

#include <memory>
#include "event_signal.h"
#include "buffer.h"

namespace mumbi {
namespace net
{
	using std::enable_shared_from_this;
	using std::string;
	using std::unique_ptr;
	using std::error_code;
	using std::shared_ptr;
	using mumbi::functional::event_signal;
	using mumbi::io::buffer_ptr;

	class network_service;	
	class session;

	using session_ptr = shared_ptr<session>;

	class session final
		: public enable_shared_from_this<session>
	{
	public:
		event_signal<session, void(session_ptr, const error_code&)>					disconnected;
		event_signal<session, void(session_ptr, const error_code&, buffer_ptr)>		received;
		event_signal<session, void(session_ptr, const error_code&, size_t)>			sent;

	public:
		static session_ptr create(network_service& network_service);

		string local_endpoint() const;
		string remote_endpoint() const;
		
		bool is_connected() const;
		void disconnect();

		void send(buffer_ptr buffer);		

	private:
		session(network_service& network_service);

	private:
		class impl;
		unique_ptr<impl>	_pimpl;

	public:
		impl& get_impl();
	};
}}

#endif	// MUMBI__NET__SESSION__H

