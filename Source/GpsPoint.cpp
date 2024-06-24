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

#include "stdafx.h"
#include "GpsPoint.h"

CGpsPoint::CGpsPoint() :
	m_pcRouteInfo(nullptr)
{
}

CGpsPoint::CGpsPoint(const CGpsPoint& gpsPoint) :
	geo::CGeoLocation(gpsPoint),
	m_pcRouteInfo(nullptr)
{
	if (gpsPoint.m_pcRouteInfo)
		m_pcRouteInfo = new CRouteInfo(*(gpsPoint.m_pcRouteInfo));
}

CGpsPoint::CGpsPoint(const geo::CGeoLocation& gLocation) :
	geo::CGeoLocation(gLocation),
	m_pcRouteInfo(nullptr)
{
}

CGpsPoint::~CGpsPoint()
{
	clearRouteInfo();
}

CGpsPoint& CGpsPoint::operator=(const geo::CGeoLatLng& gLatLng)
{
	geo::CGeoLocation::operator=(gLatLng);
	clearRouteInfo();

	return *this;
}

CGpsPoint& CGpsPoint::operator=(const geo::CGeoLocation& gLocation)
{
	geo::CGeoLocation::operator=(gLocation);
	clearRouteInfo();

	return *this;
}

CGpsPoint& CGpsPoint::operator=(const CGpsPoint& gpsPoint)
{
	if (&gpsPoint == this)
		return *this;

	geo::CGeoLocation::operator=(gpsPoint);
	clearRouteInfo();

	if (gpsPoint.m_pcRouteInfo)
		m_pcRouteInfo = new CRouteInfo(*(gpsPoint.m_pcRouteInfo));

	return *this;
}

void CGpsPoint::clear()
{
	geo::CGeoLocation::clear();
	clearRouteInfo();
}

void CGpsPoint::setRouteInfo(const geo::CGeoRoute& gRoute)
{
	clearRouteInfo();
	m_pcRouteInfo = new CRouteInfo(gRoute);
}

void CGpsPoint::clearRouteInfo()
{
	delete m_pcRouteInfo;
	m_pcRouteInfo = nullptr;
}
