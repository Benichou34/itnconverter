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

#ifndef _GEO_LOCATIONS_H_INCLUDED_
#define _GEO_LOCATIONS_H_INCLUDED_

#include "GeoLatLngs.h"
#include "GeoLocation.h"

namespace geo
{
	class CGeoLocations : public CGeoLatLngs
	{
	public:
		CGeoLocations() = default;
		CGeoLocations(const CGeoLocations& cGeoLocations) : CGeoLatLngs(cGeoLocations) {}
		CGeoLocations(CGeoLocations&& cGeoLocations) : CGeoLatLngs(std::move(cGeoLocations)) {}
		CGeoLocations(const CGeoLocations& cGeoLocations, size_t start, size_t count = 0) : CGeoLatLngs(cGeoLocations, start, count) {}
		template <class InputIterator> CGeoLocations(InputIterator first, InputIterator last) : CGeoLatLngs(first, last) {}
		virtual ~CGeoLocations() = default;

		typedef stdx::iterator_adaptor<const CGeoLocation, CGeoLatLngs::const_iterator> const_iterator;
		typedef stdx::iterator_adaptor<CGeoLocation, CGeoLatLngs::iterator> iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

		iterator begin() { return iterator(CGeoLatLngs::begin()); }
		const_iterator begin() const { return const_iterator(CGeoLatLngs::begin()); }

		iterator end() { return iterator(CGeoLatLngs::end()); }
		const_iterator end() const { return const_iterator(CGeoLatLngs::end()); }

		reverse_iterator rbegin() { return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

		reverse_iterator rend() { return reverse_iterator(begin()); }
		const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

		void push_front(const CGeoLocation& gLocation) { CGeoLatLngs::push_front(gLocation); }
		void push_back(const CGeoLocation& gLocation) { CGeoLatLngs::push_back(gLocation); }

		CGeoLocation& front() { return reinterpret_cast<CGeoLocation&>(CGeoLatLngs::front()); }
		const CGeoLocation& front() const { return reinterpret_cast<const CGeoLocation&>(CGeoLatLngs::front()); }

		CGeoLocation& back() { return reinterpret_cast<CGeoLocation&>(CGeoLatLngs::back()); }
		const CGeoLocation& back() const { return reinterpret_cast<const CGeoLocation&>(CGeoLatLngs::back()); }

		CGeoLocation& at(size_t n) { return reinterpret_cast<CGeoLocation&>(CGeoLatLngs::at(n)); }
		const CGeoLocation& at(size_t n) const { return reinterpret_cast<const CGeoLocation&>(CGeoLatLngs::at(n)); }

		CGeoLocation& operator[] (size_t n) { return at(n); }
		const CGeoLocation& operator[] (size_t n) const { return at(n); }

		template <class InputIterator>
		iterator erase(InputIterator it) { return iterator(CGeoLatLngs::erase(it)); }

		template <class InputIterator>
		iterator erase(InputIterator first, InputIterator last) { return iterator(CGeoLatLngs::erase(first, last)); }

		CGeoLocations& operator =(const CGeoLocations& cGeoLocations) { CGeoLatLngs::operator =(cGeoLocations); return *this; }
		CGeoLocations& operator+=(const CGeoLocations& cGeoLocations) { CGeoLatLngs::operator+=(cGeoLocations); return *this; }

		CGeoLocations& operator =(CGeoLocations&& cGeoLocations) { CGeoLatLngs::operator =(std::move(cGeoLocations)); return *this; }
		CGeoLocations& operator+=(CGeoLocations&& cGeoLocations) { CGeoLatLngs::operator+=(std::move(cGeoLocations)); return *this; }

		CGeoLocations& operator =(const CGeoLatLngs& cGeoLatLngs) { CGeoLatLngs::operator =(cGeoLatLngs); return *this; }
		CGeoLocations& operator+=(const CGeoLatLngs& cGeoLatLngs) { CGeoLatLngs::operator+=(cGeoLatLngs); return *this; }

		CGeoLocations& operator =(CGeoLatLngs&& cGeoLatLngs) { CGeoLatLngs::operator =(std::move(cGeoLatLngs)); return *this; }
		CGeoLocations& operator+=(CGeoLatLngs&& cGeoLatLngs) { CGeoLatLngs::operator+=(std::move(cGeoLatLngs)); return *this; }

		virtual void sortByAddress();
	};
} // namespace geo

#endif // _GEO_LOCATIONS_H_INCLUDED_
