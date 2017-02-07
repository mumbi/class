// See http://www.boost.org/libs/any for Documentation.

#ifndef BOOST_ANY_INCLUDED
#define BOOST_ANY_INCLUDED

#if defined(_MSC_VER)
# pragma once
#endif

// what:  variant type boost::any
// who:   contributed by Kevlin Henney,
//        with features contributed and bugs found by
//        Antony Polukhin, Ed Brey, Mark Rodgers, 
//        Peter Dimov, and James Curran
// when:  July 2001, April 2013 - May 2013

#include <algorithm>

//#include "boost/config.hpp"
//#include <boost/type_index.hpp>
//#include <boost/type_traits/remove_reference.hpp>
//#include <boost/type_traits/decay.hpp>
//#include <boost/type_traits/remove_cv.hpp>
//#include <boost/type_traits/add_reference.hpp>
//#include <boost/type_traits/is_reference.hpp>
//#include <boost/type_traits/is_const.hpp>
//#include <boost/throw_exception.hpp>
//#include <boost/static_assert.hpp>
//#include <boost/utility/enable_if.hpp>
//#include <boost/type_traits/is_same.hpp>
//#include <boost/type_traits/is_const.hpp>
//#include <boost/mpl/if.hpp>

#include <typeinfo>

namespace std {
namespace experimental
{
	class any
	{
	public: // structors

		any() noexcept
			: content(0)
		{
		}

		template<typename ValueType>
		any(const ValueType & value)
			: content(new holder<
				typename remove_cv<typename decay<const ValueType>::type>::type
			>(value))
		{
		}

		any(const any & other)
			: content(other.content ? other.content->clone() : 0)
		{
		}

//#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
		// Move constructor
		any(any&& other) noexcept
			: content(other.content)
		{
			other.content = 0;
		}

		template <bool B, class T = void>
		struct disable_if_c
		{
			typedef T type;
		};

		template <class T>
		struct disable_if_c<true, T> {};

		template <class Cond, class T = void>
		struct disable_if : public disable_if_c<Cond::value, T> {};

		// Perfect forwarding of ValueType
		template<typename ValueType>
		any(ValueType&& value
			, typename disable_if<std::is_same<any&, ValueType> >::type* = 0 // disable if value has type `any&`
			, typename disable_if<std::is_const<ValueType> >::type* = 0) // disable if value has type `const ValueType&&`
			: content(new holder< typename decay<ValueType>::type >(static_cast<ValueType&&>(value)))
		{
		}
//#endif

		~any() noexcept
		{
			delete content;
		}

	public: // modifiers

		any & swap(any & rhs) noexcept
		{
			std::swap(content, rhs.content);
			return *this;
		}


//#ifdef BOOST_NO_CXX11_RVALUE_REFERENCES
		/*template<typename ValueType>
		any & operator=(const ValueType & rhs)
		{
			any(rhs).swap(*this);
			return *this;
		}

		any & operator=(any rhs)
		{
			any(rhs).swap(*this);
			return *this;
		}*/

//#else 
		any & operator=(const any& rhs)
		{
			any(rhs).swap(*this);
			return *this;
		}

		// move assignement
		any & operator=(any&& rhs) noexcept
		{
			rhs.swap(*this);
			any().swap(rhs);
			return *this;
		}

		// Perfect forwarding of ValueType
		template <class ValueType>
		any & operator=(ValueType&& rhs)
		{
			any(static_cast<ValueType&&>(rhs)).swap(*this);
			return *this;
		}
//#endif

	public: // queries

		bool empty() const noexcept
		{
			return !content;
		}

		void clear() noexcept
		{
			any().swap(*this);
		}

		const type_info& type() const noexcept
		{
			return content ? content->type() : typeid(void);
		}

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS
	private: // types
#else
	public: // types (public so any_cast can be non-friend)
#endif

		class placeholder
		{
		public: // structors

			virtual ~placeholder()
			{
			}

		public: // queries

			virtual const type_info& type() const noexcept = 0;

			virtual placeholder * clone() const = 0;

		};

		template<typename ValueType>
		class holder : public placeholder
		{
		public: // structors

			holder(const ValueType & value)
				: held(value)
			{
			}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
			holder(ValueType&& value)
				: held(static_cast< ValueType&& >(value))
			{
			}
#endif
		public: // queries

			virtual const type_info& type() const noexcept
			{
				return typeid(ValueType);
			}

			virtual placeholder * clone() const
			{
				return new holder(held);
			}

		public: // representation

			ValueType held;

		private: // intentionally left unimplemented
			holder & operator=(const holder &);
		};

#ifndef BOOST_NO_MEMBER_TEMPLATE_FRIENDS

	private: // representation

		template<typename ValueType>
		friend ValueType * any_cast(any *) noexcept;

		template<typename ValueType>
		friend ValueType * unsafe_any_cast(any *) noexcept;

#else

	public: // representation (public so any_cast can be non-friend)

#endif

		placeholder * content;

	};

	inline void swap(any & lhs, any & rhs) noexcept
	{
		lhs.swap(rhs);
	}

	class bad_any_cast :
#ifndef BOOST_NO_RTTI
		public std::bad_cast
#else
		public std::exception
#endif
	{
	public:
		virtual const char * what() const noexcept
		{
			return "boost::bad_any_cast: "
				"failed conversion using boost::any_cast";
		}
	};

	template<typename ValueType>
	ValueType * any_cast(any * operand) noexcept
	{
		return operand && operand->type() == typeid(ValueType)
			? &static_cast<any::holder<typename remove_cv<ValueType>::type> *>(operand->content)->held
			: 0;
	}

	template<typename ValueType>
	inline const ValueType * any_cast(const any * operand) noexcept
	{
		return any_cast<ValueType>(const_cast<any *>(operand));
	}

	template <typename T>
	struct add_reference_impl
	{
		typedef T& type;
	};	

	template <class T> struct add_reference
	{
	   typedef typename add_reference_impl<T>::type type;
	};
	template <class T> struct add_reference<T&>
	{
	   typedef T& type;
	};

// these full specialisations are always required:
template <> struct add_reference<void> { typedef void type; };
#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
template <> struct add_reference<const void> { typedef const void type; };
template <> struct add_reference<const volatile void> { typedef const volatile void type; };
template <> struct add_reference<volatile void> { typedef volatile void type; };
#endif

	template<typename ValueType>
	ValueType any_cast(any & operand)
	{
		typedef typename remove_reference<ValueType>::type nonref;


		nonref * result = any_cast<nonref>(&operand);
		if (!result)
			throw bad_any_cast();

		// Attempt to avoid construction of a temporary object in cases when 
		// `ValueType` is not a reference. Example:
		// `static_cast<std::string>(*result);` 
		// which is equal to `std::string(*result);`
		/*typedef typename std::conditional<true,
			std::is_reference<ValueType>,
			ValueType,
			typename add_reference<ValueType>::type
		>::type ref_type;*/
		typedef ValueType ref_type;

		return static_cast<ref_type>(*result);
	}

	template<typename ValueType>
	inline ValueType any_cast(const any & operand)
	{
		typedef typename remove_reference<ValueType>::type nonref;
		return any_cast<const nonref &>(const_cast<any &>(operand));
	}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
	template<typename ValueType>
	inline ValueType any_cast(any&& operand)
	{
		static_assert(
			std::is_rvalue_reference<ValueType&&>::value /*true if ValueType is rvalue or just a value*/
			|| std::is_const< typename std::remove_reference<ValueType>::type >::value,
			"boost::any_cast shall not be used for getting nonconst references to temporary objects"
		);
		return any_cast<ValueType>(operand);
	}
#endif


	// Note: The "unsafe" versions of any_cast are not part of the
	// public interface and may be removed at any time. They are
	// required where we know what type is stored in the any and can't
	// use typeid() comparison, e.g., when our types may travel across
	// different shared libraries.
	template<typename ValueType>
	inline ValueType * unsafe_any_cast(any * operand) noexcept
	{
		return &static_cast<any::holder<ValueType> *>(operand->content)->held;
	}

	template<typename ValueType>
	inline const ValueType * unsafe_any_cast(const any * operand) noexcept
	{
		return unsafe_any_cast<ValueType>(const_cast<any *>(operand));
	}
}}

// Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#endif


////
//// Implementation of N4562 std::experimental::any (merged into C++17) for C++11 compilers.
////
//// See also:
////   + http://en.cppreference.com/w/cpp/any
////   + http://en.cppreference.com/w/cpp/experimental/any
////   + http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4562.html#any
////   + https://cplusplus.github.io/LWG/lwg-active.html#2509
////
////
//// Copyright (c) 2016 Denilson das Mercês Amorim
////
//// Distributed under the Boost Software License, Version 1.0. (See accompanying
//// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
////
//#ifndef LINB_ANY_HPP
//#define LINB_ANY_HPP
//#pragma once
//#include <typeinfo>
//#include <type_traits>
//#include <stdexcept>
//
//namespace std {
//namespace experimental
//{
//	class bad_any_cast : public std::bad_cast
//	{
//	public:
//		const char* what() const noexcept override
//		{
//			return "bad any cast";
//		}
//	};
//
//	class any final
//	{
//	public:
//		/// Constructs an object of type any with an empty state.
//		any() :
//			vtable(nullptr)
//		{
//		}
//
//		/// Constructs an object of type any with an equivalent state as other.
//		any(const any& rhs) :
//			vtable(rhs.vtable)
//		{
//			if (!rhs.empty())
//			{
//				rhs.vtable->copy(rhs.storage, this->storage);
//			}
//		}
//
//		/// Constructs an object of type any with a state equivalent to the original state of other.
//		/// rhs is left in a valid but otherwise unspecified state.
//		any(any&& rhs) noexcept :
//		vtable(rhs.vtable)
//		{
//			if (!rhs.empty())
//			{
//				rhs.vtable->move(rhs.storage, this->storage);
//				rhs.vtable = nullptr;
//			}
//		}
//
//		/// Same effect as this->clear().
//		~any()
//		{
//			this->clear();
//		}
//
//		/// Constructs an object of type any that contains an object of type T direct-initialized with std::forward<ValueType>(value).
//		///
//		/// T shall satisfy the CopyConstructible requirements, otherwise the program is ill-formed.
//		/// This is because an `any` may be copy constructed into another `any` at any time, so a copy should always be allowed.
//		template<typename ValueType, typename = typename std::enable_if<!std::is_same<typename std::decay<ValueType>::type, any>::value>::type>
//		any(ValueType&& value)
//		{
//			static_assert(std::is_copy_constructible<typename std::decay<ValueType>::type>::value,
//				"T shall satisfy the CopyConstructible requirements.");
//			this->construct(std::forward<ValueType>(value));
//		}
//
//		/// Has the same effect as any(rhs).swap(*this). No effects if an exception is thrown.
//		any& operator=(const any& rhs)
//		{
//			any(rhs).swap(*this);
//			return *this;
//		}
//
//		/// Has the same effect as any(std::move(rhs)).swap(*this).
//		///
//		/// The state of *this is equivalent to the original state of rhs and rhs is left in a valid
//		/// but otherwise unspecified state.
//		any& operator=(any&& rhs) noexcept
//		{
//			any(std::move(rhs)).swap(*this);
//			return *this;
//		}
//
//		/// Has the same effect as any(std::forward<ValueType>(value)).swap(*this). No effect if a exception is thrown.
//		///
//		/// T shall satisfy the CopyConstructible requirements, otherwise the program is ill-formed.
//		/// This is because an `any` may be copy constructed into another `any` at any time, so a copy should always be allowed.
//		template<typename ValueType, typename = typename std::enable_if<!std::is_same<typename std::decay<ValueType>::type, any>::value>::type>
//		any& operator=(ValueType&& value)
//		{
//			static_assert(std::is_copy_constructible<typename std::decay<ValueType>::type>::value,
//				"T shall satisfy the CopyConstructible requirements.");
//			any(std::forward<ValueType>(value)).swap(*this);
//			return *this;
//		}
//
//		/// If not empty, destroys the contained object.
//		void clear() noexcept
//		{
//			if (!empty())
//			{
//				this->vtable->destroy(storage);
//				this->vtable = nullptr;
//			}
//		}
//
//		/// Returns true if *this has no contained object, otherwise false.
//		bool empty() const noexcept
//		{
//			return this->vtable == nullptr;
//		}
//
//		/// If *this has a contained object of type T, typeid(T); otherwise typeid(void).
//		const std::type_info& type() const noexcept
//		{
//			return empty() ? typeid(void) : this->vtable->type();
//		}
//
//		/// Exchange the states of *this and rhs.
//		void swap(any& rhs) noexcept
//		{
//			if (this->vtable != rhs.vtable)
//			{
//				any tmp(std::move(rhs));
//
//				// move from *this to rhs.
//				rhs.vtable = this->vtable;
//				if (this->vtable != nullptr)
//				{
//					this->vtable->move(this->storage, rhs.storage);
//					//this->vtable = nullptr; -- uneeded, see below
//				}
//
//				// move from tmp (previously rhs) to *this.
//				this->vtable = tmp.vtable;
//				if (tmp.vtable != nullptr)
//				{
//					tmp.vtable->move(tmp.storage, this->storage);
//					tmp.vtable = nullptr;
//				}
//			}
//			else // same types
//			{
//				if (this->vtable != nullptr)
//					this->vtable->swap(this->storage, rhs.storage);
//			}
//		}
//
//	private: // Storage and Virtual Method Table
//
//		union storage_union
//		{
//			using stack_storage_t = typename std::aligned_storage<2 * sizeof(void*), std::alignment_of<void*>::value>::type;
//
//			void*               dynamic;
//			stack_storage_t     stack;      // 2 words for e.g. shared_ptr
//		};
//
//		/// Base VTable specification.
//		struct vtable_type
//		{
//			// Note: The caller is responssible for doing .vtable = nullptr after destructful operations
//			// such as destroy() and/or move().
//
//			/// The type of the object this vtable is for.
//			const std::type_info& (*type)() noexcept;
//
//			/// Destroys the object in the union.
//			/// The state of the union after this call is unspecified, caller must ensure not to use src anymore.
//			void(*destroy)(storage_union&) noexcept;
//
//			/// Copies the **inner** content of the src union into the yet unitialized dest union.
//			/// As such, both inner objects will have the same state, but on separate memory locations.
//			void(*copy)(const storage_union& src, storage_union& dest);
//
//			/// Moves the storage from src to the yet unitialized dest union.
//			/// The state of src after this call is unspecified, caller must ensure not to use src anymore.
//			void(*move)(storage_union& src, storage_union& dest) noexcept;
//
//			/// Exchanges the storage between lhs and rhs.
//			void(*swap)(storage_union& lhs, storage_union& rhs) noexcept;
//		};
//
//		/// VTable for dynamically allocated storage.
//		template<typename T>
//		struct vtable_dynamic
//		{
//			static const std::type_info& type() noexcept
//			{
//				return typeid(T);
//			}
//
//			static void destroy(storage_union& storage) noexcept
//			{
//				//assert(reinterpret_cast<T*>(storage.dynamic));
//				delete reinterpret_cast<T*>(storage.dynamic);
//			}
//
//			static void copy(const storage_union& src, storage_union& dest)
//			{
//				dest.dynamic = new T(*reinterpret_cast<const T*>(src.dynamic));
//			}
//
//			static void move(storage_union& src, storage_union& dest) noexcept
//			{
//				dest.dynamic = src.dynamic;
//				src.dynamic = nullptr;
//			}
//
//			static void swap(storage_union& lhs, storage_union& rhs) noexcept
//			{
//				// just exchage the storage pointers.
//				std::swap(lhs.dynamic, rhs.dynamic);
//			}
//		};
//
//		/// VTable for stack allocated storage.
//		template<typename T>
//		struct vtable_stack
//		{
//			static const std::type_info& type() noexcept
//			{
//				return typeid(T);
//			}
//
//			static void destroy(storage_union& storage) noexcept
//			{
//				reinterpret_cast<T*>(&storage.stack)->~T();
//			}
//
//			static void copy(const storage_union& src, storage_union& dest)
//			{
//				new (&dest.stack) T(reinterpret_cast<const T&>(src.stack));
//			}
//
//			static void move(storage_union& src, storage_union& dest) noexcept
//			{
//				// one of the conditions for using vtable_stack is a nothrow move constructor,
//				// so this move constructor will never throw a exception.
//				new (&dest.stack) T(std::move(reinterpret_cast<T&>(src.stack)));
//				destroy(src);
//			}
//
//			static void swap(storage_union& lhs, storage_union& rhs) noexcept
//			{
//				std::swap(reinterpret_cast<T&>(lhs.stack), reinterpret_cast<T&>(rhs.stack));
//			}
//		};
//
//		/// Whether the type T must be dynamically allocated or can be stored on the stack.
//		template<typename T>
//		struct requires_allocation :
//			std::integral_constant<bool,
//			!(std::is_nothrow_move_constructible<T>::value      // N4562 §6.3/3 [any.class]
//				&& sizeof(T) <= sizeof(storage_union::stack)
//				&& std::alignment_of<T>::value <= std::alignment_of<storage_union::stack_storage_t>::value)>
//		{};
//
//		/// Returns the pointer to the vtable of the type T.
//		template<typename T>
//		static vtable_type* vtable_for_type()
//		{
//			using VTableType = typename std::conditional<requires_allocation<T>::value, vtable_dynamic<T>, vtable_stack<T>>::type;
//			static vtable_type table = {
//				VTableType::type, VTableType::destroy,
//				VTableType::copy, VTableType::move,
//				VTableType::swap,
//			};
//			return &table;
//		}
//
//	protected:
//		template<typename T>
//		friend const T* any_cast(const any* operand) noexcept;
//		template<typename T>
//		friend T* any_cast(any* operand) noexcept;
//
//		/// Same effect as is_same(this->type(), t);
//		bool is_typed(const std::type_info& t) const
//		{
//			return is_same(this->type(), t);
//		}
//
//		/// Checks if two type infos are the same.
//		///
//		/// If ANY_IMPL_FAST_TYPE_INFO_COMPARE is defined, checks only the address of the
//		/// type infos, otherwise does an actual comparision. Checking addresses is
//		/// only a valid approach when there's no interaction with outside sources
//		/// (other shared libraries and such).
//		static bool is_same(const std::type_info& a, const std::type_info& b)
//		{
//#ifdef ANY_IMPL_FAST_TYPE_INFO_COMPARE
//			return &a == &b;
//#else
//			return a == b;
//#endif
//		}
//
//		/// Casts (with no type_info checks) the storage pointer as const T*.
//		template<typename T>
//		const T* cast() const noexcept
//		{
//			return requires_allocation<typename std::decay<T>::type>::value ?
//				reinterpret_cast<const T*>(storage.dynamic) :
//				reinterpret_cast<const T*>(&storage.stack);
//		}
//
//		/// Casts (with no type_info checks) the storage pointer as T*.
//		template<typename T>
//		T* cast() noexcept
//		{
//			return requires_allocation<typename std::decay<T>::type>::value ?
//				reinterpret_cast<T*>(storage.dynamic) :
//				reinterpret_cast<T*>(&storage.stack);
//		}
//
//	private:
//		storage_union storage; // on offset(0) so no padding for align
//		vtable_type*  vtable;
//
//		template<typename ValueType, typename T>
//		typename std::enable_if<requires_allocation<T>::value>::type
//			do_construct(ValueType&& value)
//		{
//			storage.dynamic = new T(std::forward<ValueType>(value));
//		}
//
//		template<typename ValueType, typename T>
//		typename std::enable_if<!requires_allocation<T>::value>::type
//			do_construct(ValueType&& value)
//		{
//			new (&storage.stack) T(std::forward<ValueType>(value));
//		}
//
//		/// Chooses between stack and dynamic allocation for the type decay_t<ValueType>,
//		/// assigns the correct vtable, and constructs the object on our storage.
//		template<typename ValueType>
//		void construct(ValueType&& value)
//		{
//			using T = typename std::decay<ValueType>::type;
//
//			this->vtable = vtable_for_type<T>();
//
//			do_construct<ValueType, T>(std::forward<ValueType>(value));
//		}
//	};
//
//
//
//	namespace detail
//	{
//		template<typename ValueType>
//		inline ValueType any_cast_move_if_true(typename std::remove_reference<ValueType>::type* p, std::true_type)
//		{
//			return std::move(*p);
//		}
//
//		template<typename ValueType>
//		inline ValueType any_cast_move_if_true(typename std::remove_reference<ValueType>::type* p, std::false_type)
//		{
//			return *p;
//		}
//	}
//
//	/// Performs *any_cast<add_const_t<remove_reference_t<ValueType>>>(&operand), or throws bad_any_cast on failure.
//	template<typename ValueType>
//	inline ValueType any_cast(const any& operand)
//	{
//		auto p = any_cast<typename std::add_const<typename std::remove_reference<ValueType>::type>::type>(&operand);
//		if (p == nullptr) throw bad_any_cast();
//		return *p;
//	}
//
//	/// Performs *any_cast<remove_reference_t<ValueType>>(&operand), or throws bad_any_cast on failure.
//	template<typename ValueType>
//	inline ValueType any_cast(any& operand)
//	{
//		auto p = any_cast<typename std::remove_reference<ValueType>::type>(&operand);
//		if (p == nullptr) throw bad_any_cast();
//		return *p;
//	}
//
//	///
//	/// If ANY_IMPL_ANYCAST_MOVEABLE is not defined, does as N4562 specifies:
//	///     Performs *any_cast<remove_reference_t<ValueType>>(&operand), or throws bad_any_cast on failure.
//	///
//	/// If ANY_IMPL_ANYCAST_MOVEABLE is defined, does as LWG Defect 2509 specifies:
//	///     If ValueType is MoveConstructible and isn't a lvalue reference, performs
//	///     std::move(*any_cast<remove_reference_t<ValueType>>(&operand)), otherwise
//	///     *any_cast<remove_reference_t<ValueType>>(&operand). Throws bad_any_cast on failure.
//	///
//	template<typename ValueType>
//	inline ValueType any_cast(any&& operand)
//	{
//#ifdef ANY_IMPL_ANY_CAST_MOVEABLE
//		// https://cplusplus.github.io/LWG/lwg-active.html#2509
//		using can_move = std::integral_constant<bool,
//			std::is_move_constructible<ValueType>::value
//			&& !std::is_lvalue_reference<ValueType>::value>;
//#else
//		using can_move = std::false_type;
//#endif
//
//		auto p = any_cast<typename std::remove_reference<ValueType>::type>(&operand);
//		if (p == nullptr) throw bad_any_cast();
//		return detail::any_cast_move_if_true<ValueType>(p, can_move());
//	}
//
//	/// If operand != nullptr && operand->type() == typeid(ValueType), a pointer to the object
//	/// contained by operand, otherwise nullptr.
//	template<typename T>
//	inline const T* any_cast(const any* operand) noexcept
//	{
//		if (operand == nullptr || !operand->is_typed(typeid(T)))
//			return nullptr;
//		else
//			return operand->cast<T>();
//	}
//
//	/// If operand != nullptr && operand->type() == typeid(ValueType), a pointer to the object
//	/// contained by operand, otherwise nullptr.
//	template<typename T>
//	inline T* any_cast(any* operand) noexcept
//	{
//		if (operand == nullptr || !operand->is_typed(typeid(T)))
//			return nullptr;
//		else
//			return operand->cast<T>();
//	}
//
//}}
//
//namespace std
//{
//	inline void swap(std::experimental::any& lhs, std::experimental::any& rhs) noexcept
//	{
//		lhs.swap(rhs);
//	}
//}
//
//#endif