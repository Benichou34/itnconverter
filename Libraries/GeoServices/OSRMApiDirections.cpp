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
 *
 * http://project-osrm.org/
 * https://github.com/Project-OSRM/osrm-backend/blob/master/docs/http.md
 */

#include <sstream>
#include "OSRMApiDirections.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

namespace
{
	constexpr size_t maxRequestStep = 25;

	const std::string directionRequest("http://router.project-osrm.org/route/v1/");
	const std::string travelModeCar("driving/");
	const std::string fullOverview("?overview=full");
}

GeoRouteTravelOptions COSRMApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<CGeoBasicAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

size_t COSRMApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE COSRMApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t, const CGeoRouteOptions&, Request& request)
{
	std::ostringstream ossUrl;

	// Build request
	ossUrl << directionRequest << travelModeCar;

	for (CGeoLatLngs::const_iterator it = cgLatLngs.begin(); it != cgLatLngs.end(); ++it)
	{
		if (it != cgLatLngs.begin())
			ossUrl << ';';
		ossUrl << it->lng() << ',' << it->lat();

	}

	ossUrl << fullOverview;

	request.strUrl = ossUrl.str();
	return E_GEO_OK;
}

E_GEO_STATUS_CODE COSRMApiDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		if (static_cast<std::string>(jsParser("code")) != "Ok")
			return E_GEO_NOT_FOUND;

		CGeoRoute gRoute(vehicleType, cgOptions);

		const CJsonObject& jsRoute = jsParser("routes")[0];
		gRoute.summary().assign(jsRoute("distance"), jsRoute("duration")); // Read summary

		// Create polyline
		gRoute.polyline().fromEncoded(jsRoute("geometry"), 5);

		// Read waypoints
		const CJsonArray& jsWaypoints = jsParser("waypoints");
		for (size_t i = 0; i < jsWaypoints.size(); i++)
		{
			const CJsonArray& jsLocation = jsWaypoints[i]("location");

			CGeoLocation gLocation;
			gLocation.coords(jsLocation[1], jsLocation[0]);
			gLocation.name(jsWaypoints[i]("name"));

			gRoute.locations().push_back(std::move(gLocation));
		}

		if (gRoute.locations().size() < 2)
			return E_GEO_ZERO_RESULTS;

		if (cgOptions.isLinked())
		{
			if (vecRoutes.empty())
				vecRoutes.push_back(std::move(gRoute));
			else
				vecRoutes.front() += gRoute;
		}
		else
		{
			vecRoutes.push_back(std::move(gRoute));
		}
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return E_GEO_OK;
}
