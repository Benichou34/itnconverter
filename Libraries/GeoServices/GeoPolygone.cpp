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

#include <cmath>
#include "GeoPolygone.h"

using namespace geo;

CGeoPolygone::CGeoPolygone()
{
}

CGeoPolygone::CGeoPolygone(const CGeoLatLngs& geoLatLngs) :
	m_geoLatLngs(geoLatLngs)
{
}

CGeoPolygone::CGeoPolygone(CGeoLatLngs&& geoLatLngs) :
	m_geoLatLngs(std::move(geoLatLngs))
{
}

CGeoPolygone::CGeoPolygone(const CGeoPolygone& gPolygone) :
	m_geoLatLngs(gPolygone.m_geoLatLngs)
{
}

CGeoPolygone::CGeoPolygone(CGeoPolygone&& gPolygone) :
	m_geoLatLngs(std::move(gPolygone.m_geoLatLngs))
{
}

CGeoPolygone& CGeoPolygone::operator=(const CGeoPolygone& gPolygone)
{
	m_geoLatLngs = gPolygone.m_geoLatLngs;
	return *this;
}

CGeoPolygone& CGeoPolygone::operator=(CGeoPolygone&& gPolygone)
{
	m_geoLatLngs = std::move(gPolygone.m_geoLatLngs);
	return *this;
}

bool CGeoPolygone::contains(const CGeoLatLng& geoLatLng) const
{
	if (m_geoLatLngs.size() < 3)
		return false;

	/* Use the "Crossing Count" Algorithm by Bob Stein from http://www.visibone.com/inpoly/ */
	bool bContains = false;
	const CGeoLatLng* pGeoLatLngOld = &m_geoLatLngs.back();
	const CGeoLatLng* pGeoLatLng1;
	const CGeoLatLng* pGeoLatLng2;
	CGeoLatLngs::const_iterator cit;

	for (cit = m_geoLatLngs.begin(); cit != m_geoLatLngs.end(); ++cit)
	{
		const CGeoLatLng& geoLatLngNew = *cit;

		if (geoLatLngNew.lng() > pGeoLatLngOld->lng())
		{
			pGeoLatLng1 = pGeoLatLngOld;
			pGeoLatLng2 = &geoLatLngNew;
		}
		else
		{
			pGeoLatLng1 = &geoLatLngNew;
			pGeoLatLng2 = pGeoLatLngOld;
		}

		if ((geoLatLngNew.lng() < geoLatLng.lng()) == (geoLatLng.lng() <= pGeoLatLngOld->lng()) /* edge "open" at one end */
			&& ((geoLatLng.lat() - pGeoLatLng1->lat()) * (pGeoLatLng2->lng() - pGeoLatLng1->lng())) < (pGeoLatLng2->lat() - pGeoLatLng1->lat()) * (geoLatLng.lng() - pGeoLatLng1->lng()))
			bContains = !bContains;

		pGeoLatLngOld = &geoLatLngNew;
	}

	return bContains;
}

void CGeoPolygone::clear() throw()
{
	m_geoLatLngs.clear();
}

CGeoLatLngs& CGeoPolygone::getPath()
{
	return m_geoLatLngs;
}

const CGeoLatLngs& CGeoPolygone::getPath() const
{
	return m_geoLatLngs;
}

void CGeoPolygone::setPath(const CGeoLatLngs& geoLatLngs)
{
	m_geoLatLngs = geoLatLngs;
}

void CGeoPolygone::setPath(CGeoLatLngs&& geoLatLngs)
{
	m_geoLatLngs = std::move(geoLatLngs);
}
