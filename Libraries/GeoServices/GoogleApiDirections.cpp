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

#include <chrono>
#include <thread>
#include "GoogleApiDirections.h"
#include "GoogleTools.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

namespace
{
	class CGoogleApiAcceptedRouteOptions : public CGeoBasicAcceptedRoadRouteOptions
	{
	public:
		~CGoogleApiAcceptedRouteOptions() final = default;

		GeoParameter<bool> highway() const final { return { true, true }; }
		GeoParameter<bool> tolls() const final { return { true, true }; }
		GeoParameter<bool> boatFerry() const final { return { true, true }; }
	};

	const std::string methodAvoidHighway("highways");
	const std::string methodAvoidTolls("tolls");
	const std::string methodAvoidFerries("ferries");
	const std::string methodDriving("driving");
	const std::string methodWalking("walking");
	const std::string methodBicycling("bicycling");

	const std::string directionRequest("https://maps.googleapis.com/maps/api/directions/json?sensor=false&units=metric");
	const std::string directionKey("&key=");
	const std::string directionLanguage("&language=");
	const std::string directionOrigin("&origin=");
	const std::string directionDest("&destination=");
	const std::string directionMode("&mode=");
	const std::string directionAvoid("&avoid=");
	const std::string directionWaypoints("&waypoints=");

	std::chrono::time_point<std::chrono::system_clock> g_lastApiCall(std::chrono::system_clock::now());

	// With a API Key, limited to 10 API calls per second
	constexpr std::chrono::milliseconds minRequestduration(100);
	constexpr size_t maxRequestStep = 23;
}

GeoRouteTravelOptions CGoogleApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;

	gRouteTravelOptions[GeoVehicleType::Bike] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Pedestrian] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<CGoogleApiAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

size_t CGoogleApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE CGoogleApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << directionRequest;

	if (!providerApi.getLang().empty())
		ossUrl << directionLanguage << providerApi.getLang();

	ossUrl << directionKey << providerApi.getKey();

	switch (vehicleType)
	{
	case GeoVehicleType::Pedestrian:
		ossUrl << directionMode << methodWalking;
		break;

	case GeoVehicleType::Bike:
		ossUrl << directionMode << methodBicycling;
		break;

	case GeoVehicleType::Car:
	default:
		ossUrl << directionMode << methodDriving;
		break;
	}

	if (cgOptions.typeIs(GeoRouteOptionsType::Road))
	{
		const CGeoRoadRouteOptions& cgRoadOptions = static_cast<const CGeoRoadRouteOptions&>(cgOptions);
		std::string strAvoid;

		if (!cgRoadOptions.takeHighway())
			strAvoid = methodAvoidHighway;

		if (!cgRoadOptions.takeTolls())
		{
			if (!strAvoid.empty())
				strAvoid += '|';

			strAvoid += methodAvoidTolls;
		}

		if (!cgRoadOptions.takeBoatFerry())
		{
			if (!strAvoid.empty())
				strAvoid += '|';

			strAvoid += methodAvoidFerries;
		}

		if (!strAvoid.empty())
			ossUrl << directionAvoid << strAvoid;
	}

	ossUrl << directionOrigin << CGoogleTools::fromLatLngToUrlValue(cgLatLngs.front()) << directionDest << CGoogleTools::fromLatLngToUrlValue(cgLatLngs.back());

	request.strReferrer = providerApi.getReferer();
	request.strUrl = ossUrl.str();

	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - g_lastApiCall);
	if (elapsed_ms < minRequestduration)
		std::this_thread::sleep_for(minRequestduration - elapsed_ms);

	g_lastApiCall = std::chrono::system_clock::now();

	return E_GEO_OK;
}

E_GEO_STATUS_CODE CGoogleApiDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		// Read status
		E_GEO_STATUS_CODE status = CGoogleTools::GetStatusCode(jsParser("status"));
		if (status != E_GEO_OK)
			return status;

		// Read legs
		const CJsonArray& jsLegs = jsParser("routes")[0]("legs");
		for (size_t i = 0; i < jsLegs.size(); i++)
		{
			const CJsonObject& jsLeg = jsLegs[i];

			vecRoutes.push_back(CGeoRoute(vehicleType, cgOptions));
			CGeoRoute& gRoute = vecRoutes.back();
			CGeoLatLng gLatLng;

			CGoogleTools::fromJsonToLatLng(jsLeg("start_location"), gLatLng);
			gRoute.locations().push_back(CGeoLocation(gLatLng, jsLeg("start_address")));

			CGoogleTools::fromJsonToLatLng(jsLeg("end_location"), gLatLng);
			gRoute.locations().push_back(CGeoLocation(gLatLng, jsLeg("end_address")));

			CGoogleTools::fromJsonToSummary(jsLeg, gRoute.summary());

			CGeoLatLngs& polylinePath = gRoute.polyline().getPath();
			const CJsonArray& jsSteps = jsLeg("steps");
			for (size_t j = 0; j < jsSteps.size(); j++)
			{
				const CJsonObject& jsStep = jsSteps[j];
				CGeoStep gStep;

				CGoogleTools::fromJsonToSummary(jsStep, gStep.summary());
				CGoogleTools::fromJsonToLatLng(jsStep("start_location"), gStep);
				gStep.instructions(jsStep("html_instructions"));

				gRoute.push_back(gStep);

				// Read polyline
				const CJsonValue& jsPolyLine = jsStep("polyline");
				CGeoPolyline gStepPolyline;
				gStepPolyline.fromEncoded(jsPolyLine("points"));

				polylinePath += gStepPolyline.getPath();
			}
		}
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return E_GEO_OK;
}
