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
#include "ManualDirections.h"
#include "stdx/guard.h"

using namespace geo;

CManualDirections::CManualDirections() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

GeoRouteTravelOptions CManualDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;
	gRouteTravelOptions[GeoVehicleType::Default] = std::make_unique<CGeoBasicAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

void CManualDirections::Load(const CGeoLatLng& gStart, const CGeoLatLng& gStop, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions)
{
	CGeoLatLngs cgLatLngs;

	cgLatLngs.push_back(gStart);
	cgLatLngs.push_back(gStop);

	Load(cgLatLngs, vehicleType, cgOptions);
}

void CManualDirections::Load(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions)
{
	stdx::function_guard endCallback([&]()
		{
			if (m_EndCallback)
				m_EndCallback(m_eStatus, m_vecRoutes);
		});

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_vecRoutes.clear();

	if (cgLatLngs.size() < 2)
	{
		m_eStatus = E_GEO_BAD_ARGUMENTS;
		return;
	}

	if (vehicleType != GeoVehicleType::Default)
	{
		m_eStatus = E_GEO_INVALID_REQUEST;
		return;
	}

	if (cgOptions.isLinked())
	{
		CGeoRoute gRoute(vehicleType, cgOptions);

		gRoute.locations().assign(cgLatLngs.begin(), cgLatLngs.end());
		gRoute.polyline().setPath(cgLatLngs);

		size_t ulDistance = 0;
		for (size_t i = 1; i < cgLatLngs.size(); ++i)
			ulDistance += cgLatLngs[i - 1].distanceFrom(cgLatLngs[i]);

		gRoute.summary().assign(ulDistance, 0);

		m_vecRoutes.push_back(gRoute);
	}
	else
	{
		for (size_t i = 1; i < cgLatLngs.size(); ++i)
		{
			const CGeoLatLng& gLatLng1 = cgLatLngs[i - 1];
			const CGeoLatLng& gLatLng2 = cgLatLngs[i];
			CGeoRoute gRoute(vehicleType, cgOptions);

			gRoute.locations().push_back(gLatLng1);
			gRoute.locations().push_back(gLatLng2);
			gRoute.summary().assign(gLatLng1.distanceFrom(gLatLng2), 0);

			CGeoLatLngs& cgPolyLatLngs = gRoute.polyline().getPath();
			cgPolyLatLngs.push_back(gLatLng1);
			cgPolyLatLngs.push_back(gLatLng2);

			m_vecRoutes.push_back(gRoute);
		}
	}

	m_eStatus = m_vecRoutes.empty() ? E_GEO_ZERO_RESULTS : E_GEO_OK;
}
