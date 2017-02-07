#pragma once

#ifndef MUMBI__UTIL__TUPLE__H
#define MUMBI__UTIL__TUPLE__H

#include <utility>
#include <tuple>

namespace mumbi {
namespace util
{
	using std::index_sequence;
	using std::make_index_sequence;
	using std::integral_constant;
	using std::tuple_size;
	using std::remove_reference;
	using std::enable_if;
	using std::get;
	using std::make_tuple;
	
	template<typename F, size_t... Is>
	auto make_tuple_impl(F f, index_sequence<Is...>)
	{
		return make_tuple(f(Is)...);
	}

	template<size_t N, typename F>
	auto make_tuple(F&& f)
	{
		return gen_tuple_impl(forward<F>(f), make_index_sequence<N>());
	}

	template<typename TupleType, typename F>
	void for_each(TupleType&&, F, integral_constant<size_t, tuple_size<typename remove_reference<TupleType>::type>::value>)
	{
	}

	template<size_t I, typename TupleType, typename F, typename = typename enable_if<I != tuple_size<typename remove_reference<TupleType>::type>::value>::type>
	void for_each(TupleType&& t, F f, integral_constant<size_t, I>)
	{
		f(get<I>(t));
		for_each(forward<TupleType>(t), f, integral_constant<size_t, I + 1>());
	}

	template<typename TupleType, typename F>
	void for_each(TupleType&& t, F f)
	{
		for_each(forward<TupleType>(t), f, integral_constant<size_t, 0>());
	}

	template<typename TupleType, size_t... Is, typename F>
	auto transform_impl(TupleType& t, index_sequence<Is...>, F f)
	{
		return make_tuple(f(get<Is>(t))...);
	}

	template<size_t N, typename TupleType, typename F>
	auto transform(TupleType& t, F&& f)
	{
		return transform_impl(t, make_index_sequence<N>(), forward<F>(f));
	}

	template<typename TupleType, typename F>
	auto transform(TupleType& t, F&& f)
	{
		return transform_impl(t, make_index_sequence<tuple_size<TupleType>::value>(), forward<F>(f));
	}
}}

#endif	// MUMBI__UTIL__TUPLE__H
