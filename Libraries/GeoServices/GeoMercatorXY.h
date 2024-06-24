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

#ifndef _GEO_MERCATOR_XY_H_INCLUDED_
#define _GEO_MERCATOR_XY_H_INCLUDED_

#include "GeoLatLng.h"

namespace geo
{
	class CGeoMercatorXY
	{
	public:
		CGeoMercatorXY();
		CGeoMercatorXY(const CGeoMercatorXY& gMercatorXY);
		CGeoMercatorXY(const CGeoLatLng& gLatLng);
		CGeoMercatorXY(int nX, int nY);
		~CGeoMercatorXY() = default;

		operator CGeoLatLng() const;
		operator bool() const;
		bool operator!() const;
		bool operator==(const CGeoMercatorXY& gMercatorXY) const;
		bool operator!=(const CGeoMercatorXY& gMercatorXY) const;

		CGeoMercatorXY& operator= (const CGeoMercatorXY& gMercatorXY);
		CGeoMercatorXY& operator= (const CGeoLatLng& gLatLng);

		inline int x() const throw() { return m_nX; }
		inline void x(int nX) throw() { m_nX = nX; }

		inline int y() const throw() { return m_nY; }
		inline void y(int nY) throw() { m_nY = nY; }

		void clear();

	private:
		int m_nX;
		int m_nY;
	};
} // namespace geo

#endif /*_GEO_MERCATOR_XY_H_INCLUDED_*/
