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
 * https://msdn.microsoft.com/en-us/library/ff701705.aspx
 */

#include <sstream>
#include "BingApiDirections.h"
#include "BingTools.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

namespace
{
	class CBingApiAcceptedRouteOptions : public CGeoBasicAcceptedRoadRouteOptions
	{
	public:
		~CBingApiAcceptedRouteOptions() final = default;

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

		GeoParameter<bool> highway() const final { return { true, true }; }
		GeoParameter<bool> tolls() const final { return { true, true }; }
	};

	constexpr size_t maxRequestStep = 25;

	const std::string methodAvoidHighway("highways");
	const std::string methodAvoidTolls("tolls");
	const std::string methodDriving("Driving");
	const std::string methodWalking("Walking");
	const std::string optimizeDistance("distance");
	const std::string optimizeTime("time");

	const std::string directionRequest("http://dev.virtualearth.net/REST/V1/Routes/");
	const std::string directionOutput("?o=json&rpo=Points&du=km");
	const std::string directionKey("&key=");
	const std::string directionLanguage("&c=");
	const std::string directionAvoid("&avoid=");
	const std::string directionOptimize("&optmz=");
	const std::string directionWaypoints("&wp.");
}

GeoRouteTravelOptions CBingApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;

	gRouteTravelOptions[GeoVehicleType::Pedestrian] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<CBingApiAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

size_t CBingApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE CBingApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	// Build request
	ossUrl << directionRequest;

	switch (vehicleType)
	{
	case GeoVehicleType::Pedestrian:
	case GeoVehicleType::Bike:
		ossUrl << methodWalking;
		break;

	case GeoVehicleType::Car:
	default:
		ossUrl << methodDriving;
		break;
	}

	ossUrl << directionOutput;

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

		if (!cgRoadOptions.takeHighway())
		{
			ossUrl << (optCounter ? "," : directionAvoid) << methodAvoidHighway;
			++optCounter;
		}

		if (!cgRoadOptions.takeTolls())
		{
			ossUrl << (optCounter ? "," : directionAvoid) << methodAvoidTolls;
		}
	}

	CGeoLatLngs::const_iterator itBegin = cgLatLngs.begin();
	for (CGeoLatLngs::const_iterator it = itBegin; it != cgLatLngs.end(); ++it)
		ossUrl << directionWaypoints << std::distance(itBegin, it) << "=" << CBingTools::fromLatLngToUrlValue(*it);

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	ossUrl << directionKey << providerApi.getKey();
	if (!providerApi.getLang().empty())
		ossUrl << directionLanguage << providerApi.getLang();

	request.strUrl = ossUrl.str();
	request.strReferrer = providerApi.getReferer();

	return E_GEO_OK;
}

E_GEO_STATUS_CODE CBingApiDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		// Read status
		E_GEO_STATUS_CODE status = CBingTools::GetStatusCode(jsParser("statusCode"));
		if (status != E_GEO_OK)
			return status;

		const CJsonObject& jsRoute = jsParser("resourceSets")[0]("resources")[0];

		// Create polyline
		CGeoLatLngs gPathLatLngs;
		const CJsonArray& jsCoordinatesArray = jsRoute("routePath")("line")("coordinates");
		for (size_t i = 0; i < jsCoordinatesArray.size(); i++)
		{
			CGeoLatLng gLatLng;
			CBingTools::fromJsonToLatLng(jsCoordinatesArray[i], gLatLng);
			gPathLatLngs.push_back(gLatLng);
		}

		if (cgOptions.isLinked())
		{
			CGeoRoute gRoute(vehicleType, cgOptions);

			gRoute.polyline().setPath(gPathLatLngs);
			CBingTools::fromJsonToSummary(jsRoute, gRoute.summary());

			if (vecRoutes.empty())
				vecRoutes.push_back(gRoute);
			else
				vecRoutes.front() += gRoute;
		}

		// Read Route
		CGeoLatLngs::iterator itBeginStep = gPathLatLngs.begin();

		const CJsonArray& jsLegs = jsRoute("routeLegs");
		for (size_t i = 0; i < jsLegs.size(); i++)
		{
			const CJsonObject& jsLeg = jsLegs[i];
			CGeoRoute gRoute(vehicleType, cgOptions);
			size_t endPathIndice = 0;

			// Read Steps
			const CJsonArray& jsSteps = jsLeg("itineraryItems");
			for (size_t j = 0; j < jsSteps.size(); j++)
			{
				const CJsonObject& jsStep = jsSteps[j];
				CGeoStep gStep;

				CBingTools::fromJsonToSummary(jsStep, gStep.summary());
				CBingTools::fromJsonToLatLng(jsStep("maneuverPoint")("coordinates"), gStep);
				gStep.instructions(jsStep("instruction")("text"));
				endPathIndice = jsStep("details")[0]("endPathIndices")[0];

				if (cgOptions.isLinked())
					vecRoutes.front().push_back(gStep);
				else
					gRoute.push_back(gStep);
			}

			if (cgOptions.isLinked())
			{
				CGeoLocation geoStartLocation;
				CBingTools::fromJsonToLatLng(jsLeg("actualStart")("coordinates"), geoStartLocation);
				vecRoutes.front().locations().push_back(geoStartLocation);

				if (i == (jsLegs.size() - 1))
				{
					CGeoLocation geoEndLocation;
					CBingTools::fromJsonToLatLng(jsLeg("actualEnd")("coordinates"), geoEndLocation);
					vecRoutes.front().locations().push_back(geoEndLocation);
				}
			}
			else
			{
				CGeoLocation geoStartLocation;
				CBingTools::fromJsonToLatLng(jsLeg("actualStart")("coordinates"), geoStartLocation);
				gRoute.locations().push_back(geoStartLocation);

				CGeoLocation geoEndLocation;
				CBingTools::fromJsonToLatLng(jsLeg("actualEnd")("coordinates"), geoEndLocation);
				gRoute.locations().push_back(geoEndLocation);

				CBingTools::fromJsonToSummary(jsLeg, gRoute.summary());

				// Extraction of the polyline
				if (endPathIndice > 0)
				{
					CGeoLatLngs::iterator itEndStep = itBeginStep;
					std::advance(itEndStep, endPathIndice);

					CGeoLatLngs::iterator itEndRange = itEndStep;
					++itEndRange;

					gRoute.polyline().getPath().assign(itBeginStep, itEndRange);
					itBeginStep = itEndStep;
				}

				vecRoutes.push_back(gRoute);
			}
		}
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return E_GEO_OK;
}
