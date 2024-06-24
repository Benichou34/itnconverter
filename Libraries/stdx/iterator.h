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
 */

#ifndef STDX_ITERATOR_H_INCLUDED
#define STDX_ITERATOR_H_INCLUDED

#include <iterator>
#include <type_traits>

namespace stdx
{
	template <
		class Category,
		class T,
		class Base = T*,
		class Distance = std::ptrdiff_t,
		class Pointer = const T*,
		class Reference = const T&
	>
	class const_iterator : public std::iterator<Category, T, Distance, Pointer, Reference>
	{
	public:
		typedef Base base_type;
		typedef Category iterator_category;
		typedef T value_type;
		typedef Distance difference_type;
		typedef Distance distance_type;
		typedef Pointer pointer;
		typedef Reference reference;

		const_iterator() {}
		const_iterator(const base_type& x) : m_p(x) {}
		const_iterator(const const_iterator& i) : m_p(i.m_p) {} // copy - constructible

		const_iterator& operator=(const const_iterator& i) { m_p = i.m_p; return *this; }

		// Input / output / Forward iterator
		bool operator==(const const_iterator& rhs) const { return m_p == rhs.m_p; } //  equality operators
		bool operator!=(const const_iterator& rhs) const { return m_p != rhs.m_p; } //  inequality operators

		reference operator*() const { return *m_p; }
		pointer operator->() const { return m_p; }

		const_iterator& operator++() { ++m_p; return *this; }
		const_iterator operator++(int) { const_iterator tmp(*this); operator++(); return tmp; }

		void swap(const_iterator& i) { std::swap(m_p, i.m_p); }

		// Bidirectional iterator
		// template <typename Enable = typename std::enable_if<std::is_base_of<std::bidirectional_iterator_tag, iterator_category>::value>::type> // fatal error C1001
		const_iterator& operator--() { --m_p; return *this; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::bidirectional_iterator_tag, iterator_category>::value>::type>
		const_iterator operator--(int) { const_iterator tmp(*this); operator--(); return tmp; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		const_iterator& operator+=(difference_type _Off) { m_p += _Off; return *this; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		const_iterator operator+(difference_type _Off) const { const_iterator tmp(*this); tmp += _Off; return tmp; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		const_iterator& operator-=(difference_type _Off) { return operator+=(-_Off); }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		const_iterator operator-(difference_type _Off) const { return operator+(-_Off); }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		difference_type operator-(const const_iterator& _Right) const { return  m_p - _Right.m_p; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		reference operator[](difference_type _Off) const { return m_p[_Off]; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		bool operator<(const const_iterator& rhs) const { return m_p < rhs.m_p; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		bool operator>(const const_iterator& rhs) const { return m_p > rhs.m_p; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		bool operator<=(const const_iterator& rhs) const { return m_p <= rhs.m_p; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		bool operator>=(const const_iterator& rhs) const { return m_p >= rhs.m_p; }

	protected:
		base_type m_p;
	};

	template <class Base>
	class iterator : public Base
	{
	public:
		typedef Base base_class;
		typedef typename base_class::base_type base_type;
		typedef typename base_class::iterator_category iterator_category;
		typedef typename base_class::value_type value_type;
		typedef typename base_class::difference_type difference_type;
		typedef typename base_class::distance_type distance_type;
		typedef value_type* pointer;
		typedef value_type& reference;

		iterator() {}
		iterator(const base_type& x) : base_class(x) {}
		iterator(const iterator& i) : base_class(i) {} // copy - constructible

		iterator& operator=(const iterator& i) { base_class::operator=(i); return *this; }

		reference operator*() const { return const_cast<reference>(base_class::operator*()); }
		pointer operator->() const { return const_cast<pointer>(base_class::operator->()); }

		iterator& operator++() { base_class::operator++(); return *this; }
		iterator operator++(int) { iterator tmp(*this); operator++(); return tmp; }

		// Bidirectional iterator
		// template <typename Enable = typename std::enable_if<std::is_base_of<std::bidirectional_iterator_tag, iterator_category>::value>::type> // fatal error C1001
		iterator& operator--() { base_class::operator--(); return *this; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::bidirectional_iterator_tag, iterator_category>::value>::type>
		iterator operator--(int) { iterator tmp(*this); operator--(); return tmp; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		iterator& operator+=(difference_type _Off) { base_class::operator+=(_Off); return *this; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		iterator operator+(difference_type _Off) const { iterator tmp(*this); tmp += _Off; return tmp; }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		iterator& operator-=(difference_type _Off) { return operator+=(-_Off); }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		iterator operator-(difference_type _Off) const { return operator+(-_Off); }

		template <typename Enable = typename std::enable_if<std::is_base_of<std::random_access_iterator_tag, iterator_category>::value>::type>
		reference operator[](difference_type _Off) const { return base_class::operator[](_Off); }
	};

	template <class T, class Base>
	class iterator_adaptor : public Base
	{
	public:
		typedef T value_type;
		typedef value_type* pointer;
		typedef value_type& reference;

		iterator_adaptor() {}
		template <class U> iterator_adaptor(U&& b) : Base(std::forward<U>(b)) {}
		template <class U> iterator_adaptor& operator=(U&& b) { Base::operator=(std::forward<U>(b)); return *this; }

		reference operator*() const { return static_cast<reference>(Base::operator*()); }
		pointer operator->() const { return &operator*(); }
	};
}
#endif // STDX_ITERATOR_H_INCLUDED
