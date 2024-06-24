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
#include <iterator>
#include "GeoRoute.h"

using namespace geo;

CGeoRoute::CGeoRoute() :
	m_vehicleType(GeoVehicleType::Default),
	m_gRouteOptions(stdx::make_clone<CGeoRouteOptions>())
{
}

CGeoRoute::CGeoRoute(GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& gRouteOptions) :
	m_vehicleType(vehicleType),
	m_gRouteOptions(gRouteOptions)
{
}

CGeoRoute::CGeoRoute(const CGeoRoute& gRoute) : GeoSteps(gRoute),
m_vehicleType(gRoute.m_vehicleType),
m_cgLocations(gRoute.m_cgLocations),
m_gSummary(gRoute.m_gSummary),
m_gPolyLine(gRoute.m_gPolyLine),
m_gRouteOptions(gRoute.m_gRouteOptions)
{
}

CGeoRoute::CGeoRoute(CGeoRoute&& gRoute) : GeoSteps(std::move(gRoute)),
m_vehicleType(std::move(gRoute.m_vehicleType)),
m_cgLocations(std::move(gRoute.m_cgLocations)),
m_gSummary(std::move(gRoute.m_gSummary)),
m_gPolyLine(std::move(gRoute.m_gPolyLine)),
m_gRouteOptions(std::move(gRoute.m_gRouteOptions))
{
}

CGeoRoute& CGeoRoute::operator+=(const CGeoRoute& gRoute)
{
	GeoSteps::insert(end(), gRoute.begin(), gRoute.end());

	m_gSummary += gRoute.m_gSummary;
	m_gPolyLine += gRoute.m_gPolyLine;
	m_cgLocations += gRoute.m_cgLocations;

	if (m_vehicleType != gRoute.m_vehicleType)
		m_vehicleType = GeoVehicleType::Default;

	m_gRouteOptions->merge(*gRoute.m_gRouteOptions);
	return *this;
}

CGeoRoute& CGeoRoute::operator+=(CGeoRoute&& gRoute)
{
	for (CGeoRoute::iterator it = gRoute.begin(); it != gRoute.end(); ++it)
		GeoSteps::push_back(std::move(*it));

	m_gSummary += std::move(gRoute.m_gSummary);
	m_gPolyLine += std::move(gRoute.m_gPolyLine);
	m_cgLocations += std::move(gRoute.m_cgLocations);

	if (m_vehicleType != gRoute.m_vehicleType)
		m_vehicleType = GeoVehicleType::Default;

	// Merging route options
	m_gRouteOptions->merge(*gRoute.m_gRouteOptions);

	return *this;
}

CGeoRoute& CGeoRoute::operator=(const CGeoRoute& gRoute)
{
	GeoSteps::operator=(gRoute);

	m_vehicleType = gRoute.m_vehicleType;
	m_cgLocations = gRoute.m_cgLocations;
	m_gSummary = gRoute.m_gSummary;
	m_gPolyLine = gRoute.m_gPolyLine;
	m_gRouteOptions = gRoute.m_gRouteOptions;

	return *this;
}

CGeoRoute& CGeoRoute::operator=(CGeoRoute&& gRoute)
{
	GeoSteps::operator=(std::move(gRoute));

	m_vehicleType = std::move(gRoute.m_vehicleType);
	m_cgLocations = std::move(gRoute.m_cgLocations);
	m_gSummary = std::move(gRoute.m_gSummary);
	m_gPolyLine = std::move(gRoute.m_gPolyLine);
	m_gRouteOptions = std::move(gRoute.m_gRouteOptions);

	return *this;
}

void CGeoRoute::clear() throw()
{
	m_vehicleType = GeoVehicleType::Default;
	m_cgLocations.clear();
	m_gSummary.clear();
	m_gPolyLine.clear();
	m_gRouteOptions = stdx::make_clone<CGeoRouteOptions>();

	GeoSteps::clear();
}

bool CGeoRoute::operator< (const CGeoRoute& gRoute) const
{
	if (m_gRouteOptions != gRoute.m_gRouteOptions)
		return m_gRouteOptions < gRoute.m_gRouteOptions;

	return m_cgLocations < gRoute.m_cgLocations;
}

CGeoRoute geo::operator+(const CGeoRoute& gRoute1, const CGeoRoute& gRoute2)
{
	CGeoRoute gRoute(gRoute1);
	gRoute += gRoute2;

	return gRoute;
}
