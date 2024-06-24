/*
* Copyright (c) 2022, Benichou Software
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the author nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Purpose: Implementation of Prototype pattern
*/

#ifndef STDX_PROTOTYPE_H_INCLUDED
#define STDX_PROTOTYPE_H_INCLUDED

#include <memory>
#include <utility>
#include <type_traits>
#include <cstddef>

namespace stdx
{
	template <class T>
	struct cloneable
	{
		typedef T clone_type;

		cloneable() = default;
		cloneable(const cloneable&) = default;

		virtual ~cloneable() = default;
		virtual std::unique_ptr<clone_type> clone() const = 0;
	};

#define CLONEABLE(T) std::unique_ptr<clone_type> clone() const override { return std::unique_ptr<clone_type>(new T(*this)); }

	template <class T, class Deleter = std::default_delete<T>>
	class clone_ptr
	{
	public:
		typedef T element_type;
		typedef element_type* pointer;
		typedef element_type& reference;

		clone_ptr() = default;
		template<class U, class = typename std::enable_if<std::is_base_of<T, U>::value>::type>
		clone_ptr(const U& u) : m_cptr(u.clone()) {}
		clone_ptr(const clone_ptr& cptr) : m_cptr(cptr.m_cptr ? cptr.m_cptr->clone() : nullptr) {}
		clone_ptr(clone_ptr&& cptr) : m_cptr(std::move(cptr.m_cptr)) {}
		clone_ptr(std::nullptr_t) {}
		template<class U, class E>
		clone_ptr(std::unique_ptr<U, E>&& u) : m_cptr(std::move(u)) {}
		~clone_ptr() = default;

		template<class U, class = typename std::enable_if<std::is_base_of<T, U>::value>::type>
		clone_ptr& operator=(const U& u)
		{
			reset(u.clone());
			return *this;
		}

		clone_ptr& operator=(const clone_ptr& cptr)
		{
			reset(cptr);
			return *this;
		}

		clone_ptr& operator=(clone_ptr&& cptr)
		{
			reset();
			swap(cptr);
			return *this;
		}

		template<class U, class E>
		clone_ptr& operator=(std::unique_ptr<U, E>&& u)
		{
			reset(std::move(u));
			return *this;
		}

		clone_ptr& operator=(std::nullptr_t)
		{
			reset();
			return *this;
		}

		pointer release() { return m_cptr.release(); }

		void swap(clone_ptr& cptr) { std::swap(m_cptr, cptr.m_cptr); }

		void reset(const clone_ptr& cptr) { reset(cptr.m_cptr ? cptr.m_cptr->clone() : nullptr); }
		template<class U, class E>
		void reset(std::unique_ptr<U, E>&& u) { reset(); std::swap(m_cptr, u); }
		void reset() { m_cptr.reset(); }

		pointer get() const { return m_cptr.get(); }

		explicit operator bool() const { return static_cast<bool>(m_cptr); }

		reference operator*() const { return *get(); }
		pointer operator->() const { return get(); }

		bool operator==(const clone_ptr& cptr) const { return m_cptr == cptr.m_cptr; }
		bool operator!=(const clone_ptr& cptr) const { return m_cptr != cptr.m_cptr; }
		bool operator< (const clone_ptr& cptr) const { return m_cptr < cptr.m_cptr; }
		bool operator<=(const clone_ptr& cptr) const { return m_cptr <= cptr.m_cptr; }
		bool operator> (const clone_ptr& cptr) const { return m_cptr > cptr.m_cptr; }
		bool operator>=(const clone_ptr& cptr) const { return m_cptr >= cptr.m_cptr; }

	private:
		std::unique_ptr<element_type, Deleter> m_cptr;
	};

	template <class T, class... Args>
	clone_ptr<T> make_clone(Args&&... args) { return std::make_unique<T>(std::forward<Args>(args)...); }

	template <class T>
	class clone_obj
	{
	public:
		typedef T element_type;
		typedef element_type& reference;
		typedef const element_type& const_reference;

		clone_obj() = default;
		clone_obj(const clone_obj& cobj) : m_cptr(cobj.m_cptr) {}
		clone_obj(clone_obj&& cobj) : m_cptr(std::move(cobj.m_cptr)) {}
		template<class U>
		clone_obj(const U& e) : m_cptr(e.clone()) {}
		template<class U>
		clone_obj(U&& e) : m_cptr(std::unique_ptr<T>(new typename std::remove_reference<U>::type(std::forward<U>(e)))) {}
		~clone_obj() = default;

		clone_obj& operator=(const clone_obj& cobj)
		{
			m_cptr = cobj.m_cptr;
			return *this;
		}

		clone_obj& operator=(clone_obj&& cobj)
		{
			m_cptr = std::move(cobj.m_cptr);
			return *this;
		}

		template<class U>
		clone_obj& operator=(const U& e)
		{
			static_assert(std::is_base_of<T, U>::value, "source type must be derived from element_type");
			m_cptr = e.clone();
			return *this;
		}

		template<class U>
		clone_obj& operator=(U&& e)
		{
			static_assert(std::is_base_of<T, typename std::remove_reference<U>::type>::value, "source type must be derived from element_type");
			m_cptr = std::unique_ptr<T>(new typename std::remove_reference<U>::type(std::forward<U>(e)));
			return *this;
		}

		operator const_reference() const { return m_cptr.operator*(); } // Conversion operator
		operator reference() { return m_cptr.operator*(); } // Conversion operator

		bool operator==(const clone_obj& cobj) const { return static_cast<const_reference>(*this) == static_cast<const_reference>(cobj); }
		bool operator!=(const clone_obj& cobj) const { return static_cast<const_reference>(*this) != static_cast<const_reference>(cobj); }
		bool operator< (const clone_obj& cobj) const { return static_cast<const_reference>(*this) < static_cast<const_reference>(cobj); }
		bool operator<=(const clone_obj& cobj) const { return static_cast<const_reference>(*this) <= static_cast<const_reference>(cobj); }
		bool operator> (const clone_obj& cobj) const { return static_cast<const_reference>(*this) > static_cast<const_reference>(cobj); }
		bool operator>=(const clone_obj& cobj) const { return static_cast<const_reference>(*this) >= static_cast<const_reference>(cobj); }

	private:
		clone_ptr<T> m_cptr;
	};
}
#endif // STDX_PROTOTYPE_H_INCLUDED
