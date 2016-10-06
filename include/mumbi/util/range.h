#pragma once

#ifndef MUMBI__UTIL__RANGE__H
#define MUMBI__UTIL__RANGE__H

#include <iterator>

namespace mumbi {
namespace util
{
	template <typename BaseIterator>
	struct const_forward_iterator : public BaseIterator
	{
		using base_type = BaseIterator;
		using iterator_type = const_forward_iterator;
		using iterator_category = forward_iterator_tag;

		const_forward_iterator& operator+=(typename base_type::difference_type) = delete;
		const_forward_iterator& operator-=(typename base_type::difference_type) = delete;
		const_forward_iterator  operator+ (typename base_type::difference_type) = delete;
		const_forward_iterator  operator- (typename base_type::difference_type) = delete;
		
		const_forward_iterator&	operator++()
		{
			return static_cast<const_forward_iterator&>(base_type::operator++());
		}

		const_forward_iterator operator++(int)
		{
			return static_cast<base_type>(*this)++;
		}

		const_forward_iterator(base_type it) : base_type(it) {}
	};

	template <typename IteratorType>
	struct iterator_pair : pair<IteratorType, IteratorType>
	{
		using iterator_type = IteratorType;

		iterator_pair(iterator_type b, iterator_type e) : pair<iterator_type, iterator_type>(b, e) {}

		iterator_type begin() const { return this->first; }
		iterator_type end()   const { return this->second; }
	};

	template<typename Container> 
	using range = iterator_pair<const_forward_iterator<typename Container::iterator>>;

	template<typename Container>
	using const_range = iterator_pair<const_forward_iterator<typename Container::const_iterator>>;

	template<typename Container>
	range<Container> make_range(Container& container)
	{		
		return { container.begin(), container.end() };
	}

	template<typename Container>
	const_range<Container> make_range(const Container& container)
	{
		return{ container.cbegin(), container.cend() };
	}
}}

#endif	// MUMBI__UTIL__RANGE__H
