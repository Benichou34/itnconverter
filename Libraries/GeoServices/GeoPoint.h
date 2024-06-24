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

#ifndef _GEO_POINT_H_INCLUDED_
#define _GEO_POINT_H_INCLUDED_

namespace geo
{
	class CGeoPoint
	{
	public:
		CGeoPoint();
		CGeoPoint(const CGeoPoint& geoPoint);
		CGeoPoint(long lX, long lY);
		virtual ~CGeoPoint() = default;

		virtual operator bool() const;
		virtual bool operator!() const;
		virtual bool operator==(const CGeoPoint& geoPoint) const;
		virtual bool operator!=(const CGeoPoint& geoPoint) const;

		CGeoPoint& operator= (const CGeoPoint& geoPoint);
		virtual CGeoPoint& operator+= (const CGeoPoint& geoPoint);
		virtual CGeoPoint& operator-= (const CGeoPoint& geoPoint);

		virtual CGeoPoint& operator+= (long l);
		virtual CGeoPoint& operator-= (long l);
		virtual CGeoPoint& operator*= (long l);
		virtual CGeoPoint& operator/= (long l);
		virtual CGeoPoint& operator%= (long l);

		virtual CGeoPoint operator+ (const CGeoPoint& geoPoint) const;
		virtual CGeoPoint operator- (const CGeoPoint& geoPoint) const;
		virtual CGeoPoint operator-() const;

		inline long x() const throw() { return m_lX; }
		inline void x(long lX) throw() { m_lX = lX; }

		inline long y() const throw() { return m_lY; }
		inline void y(long lY) throw() { m_lY = lY; }

		virtual void clear();

		// Used by Sort Algorithms, operator<
		bool operator< (const CGeoPoint& geoPoint) const;

	private:
		long m_lX;
		long m_lY;
	};
} // namespace geo

#endif /*_GEO_POINT_H_INCLUDED_*/
