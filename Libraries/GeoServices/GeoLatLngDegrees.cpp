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
#include <cmath>
#include "GeoLatLngDegrees.h"

using namespace geo;

namespace
{
	CGeoLatLngDegrees::S_DM ddToDm(double dd)
	{
		CGeoLatLngDegrees::S_DM dm;

		dm.bDirection = (dd < 0);
		dd = std::abs(dd);
		dm.lDegrees = static_cast<long>(dd);
		dd -= dm.lDegrees;
		dm.dMinutes = dd * 60;

		return dm;
	}

	CGeoLatLngDegrees::S_DMS ddToDms(double dd)
	{
		CGeoLatLngDegrees::S_DM dm = ddToDm(dd);
		CGeoLatLngDegrees::S_DMS dms;

		dms.bDirection = dm.bDirection;
		dms.lDegrees = dm.lDegrees;
		dms.lMinutes = static_cast<long>(dm.dMinutes);
		dm.dMinutes -= dms.lMinutes;
		dms.dSeconds = dm.dMinutes * 60;

		return dms;
	}

	double dmToDD(const CGeoLatLngDegrees::S_DM& dm)
	{
		double dd = static_cast<double>(std::abs(dm.lDegrees)) + std::abs(dm.dMinutes) / 60;
		if (dm.bDirection)
			dd = -dd;
		return dd;
	}

	double dmsToDD(const CGeoLatLngDegrees::S_DMS& dms)
	{
		double dd = static_cast<double>(std::abs(dms.lDegrees)) + static_cast<double>(std::abs(dms.lMinutes)) / 60 + std::abs(dms.dSeconds) / 3600;
		if (dms.bDirection)
			dd = -dd;
		return dd;
	}
}

CGeoLatLngDegrees::CGeoLatLngDegrees()
{
}

CGeoLatLngDegrees::CGeoLatLngDegrees(const CGeoLatLngDegrees& gLatLngDegrees) :
	m_geoLatLng(gLatLngDegrees.m_geoLatLng)
{
}

CGeoLatLngDegrees::CGeoLatLngDegrees(const CGeoLatLng& gLatLng) :
	m_geoLatLng(gLatLng)
{
}

CGeoLatLngDegrees::CGeoLatLngDegrees(double dLat, double dLng)
{
	assign(dLat, dLng);
}

CGeoLatLngDegrees::CGeoLatLngDegrees(const S_DM& dmLat, const S_DM& dmLng)
{
	assign(dmLat, dmLng);
}

CGeoLatLngDegrees::CGeoLatLngDegrees(const S_DMS& dmsLat, const S_DMS& dmsLng)
{
	assign(dmsLat, dmsLng);
}

CGeoLatLngDegrees& CGeoLatLngDegrees::operator= (const CGeoLatLngDegrees& gLatLngDegrees)
{
	m_geoLatLng = gLatLngDegrees.m_geoLatLng;
	return *this;
}

CGeoLatLngDegrees& CGeoLatLngDegrees::operator= (const CGeoLatLng& gLatLng)
{
	m_geoLatLng = gLatLng;
	return *this;
}

void CGeoLatLngDegrees::assign(double dLat, double dLng)
{
	m_geoLatLng.coords(dLat, dLng);
}

void CGeoLatLngDegrees::assign(const S_DM& dmLat, const S_DM& dmLng)
{
	m_geoLatLng.coords(dmToDD(dmLat), dmToDD(dmLng));
}

void CGeoLatLngDegrees::assign(const S_DMS& dmsLat, const S_DMS& dmsLng)
{
	m_geoLatLng.coords(dmsToDD(dmsLat), dmsToDD(dmsLng));
}

CGeoLatLngDegrees::S_DM CGeoLatLngDegrees::dmLat() const
{
	return ddToDm(m_geoLatLng.lat());
}

CGeoLatLngDegrees::S_DM CGeoLatLngDegrees::dmLng() const
{
	return ddToDm(m_geoLatLng.lng());
}

CGeoLatLngDegrees::S_DMS CGeoLatLngDegrees::dmsLat() const
{
	return ddToDms(m_geoLatLng.lat());
}

CGeoLatLngDegrees::S_DMS CGeoLatLngDegrees::dmsLng() const
{
	return ddToDms(m_geoLatLng.lng());
}

const CGeoLatLng& CGeoLatLngDegrees::latLng() const
{
	return m_geoLatLng;
}
