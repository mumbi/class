#pragma once

#ifndef MUMBI__NET__CONNECTOR__H
#define MUMBI__NET__CONNECTOR__H

#include <system_error>
#include <memory>
#include <string>
#include "../functional/event_signal.h"

namespace mumbi {
namespace net
{
	using std::error_code;	
	using std::string;	
	using std::shared_ptr;

	using mumbi::functional::event_signal;
	using mumbi::functional::signal_connector;

	class network_service;
	class session;
	class connector;	

	class connector final
	{
	private:
		using error_occurred_signal_type	= event_signal<connector, void(const error_code&)>;
		using connecting_signal_type		= event_signal<connector, void(const std::string&)>;
		using connected_signal_type			= event_signal<connector, void(session&)>;

	public:
		signal_connector<connector, error_occurred_signal_type>	error_occurred;
		signal_connector<connector, connecting_signal_type>		connecting;
		signal_connector<connector, connected_signal_type>		connected;

	public:		
		connector(network_service& network_service);
		
		void connect(const string& host, uint16_t port);

	private:
		class impl;
		shared_ptr<impl>	_impl;
	};
}}

#endif	// MUMBI__NET__CONNECTOR__H