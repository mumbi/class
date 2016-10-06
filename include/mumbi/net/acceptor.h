#pragma once

#ifndef MUMBI__NET__ACCEPTOR__H
#define MUMBI__NET__ACCEPTOR__H

#include <system_error>
#include <memory>
#include "event_signal.h"

namespace mumbi {
namespace net
{
	using std::error_code;
	using std::enable_shared_from_this;
	using std::unique_ptr;
	using std::shared_ptr;
	using mumbi::functional::event_signal;

	class network_service;	
	class session;
	class acceptor;

	using session_ptr = shared_ptr<session>;
	using acceptor_ptr = shared_ptr<acceptor>;
	
	class acceptor final
		: public enable_shared_from_this<acceptor>
	{
	public:
		event_signal<acceptor, void(const error_code&)>		error_occurred;
		event_signal<acceptor, void(session_ptr)>			accepted;

	public:		
		static acceptor_ptr	create(network_service& network_service, uint16_t port);

	public:		
		~acceptor();

		bool accept();		
		void close();

	private:
		acceptor(network_service& network_service, uint16_t port);

	private:
		class impl;
		unique_ptr<impl>	_pimpl;
	};	
}}

#endif	//	MUMBI__NET__ACCEPTOR__H
