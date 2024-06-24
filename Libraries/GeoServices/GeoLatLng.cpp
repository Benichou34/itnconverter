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

#include <algorithm>
#include <cmath>
#include "GeoLatLng.h"

using namespace geo;

namespace
{
	constexpr double EPSILON = 0.0001; // Float comparaison tolerance
	constexpr size_t EARTH_RADIUS = 6378137; // Earth radius in meter

	inline bool isEquals(double d1, double d2, double dPrecision)
	{
		return (((d2 - dPrecision) < d1) && ((d2 + dPrecision) > d1));
	}

	inline double metersToRadians(double mtr) // Convert meters to radians
	{
		return mtr / EARTH_RADIUS;
	}

	inline double radiansToMeters(double rad) // Convert radians to meters
	{
		return rad * EARTH_RADIUS;
	}

	inline double degreesToRadians(double deg)// Convert degrees to radians
	{
		return deg / 57.295779513082320876798154814105;
	}
}

CGeoLatLng::CGeoLatLng() :
	m_dPrecision(EPSILON),
	m_dLatitude(0),
	m_dLongitude(0),
	m_dAltitude(0)
{
}

CGeoLatLng::CGeoLatLng(const CGeoLatLng& gLatLng) :
	m_dPrecision(gLatLng.m_dPrecision),
	m_dLatitude(gLatLng.m_dLatitude),
	m_dLongitude(gLatLng.m_dLongitude),
	m_dAltitude(gLatLng.m_dAltitude)
{
}

CGeoLatLng::CGeoLatLng(CGeoLatLng&& gLatLng) :
	m_dPrecision(gLatLng.m_dPrecision),
	m_dLatitude(gLatLng.m_dLatitude),
	m_dLongitude(gLatLng.m_dLongitude),
	m_dAltitude(gLatLng.m_dAltitude)
{
}

CGeoLatLng::CGeoLatLng(double dLatitude, double dLongitude, double dAltitude) :
	m_dPrecision(EPSILON),
	m_dLatitude(dLatitude),
	m_dLongitude(dLongitude),
	m_dAltitude(dAltitude)
{
}

CGeoLatLng::operator bool() const
{
	return !operator!();
}

bool CGeoLatLng::operator!() const
{
	return (!m_dLatitude && !m_dLongitude);
}

bool CGeoLatLng::operator==(const CGeoLatLng& gLatLng) const
{
	double dPrecision = std::max<double>(m_dPrecision, gLatLng.m_dPrecision);
	return (isEquals(m_dLatitude, gLatLng.m_dLatitude, dPrecision) && isEquals(m_dLongitude, gLatLng.m_dLongitude, dPrecision));
}

bool CGeoLatLng::operator!=(const CGeoLatLng& gLatLng) const
{
	return !operator==(gLatLng);
}

CGeoLatLng& CGeoLatLng::operator= (const CGeoLatLng& gLatLng)
{
	if (&gLatLng != this)
	{
		m_dPrecision = gLatLng.m_dPrecision;
		m_dLatitude = gLatLng.m_dLatitude;
		m_dLongitude = gLatLng.m_dLongitude;
		m_dAltitude = gLatLng.m_dAltitude;
	}

	return *this;
}

CGeoLatLng& CGeoLatLng::operator= (CGeoLatLng&& gLatLng)
{
	return operator=(gLatLng);
}

void CGeoLatLng::coords(double dLatitude, double dLongitude, double dAltitude)
{
	m_dLatitude = dLatitude;
	m_dLongitude = dLongitude;
	m_dAltitude = dAltitude;
}

void CGeoLatLng::wrap()
{
	if (m_dLatitude > 90)
		m_dLatitude = 90;
	if (m_dLatitude < -90)
		m_dLatitude = -90;
	if (m_dLongitude > 180)
		m_dLongitude = 180;
	if (m_dLongitude < -180)
		m_dLongitude = -180;
}

void CGeoLatLng::clear()
{
	m_dPrecision = EPSILON;
	m_dLatitude = 0;
	m_dLongitude = 0;
	m_dAltitude = 0;
}

double CGeoLatLng::latRadians() const
{
	return degreesToRadians(lat());
}

double CGeoLatLng::lngRadians() const
{
	return degreesToRadians(lng());
}

size_t CGeoLatLng::distanceFrom(const CGeoLatLng& gLatLng) const
{
	double dPrevLat, dPrevLng;
	double dLat, dLng;
	double dDistance;

	dPrevLat = latRadians();
	dPrevLng = lngRadians();

	dLat = gLatLng.latRadians();
	dLng = gLatLng.lngRadians();

	dDistance = sqrt(
		pow(cos(dLat) * cos(dLng) - cos(dPrevLat) * cos(dPrevLng), 2) +
		pow(sin(dLat) - sin(dPrevLat), 2) +
		pow(cos(dLat) * sin(dLng) - cos(dPrevLat) * sin(dPrevLng), 2));

	return static_cast<size_t>(radiansToMeters(dDistance));
}

bool CGeoLatLng::operator< (const CGeoLatLng& gLatLng) const
{
	if (isEquals(m_dLatitude, gLatLng.m_dLatitude, std::max<double>(m_dPrecision, gLatLng.m_dPrecision)))
		return m_dLongitude < gLatLng.m_dLongitude;
	else
		return m_dLatitude < gLatLng.m_dLatitude;
}
