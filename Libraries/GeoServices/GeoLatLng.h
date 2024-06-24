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

#ifndef _GEO_LATLNG_H_INCLUDED_
#define _GEO_LATLNG_H_INCLUDED_

#include <cstddef>
#include "stdx/prototype.h"

namespace geo
{
	class CGeoLatLngLine;

	class CGeoLatLng : public stdx::cloneable<CGeoLatLng>
	{
	public:
		CLONEABLE(CGeoLatLng);

		CGeoLatLng();
		CGeoLatLng(const CGeoLatLng& gLatLng);
		CGeoLatLng(CGeoLatLng&& gLatLng);
		CGeoLatLng(double dLatitude, double dLongitude, double dAltitude = 0);
		virtual ~CGeoLatLng() = default;

		virtual operator bool() const;
		virtual bool operator!() const;
		virtual bool operator==(const CGeoLatLng& gLatLng) const;
		virtual bool operator!=(const CGeoLatLng& gLatLng) const;

		CGeoLatLng& operator= (const CGeoLatLng& gLatLng);
		CGeoLatLng& operator= (CGeoLatLng&& gLatLng);

		void coords(double dLatitude, double dLongitude, double dAltitude = 0);

		inline double precision() const throw() { return m_dPrecision; }
		inline void precision(double dPrecision) throw() { m_dPrecision = dPrecision; }

		inline double lat() const throw() { return m_dLatitude; }
		inline void lat(double dLatitude) throw() { m_dLatitude = dLatitude; }
		double latRadians() const;

		inline double lng() const throw() { return m_dLongitude; }
		inline void lng(double dLongitude) throw() { m_dLongitude = dLongitude; }
		double lngRadians() const;

		inline double alt() const throw() { return m_dAltitude; }
		inline void alt(double dAltitude) throw() { m_dAltitude = dAltitude; }

		virtual size_t distanceFrom(const CGeoLatLng& gLatLng) const;

		virtual void wrap();
		virtual void clear();

		// Used by Sort Algorithms, operator<
		bool operator< (const CGeoLatLng& gLatLng) const;

	private:
		double m_dPrecision;
		double m_dLatitude;
		double m_dLongitude;
		double m_dAltitude;
	};
} // namespace geo

#endif // _GEO_LATLNG_H_INCLUDED_
