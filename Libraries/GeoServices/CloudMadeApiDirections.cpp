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
#include "CloudMadeApiDirections.h"
#include "CloudMadeTools.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

namespace
{
	class CCloudMadeApiAcceptedRouteOptions : public CGeoBasicAcceptedRoadRouteOptions
	{
	public:
		~CCloudMadeApiAcceptedRouteOptions() final = default;

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
	};

	constexpr size_t maxRequestStep = 25;

	const std::string methodDriving("/car");
	const std::string methodWalking("/foot");
	const std::string methodBicycling("/bicycle");
	const std::string itiQuickest("/fastest");
	const std::string itiShortest("/shortest");

	const std::string directionRequest("http://routes.cloudmade.com/");
	const std::string directionSteps("/api/0.3/");
	const std::string directionOutput(".js?units=km");
	const std::string directionLanguage("&lang=");
}

GeoRouteTravelOptions CCloudMadeApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;

	gRouteTravelOptions[GeoVehicleType::Bike] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Pedestrian] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<CCloudMadeApiAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

size_t CCloudMadeApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE CCloudMadeApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << directionRequest << providerApi.getKey() << directionSteps << CCloudMadeTools::fromLatLngToUrlValue(cgLatLngs.front());

	if (cgLatLngs.size() > 2)
	{
		CGeoLatLngs::const_iterator itBegin = cgLatLngs.begin();
		CGeoLatLngs::const_iterator itEnd = cgLatLngs.end();

		++itBegin;
		--itEnd;

		ossUrl << ",[";
		for (CGeoLatLngs::const_iterator it = itBegin; it != itEnd; ++it)
			ossUrl << ((it == itBegin) ? "" : ",") << CCloudMadeTools::fromLatLngToUrlValue(*it);
		ossUrl << "]";
	}
	ossUrl << "," << CCloudMadeTools::fromLatLngToUrlValue(cgLatLngs.back());

	switch (vehicleType)
	{
	case GeoVehicleType::Pedestrian:
		ossUrl << methodWalking;
		break;

	case GeoVehicleType::Bike:
		ossUrl << methodBicycling;
		break;

	case GeoVehicleType::Car:
	default:
		ossUrl << methodDriving;
		break;
	}

	switch (cgOptions.getItineraryType())
	{
	case GeoItineraryType::Quickest:
		ossUrl << itiQuickest;
		break;

	case GeoItineraryType::Shortest:
		ossUrl << itiShortest;
		break;

	default:
		break;
	}

	ossUrl << directionOutput;
	if (!providerApi.getLang().empty())
		ossUrl << directionLanguage << providerApi.getLang();

	request.strUrl = ossUrl.str();
	request.strReferrer = providerApi.getReferer();

	return E_GEO_OK;
}

E_GEO_STATUS_CODE CCloudMadeApiDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		E_GEO_STATUS_CODE status = CCloudMadeTools::GetStatusCode(jsParser("status"));
		if (status != E_GEO_OK)
			return status;

		// Read header
		CGeoRoute gRoute(vehicleType, cgOptions);

		// Create polyline
		CGeoLatLngs& gPathLatLngs = gRoute.polyline().getPath();
		const CJsonArray& jsCoordinatesArray = jsParser("route_geometry");
		for (size_t i = 0; i < jsCoordinatesArray.size(); i++)
		{
			CGeoLatLng gLatLng;
			CCloudMadeTools::fromJsonToLatLng(jsCoordinatesArray[i], gLatLng);
			gPathLatLngs.push_back(gLatLng);
		}

		if (gPathLatLngs.size() < 2)
			return E_GEO_ZERO_RESULTS;

		const CJsonObject& jsSummary = jsParser("route_summary");
		gRoute.summary().assign(jsSummary("total_distance"), jsSummary("total_time"));

		// Read Transit points
		gRoute.locations().push_back(gPathLatLngs.front());
		gRoute.locations().front().name(jsSummary("start_point"));

		if (jsSummary.exist("transit_points"))
		{
			const CJsonArray& jsPoints = jsSummary("transit_points");
			for (size_t i = 0; i < jsPoints.size(); i++)
			{
				CGeoLocation gLocation;
				CCloudMadeTools::fromJsonToLocation(jsPoints[i], gLocation);
				gRoute.locations().push_back(gLocation);
			}
		}

		gRoute.locations().push_back(gPathLatLngs.back());
		gRoute.locations().back().name(jsSummary("end_point"));

		// Read Steps
		const CJsonArray& jsSteps = jsParser("route_instructions");
		for (size_t i = 0; i < jsSteps.size(); i++)
		{
			const CJsonArray& jsStep = jsSteps[i];
			CGeoStep gStep;

			gStep.instructions(jsStep[0]);
			gStep.summary().assign(jsStep[1], jsStep[3]);
			gStep = gPathLatLngs[jsStep[2]];

			gRoute.push_back(gStep);
		}

		if (cgOptions.isLinked())
		{
			if (vecRoutes.empty())
				vecRoutes.push_back(gRoute);
			else
				vecRoutes.front() += gRoute;
		}
		else
		{
			vecRoutes.push_back(gRoute);
		}
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return E_GEO_OK;
}
