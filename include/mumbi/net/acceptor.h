#pragma once

#ifndef MUMBI__NET__ACCEPTOR__H
#define MUMBI__NET__ACCEPTOR__H

#include <system_error>
#include <memory>
#include "../functional/event_signal.h"

namespace mumbi {
namespace net
{
	using std::error_code;		
	using std::shared_ptr;
	using std::string;

	using mumbi::functional::event_signal;
	using mumbi::functional::signal_connector;

	class network_service;	
	class session;	
	
	class acceptor final		
	{
	private:	
		using error_occurred_signal_type	= event_signal<acceptor, void(const error_code&)>;
		using accepted_signal_type			= event_signal<acceptor, void(session&, const string&)>;

	public:
		signal_connector<acceptor, error_occurred_signal_type>	error_occurred;
		signal_connector<acceptor, accepted_signal_type>		accepted;

	public:
		acceptor(network_service& network_service, uint16_t port);	

		void accept();		
		void close();	

	private:
		class impl;		
		shared_ptr<impl>	_pimpl;		
	};	
}}

#endif	//	MUMBI__NET__ACCEPTOR__H
