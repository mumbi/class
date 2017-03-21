#pragma once

#ifndef MUMBI__NET__NETWORK_SERVICE__H
#define MUMBI__NET__NETWORK_SERVICE__H

#include <memory>

namespace mumbi {
namespace net
{
	using std::unique_ptr;

	class network_service final		
	{
	public:
		~network_service();		
		network_service();
		network_service(size_t thread_count);

		bool stopped() const;

		void restart();		
		void stop();
		
		void start_running();
		void end_running();

	private:
		class impl;
		unique_ptr<impl>	_impl;

	public:
		impl& get_impl();
	};
}}

#endif	// MUMBI__NET__NETWORK_SERVICE__H
