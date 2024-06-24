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
 * https://geoservices.ign.fr/
 */

#include <sstream>
#include "GeoPortalApiDirections.h"
#include "GeoPortalTools.h"
#include "jsonParser/JsonParser.h"
#include "stdx\string_helper.h"

using namespace geo;

namespace
{
	class CGeoPortalApiAcceptedRouteOptions : public CGeoBasicAcceptedRoadRouteOptions
	{
	public:
		~CGeoPortalApiAcceptedRouteOptions() final = default;

		GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const  final
		{
			return
			{
				std::set<GeoItineraryType::type_t>
				{
					GeoItineraryType::Quickest,
					GeoItineraryType::Shortest,
				},
				GeoItineraryType::Quickest
			};
		}

		GeoParameter<bool> tunnel() const final { return { true, true }; }
		GeoParameter<bool> tolls() const final { return { true, true }; }
	};

	constexpr size_t maxRequestStep = 2;

	const std::string methodAvoidTunnel("Tunnel");
	const std::string methodAvoidTolls("toll");
	const std::string methodDriving("Voiture");
	const std::string methodWalking("Pieton");
	const std::string optimizeDistance("DISTANCE");
	const std::string optimizeTime("TIME");

	const std::string directionRequest("http://wxs.ign.fr/");
	const std::string directionOutput("/itineraire/rest/route.json");
	const std::string directionOrigin("?origin=");
	const std::string directionDestination("&destination=");
	const std::string directionGraphName("&graphName=");
	const std::string directionAvoid("&exclusions=");
	const std::string directionOptimize("&method=");
	const std::string directionWaypoints("&waypoints=");
}

GeoRouteTravelOptions CGeoPortalApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;

	gRouteTravelOptions[GeoVehicleType::Pedestrian] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<CGeoPortalApiAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

size_t CGeoPortalApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE CGeoPortalApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << directionRequest << providerApi.getKey() << directionOutput;
	ossUrl << directionOrigin << CGeoPortalTools::fromLatLngToUrlValue(cgLatLngs.front()) << directionDestination << CGeoPortalTools::fromLatLngToUrlValue(cgLatLngs.back());

	if (cgLatLngs.size() > 2)
	{
		ossUrl << directionWaypoints;

		CGeoLatLngs::const_iterator itBegin = cgLatLngs.begin();
		CGeoLatLngs::const_iterator itEnd = cgLatLngs.end();
		++itBegin;
		--itEnd;

		for (CGeoLatLngs::const_iterator it = itBegin; it != itEnd; ++it)
			ossUrl << ((it != itBegin) ? ";" : "") << CGeoPortalTools::fromLatLngToUrlValue(*it);
	}

	switch (vehicleType)
	{
	case GeoVehicleType::Pedestrian:
		ossUrl << directionGraphName << methodWalking;
		break;

	case GeoVehicleType::Car:
	default:
		ossUrl << directionGraphName << methodDriving;
		break;
	}

	if (cgOptions.typeIs(GeoRouteOptionsType::Road))
	{
		const CGeoRoadRouteOptions& cgRoadOptions = static_cast<const CGeoRoadRouteOptions&>(cgOptions);
		size_t optCounter = 0;

		switch (cgRoadOptions.getItineraryType())
		{
		case GeoItineraryType::Quickest:
			ossUrl << directionOptimize << optimizeTime;
			break;

		case GeoItineraryType::Shortest:
			ossUrl << directionOptimize << optimizeDistance;
			break;

		default:
			break;
		}

		if (!cgRoadOptions.takeTunnel())
		{
			ossUrl << (optCounter ? "," : directionAvoid) << methodAvoidTunnel;
			++optCounter;
		}

		if (!cgRoadOptions.takeTolls())
		{
			ossUrl << (optCounter ? "," : directionAvoid) << methodAvoidTolls;
		}
	}

	request.strReferrer = providerApi.getReferer();
	request.strUrl = ossUrl.str();
	return E_GEO_OK;
}

E_GEO_STATUS_CODE CGeoPortalApiDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		E_GEO_STATUS_CODE status = CGeoPortalTools::GetStatusCode(jsParser("status"));
		if (status != E_GEO_OK)
			return status;

		vecRoutes.push_back(CGeoRoute(vehicleType, cgOptions));
		CGeoRoute& gRoute = vecRoutes.back();

		// Read Summary
		gRoute.summary().assign(stdx::string_helper::string_to<size_t>(jsParser("distanceMeters")), stdx::string_helper::string_to<size_t>(jsParser("durationSeconds")));

		// Read Geometry
		std::string strGeometry = jsParser("geometryWkt");

		size_t pos = strGeometry.find("LINESTRING");
		if (pos == std::string::npos)
			return E_GEO_INVALID_REQUEST;

		pos = strGeometry.find('(', pos);
		if (pos == std::string::npos)
			return E_GEO_INVALID_REQUEST;

		size_t endPos = strGeometry.rfind(')');
		if (endPos == std::string::npos)
			return E_GEO_INVALID_REQUEST;

		stdx::string_helper::vector vecStrResult;
		stdx::string_helper::split(strGeometry.substr(pos + 1, endPos - pos), vecStrResult, stdx::find_first_of(", "), false);

		if (vecStrResult.size() % 2 != 0)
			return E_GEO_INVALID_REQUEST;

		CGeoLatLngs& polylinePath = gRoute.polyline().getPath();
		auto it = vecStrResult.begin();
		while (it != vecStrResult.end())
		{
			double lng = stdx::string_helper::string_to<double>(*it);
			++it;
			double lat = stdx::string_helper::string_to<double>(*it);
			++it;

			polylinePath.emplace_back(CGeoLatLng(lat, lng));
		}

		if (polylinePath.size() < 2)
			return E_GEO_INVALID_REQUEST;

		gRoute.locations().push_back(polylinePath.front());
		gRoute.locations().push_back(polylinePath.back());
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return E_GEO_OK;
}
