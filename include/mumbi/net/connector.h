#pragma once

#ifndef MUMBI__NET__CONNECTOR__H
#define MUMBI__NET__CONNECTOR__H

#include <system_error>
#include <memory>
#include <string>
#include "event_signal.h"

namespace mumbi {
namespace net
{
	using std::error_code;
	using std::enable_shared_from_this;
	using std::string;
	using std::unique_ptr;
	using std::shared_ptr;
	using mumbi::functional::event_signal;

	class network_service;
	class session;
	class connector;

	using session_ptr = shared_ptr<session>;
	using connector_ptr = shared_ptr<connector>;

	class connector final
		: public enable_shared_from_this<connector>
	{
	public:
		event_signal<connector, void(const error_code&)>	error_occurred;
		event_signal<connector, void(const std::string&)>	connecting;
		event_signal<connector, void(session_ptr)>			connected;

	public:
		static connector_ptr create(network_service& network_service);

		~connector();
		void connect(const string& host, uint16_t port);		

	private:		
		connector(network_service& network_service);

	private:
		class impl;		
		unique_ptr<impl>	_pimpl;
	};
}}

#endif	// MUMBI__NET__CONNECTOR__H