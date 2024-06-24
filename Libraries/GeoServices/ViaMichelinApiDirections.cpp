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
#include "ViaMichelinApiDirections.h"
#include "ViaMichelinTools.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

namespace
{
	class CViaMichelinApiAcceptedRouteOptions : public CGeoBasicAcceptedRoadRouteOptions
	{
	public:
		~CViaMichelinApiAcceptedRouteOptions() final = default;

		GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const  final
		{
			return
			{
				std::set<GeoItineraryType::type_t>
				{
					GeoItineraryType::Recommended,
					GeoItineraryType::Quickest,
					GeoItineraryType::Shortest,
					GeoItineraryType::Sightseeing,
					GeoItineraryType::Economical
				},
				GeoItineraryType::Recommended
			};
		}

		GeoParameter<bool> tolls() const final { return { true, true }; }
	};

	constexpr size_t maxRequestStep = 8;

	const std::string directionRequest("http://apir.viamichelin.com/apir/1/iti.json/");
	const std::string directionCallback("HTTPResponseLoaded");
	const std::string directionOutput("?charset=UTF-8&ie=UTF-8&distUnit=m&avoidBorders=true&avoidCCZ=false&avoidORC=false&multipleIti=false&itiIdx=0&callback=HTTPResponseLoaded&steps=");
	const std::string directionAuthKey("&authkey=");
	const std::string directionAvoidTolls("&avoidTolls=");
	const std::string directionVehicle("&veht=");
	const std::string directionItineraryType("&itit=");

	enum vehicle // Type of vehicle used by the itinerary calculation.
	{
		vehicle_car = 0,
		vehicle_truck = 1,
		vehicle_pedestrian = 2,
		vehicle_bike = 3,
		vehicle_motorbike = 4
	};

	enum itinerary // Type of itinerary to be calculated.
	{
		iti_recommended = 0, // Focuses on road safety and comfort while offering a good compromise between time and distance.
		iti_quickest = 1,    // Quickest provides the fastest route by emphasising motorways and major roads.
		iti_shortest = 2,    // Shortest optimises distance.
		iti_sightseeing = 3, // Sightseeing focuses on scenic road that may have a touristic interest.
		iti_economical = 4   // Economical takes into consideration fuel consumption costs and tolls as well as the route distance.
	};
}

GeoRouteTravelOptions CViaMichelinApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;

	gRouteTravelOptions[GeoVehicleType::Pedestrian] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Bike] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<CViaMichelinApiAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Truck] = std::make_unique<CViaMichelinApiAcceptedRouteOptions>();
	//	gRouteTravelOptions[GeoVehicleType::Motorbike] = std::make_unique<CViaMichelinApiAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

size_t CViaMichelinApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE CViaMichelinApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << directionRequest << providerApi.getLang() << directionOutput;

	for (CGeoLatLngs::const_iterator it = cgLatLngs.begin(); it != cgLatLngs.end(); ++it)
		ossUrl << "1:" << (cgOptions.isLinked() ? "p" : "e") << ":" << CViaMichelinTools::fromLatLngToApiUrlValue(*it) << ";";

	switch (vehicleType)
	{
	case GeoVehicleType::Car:
		ossUrl << directionVehicle << vehicle_car;
		break;

	case GeoVehicleType::Truck:
		ossUrl << directionVehicle << vehicle_truck;
		break;

	case GeoVehicleType::Pedestrian:
		ossUrl << directionVehicle << vehicle_pedestrian;
		break;

	case GeoVehicleType::Bike:
		ossUrl << directionVehicle << vehicle_bike;
		break;

	case GeoVehicleType::Motorbike:
		ossUrl << directionVehicle << vehicle_motorbike;
		break;

	default:
		ossUrl << directionVehicle << vehicle_car;
		break;
	}

	switch (cgOptions.getItineraryType())
	{
	case GeoItineraryType::Recommended:
		ossUrl << directionItineraryType << iti_recommended;
		break;

	case GeoItineraryType::Quickest:
		ossUrl << directionItineraryType << iti_quickest;
		break;

	case GeoItineraryType::Shortest:
		ossUrl << directionItineraryType << iti_shortest;
		break;

	case GeoItineraryType::Sightseeing:
		ossUrl << directionItineraryType << iti_sightseeing;
		break;

	case GeoItineraryType::Economical:
		ossUrl << directionItineraryType << iti_economical;
		break;

	default:
		break;
	}

	if (cgOptions.typeIs(GeoRouteOptionsType::Road))
		ossUrl << directionAvoidTolls << (static_cast<const CGeoRoadRouteOptions&>(cgOptions).takeTolls() ? "false" : "true");
	ossUrl << directionAuthKey << providerApi.getKey();

	request.strUrl = ossUrl.str();
	return E_GEO_OK;
}

E_GEO_STATUS_CODE CViaMichelinApiDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		size_t pos = strRequest.find(directionCallback);
		if (pos == std::string::npos)
			return E_GEO_INVALID_REQUEST;

		CJsonParser jsParser;
		jsParser << strRequest.substr(pos + directionCallback.size());

		if (jsParser.exist("error"))
			return CViaMichelinTools::GetStatusCode(jsParser("error")("errorCode"));

		// Read header
		CGeoRoute gRoute(vehicleType, cgOptions);
		CViaMichelinTools::fromJsonToSummary(jsParser("header"), gRoute.summary());

		// Read Steps
		const CJsonArray& jsSteps = jsParser("roadSheet")[0];
		for (size_t j = 0; j < jsSteps.size(); j++)
		{
			const CJsonArray& jsStep = jsSteps[j];
			if (static_cast<const std::string&>(jsStep[0]) == "C")
			{
				CGeoStep gStep;

				gStep.summary().assign(jsStep[2], jsStep[3]);

				gStep.lng(jsStep[1]("lon"));
				gStep.lat(jsStep[1]("lat"));
				gStep.comment(jsStep[7]);
				gStep.name(jsStep[6]);

				if (jsStep[8].getType() == CJsonValue::JSON_TYPE_STRING)
				{
					const std::string& street = jsStep[8];
					if (!street.empty())
						gStep.name(gStep.name() + std::string(", ") + street);
				}

				if (!static_cast<int>(jsStep[4]) && !static_cast<int>(jsStep[5]))
					gRoute.locations().push_back(gStep);

				gRoute.push_back(gStep);
			}
		}

		// Create polyline
		CGeoLatLngs& gPathLatLngs = gRoute.polyline().getPath();
		CViaMichelinTools::getCoordsFromString(jsParser("itineraryTrace"), gPathLatLngs);
		if (!gPathLatLngs.empty() && !gRoute.locations().empty())
			gPathLatLngs.push_front(gRoute.locations().front());

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
