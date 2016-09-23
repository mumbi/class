#pragma once

#ifndef NET__NETWORK_SERVICE__H
#define NET__NETWORK_SERVICE__H

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

		void start();
		void stop();	

	private:
		class impl;
		unique_ptr<impl>	_pimpl;

	public:
		impl& get_impl();
	};
}}

#endif	// NET__NETWORK_SERVICE__H
