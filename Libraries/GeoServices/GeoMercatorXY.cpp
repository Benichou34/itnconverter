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

#include <sstream>
#include <stdexcept>
#include <cmath>
#include "GeoMercatorXY.h"

using namespace geo;

#define EARTH_RADIUS 6371000            /* Earth radius in meter */
#define PI           3.1415926535897931 /* PI number */

// Convert radians to degrees
#define RAD2DEG(rad) (static_cast<double>(rad) * 57.295779513082320876798154814105)

CGeoMercatorXY::CGeoMercatorXY()
{
	clear();
}

CGeoMercatorXY::CGeoMercatorXY(const CGeoMercatorXY& gMercatorXY)
{
	operator=(gMercatorXY);
}

CGeoMercatorXY::CGeoMercatorXY(const CGeoLatLng& gLatLng)
{
	operator=(gLatLng);
}

CGeoMercatorXY::CGeoMercatorXY(int nX, int nY)
{
	m_nX = nX;
	m_nY = nY;
}

CGeoMercatorXY::operator CGeoLatLng() const
{
	return CGeoLatLng(RAD2DEG(2 * (atan(exp(static_cast<double>(m_nY) / EARTH_RADIUS)) - PI / 4)), RAD2DEG(static_cast<double>(m_nX) / EARTH_RADIUS));
}

CGeoMercatorXY::operator bool() const
{
	return !operator!();
}

bool CGeoMercatorXY::operator!() const
{
	return (!m_nX && !m_nY);
}

bool CGeoMercatorXY::operator==(const CGeoMercatorXY& gMercatorXY) const
{
	return (m_nX == gMercatorXY.m_nX && m_nY == gMercatorXY.m_nY);
}

bool CGeoMercatorXY::operator!=(const CGeoMercatorXY& gMercatorXY) const
{
	return !operator==(gMercatorXY);
}

CGeoMercatorXY& CGeoMercatorXY::operator= (const CGeoMercatorXY& gMercatorXY)
{
	m_nX = gMercatorXY.m_nX;
	m_nY = gMercatorXY.m_nY;

	return *this;
}

CGeoMercatorXY& CGeoMercatorXY::operator= (const CGeoLatLng& gLatLng)
{
	m_nX = std::lround(gLatLng.lngRadians() * EARTH_RADIUS);
	m_nY = std::lround(log(tan(gLatLng.latRadians() / 2 + PI / 4)) * EARTH_RADIUS);

	return *this;
}

void CGeoMercatorXY::clear()
{
	m_nX = 0;
	m_nY = 0;
}
