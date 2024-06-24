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

#include "GeoPoint.h"

using namespace geo;

CGeoPoint::CGeoPoint()
{
	clear();
}

CGeoPoint::CGeoPoint(const CGeoPoint& geoPoint)
{
	operator=(geoPoint);
}

CGeoPoint::CGeoPoint(long lX, long lY) :
	m_lX(lX),
	m_lY(lY)
{
}

CGeoPoint::operator bool() const
{
	return !operator!();
}

bool CGeoPoint::operator!() const
{
	return (!m_lX && !m_lY);
}

bool CGeoPoint::operator==(const CGeoPoint& geoPoint) const
{
	return (m_lX == geoPoint.m_lX && m_lY == geoPoint.m_lY);
}

bool CGeoPoint::operator!=(const CGeoPoint& geoPoint) const
{
	return !operator==(geoPoint);
}

CGeoPoint& CGeoPoint::operator= (const CGeoPoint& geoPoint)
{
	m_lX = geoPoint.m_lX;
	m_lY = geoPoint.m_lY;

	return *this;
}

CGeoPoint& CGeoPoint::operator+= (const CGeoPoint& geoPoint)
{
	m_lX += geoPoint.m_lX;
	m_lY += geoPoint.m_lY;

	return *this;
}

CGeoPoint& CGeoPoint::operator-= (const CGeoPoint& geoPoint)
{
	m_lX -= geoPoint.m_lX;
	m_lY -= geoPoint.m_lY;

	return *this;
}

CGeoPoint CGeoPoint::operator+ (const CGeoPoint& geoPoint) const
{
	return CGeoPoint(m_lX + geoPoint.m_lX, m_lY + geoPoint.m_lY);
}

CGeoPoint CGeoPoint::operator- (const CGeoPoint& geoPoint) const
{
	return CGeoPoint(m_lX - geoPoint.m_lX, m_lY - geoPoint.m_lY);
}

CGeoPoint CGeoPoint::operator-() const
{
	return CGeoPoint(-m_lX, -m_lY);
}

CGeoPoint& CGeoPoint::operator+= (long l)
{
	m_lX += l;
	m_lY += l;

	return *this;
}

CGeoPoint& CGeoPoint::operator-= (long l)
{
	m_lX -= l;
	m_lY -= l;

	return *this;
}

CGeoPoint& CGeoPoint::operator*= (long l)
{
	m_lX *= l;
	m_lY *= l;

	return *this;
}

CGeoPoint& CGeoPoint::operator/= (long l)
{
	m_lX /= l;
	m_lY /= l;

	return *this;
}

CGeoPoint& CGeoPoint::operator%= (long l)
{
	m_lX %= l;
	m_lY %= l;

	return *this;
}

void CGeoPoint::clear()
{
	m_lX = 0;
	m_lY = 0;
}

bool CGeoPoint::operator< (const CGeoPoint& geoPoint) const
{
	if (m_lX == geoPoint.m_lX)
		return m_lY < geoPoint.m_lY;
	else
		return m_lX < geoPoint.m_lX;
}
