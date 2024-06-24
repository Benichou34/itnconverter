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

#if !defined(_GPSPOINT_H_INCLUDED_)
#define _GPSPOINT_H_INCLUDED_

#include "GeoServices/GeoServices.h"

class CRouteInfo
{
private:
	size_t m_ulCumulativeDistance;
	size_t m_ulCumulativeDuration;
	geo::CGeoRoute m_gRoute;

public:
	CRouteInfo(const geo::CGeoRoute& gRoute) :
		m_ulCumulativeDistance(0),
		m_ulCumulativeDuration(0),
		m_gRoute(gRoute) {}
	CRouteInfo(const CRouteInfo& cRouteInfo) :
		m_ulCumulativeDistance(cRouteInfo.m_ulCumulativeDistance),
		m_ulCumulativeDuration(cRouteInfo.m_ulCumulativeDuration),
		m_gRoute(cRouteInfo.m_gRoute) {}

	virtual ~CRouteInfo() {}

	virtual size_t cumulativeDistance() const throw() { return m_ulCumulativeDistance; }
	virtual size_t& cumulativeDistance() throw() { return m_ulCumulativeDistance; }

	virtual size_t cumulativeDuration() const throw() { return m_ulCumulativeDuration; }
	virtual size_t& cumulativeDuration() throw() { return m_ulCumulativeDuration; }

	virtual const geo::CGeoRoute& route() const throw() { return m_gRoute; }
	virtual geo::CGeoRoute& route() throw() { return m_gRoute; }

	virtual const geo::CGeoSummary& summary() const throw() { return m_gRoute.summary(); }
	virtual geo::CGeoSummary& summary() throw() { return m_gRoute.summary(); }

	virtual const geo::CGeoPolyline& polyline() const throw() { return m_gRoute.polyline(); }
	virtual geo::CGeoPolyline& polyline() throw() { return m_gRoute.polyline(); }
};

class CGpsPoint : public geo::CGeoLocation
{
private:
	CRouteInfo* m_pcRouteInfo;

public:
	CGpsPoint();
	CGpsPoint(const CGpsPoint& gpsPoint);
	CGpsPoint(const geo::CGeoLocation& gLocation);
	virtual ~CGpsPoint();

	CGpsPoint& operator=(const geo::CGeoLatLng& gLatLng);
	CGpsPoint& operator=(const geo::CGeoLocation& gLocation);
	CGpsPoint& operator=(const CGpsPoint& gpsPoint);

	virtual void clear();

	virtual void setRouteInfo(const geo::CGeoRoute& gRoute);
	virtual CRouteInfo* routeInfo() throw() { return m_pcRouteInfo; }
	virtual void clearRouteInfo();
};

#endif // !defined(_GPSPOINT_H_INCLUDED_)
