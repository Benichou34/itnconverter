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

#ifndef _GEO_LATLNGS_H_INCLUDED_
#define _GEO_LATLNGS_H_INCLUDED_

#include <deque>
#include <list>
#include "GeoLatLng.h"
#include "stdx/iterator.h"

namespace geo
{
	typedef std::list< stdx::clone_obj<CGeoLatLng> > GeoLatLngs;

	class CGeoLatLngs : public GeoLatLngs
	{
	public:
		CGeoLatLngs();
		CGeoLatLngs(const CGeoLatLngs& gLatLngs);
		CGeoLatLngs(CGeoLatLngs&& gLatLngs);
		CGeoLatLngs(const CGeoLatLngs& gLatLngs, size_t start, size_t count = 0);
		template <class InputIterator> CGeoLatLngs(InputIterator first, InputIterator last) : GeoLatLngs(first, last) {}
		virtual ~CGeoLatLngs();

		typedef stdx::iterator_adaptor<const CGeoLatLng, GeoLatLngs::const_iterator> const_iterator;
		typedef stdx::iterator_adaptor<CGeoLatLng, GeoLatLngs::iterator> iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

		iterator begin() { return iterator(GeoLatLngs::begin()); }
		const_iterator begin() const { return const_iterator(GeoLatLngs::begin()); }

		iterator end() { return iterator(GeoLatLngs::end()); }
		const_iterator end() const { return const_iterator(GeoLatLngs::end()); }

		reverse_iterator rbegin() { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

		reverse_iterator rend() { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

		void push_front(const CGeoLatLng& gLatLng) { GeoLatLngs::push_front(gLatLng); }
		void push_back(const CGeoLatLng& gLatLng) { GeoLatLngs::push_back(gLatLng); }

		CGeoLatLng& front() { return GeoLatLngs::front(); }
		const CGeoLatLng& front() const { return GeoLatLngs::front(); }

		CGeoLatLng& back() { return GeoLatLngs::back(); }
		const CGeoLatLng& back() const { return GeoLatLngs::back(); }

		CGeoLatLng& at(size_t n);
		const CGeoLatLng& at(size_t n) const;

		CGeoLatLng& operator[] (size_t n) { return at(n); }
		const CGeoLatLng& operator[] (size_t n) const { return at(n); }

		template <class InputIterator>
		void append(InputIterator first, InputIterator last)
		{
			for (; first != last; ++first)
				push_back(*first);
		}

		template <class InputIterator>
		iterator erase(InputIterator it) { return iterator(GeoLatLngs::erase(it)); }

		template <class InputIterator>
		iterator erase(InputIterator first, InputIterator last) { return iterator(GeoLatLngs::erase(first, last)); }

		virtual size_t upper_bound() const;

		virtual void removeDuplicates();
		virtual void removeEmpties();

		CGeoLatLngs& operator=(const CGeoLatLngs& gLatLngs);
		CGeoLatLngs& operator=(CGeoLatLngs&& gLatLngs);
		CGeoLatLngs& operator+=(const CGeoLatLngs& gLatLngs);
		CGeoLatLngs& operator+=(CGeoLatLngs&& gLatLngs);
	};
} // namespace geo

#endif // _GEO_LATLNGS_H_INCLUDED_
