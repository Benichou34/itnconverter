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
#include <sstream>
#include "TomtomTools.h"
#include "TomtomApiDirections.h"
#include "jsonParser/JsonParser.h"
#include "stdx/string_helper.h"

using namespace geo;

namespace
{
	class CTomtomApiAcceptedRouteOptions : public CGeoBasicAcceptedRoadRouteOptions
	{
	public:
		~CTomtomApiAcceptedRouteOptions() final = default;

		GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const  final
		{
			return
			{
				std::set<GeoItineraryType::type_t>
				{
					GeoItineraryType::Quickest,
					GeoItineraryType::Shortest,
					GeoItineraryType::Economical
				},
				GeoItineraryType::Quickest
			};
		}

		GeoParameter<bool> tolls() const final { return { true, true }; }
		GeoParameter<bool> highway() const final { return { true, true }; }
		GeoParameter<bool> boatFerry() const final { return { true, true }; }
		GeoParameter<bool> dirtRoad() const final { return { true, true }; }
	};

	class CTomtomApiAcceptedThrillingOptions : public CGeoAcceptedThrillingRouteOptions
	{
	public:
		~CTomtomApiAcceptedThrillingOptions() final = default;

		bool preview() const final { return true; }
		GeoParameter<bool> linked() const final { return { true, true }; }
		GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const  final
		{
			return
			{
				std::set<GeoItineraryType::type_t>
				{
					GeoItineraryType::Quickest,
					GeoItineraryType::Shortest,
					GeoItineraryType::Economical,
					GeoItineraryType::Thrilling
				},
				GeoItineraryType::Quickest
			};
		}

		GeoParameter<bool> highway() const final { return { true, true }; }
		GeoParameter<bool> tolls() const final { return { true, true }; }
		GeoParameter<bool> boatFerry() const final { return { true, true }; }
		GeoParameter<bool> railFerry() const final { return { false, false }; }
		GeoParameter<bool> tunnel() const final { return { false, false }; }
		GeoParameter<bool> dirtRoad() const final { return { true, true }; }

		GeoParameter<std::set<degree_t>, degree_t> hilliness() const final { return { std::set<degree_t> {degree_low, degree_normal, degree_high}, degree_normal }; }
		GeoParameter<std::set<level_t>, level_t> windingness() const final { return { std::set<level_t> {level_low, level_normal, level_high}, level_normal }; }
		GeoParameter<bool> alreadyUsedRoads() const final { return { true, true }; }
	};

	constexpr size_t maxRequestStep = 50;

	const std::string directionRequest("https://api.tomtom.com/routing/1/calculateRoute/");
	const std::string directionKey("/json?key=");
	const std::string directionOptions("&traffic=false");
	const std::string directionLanguage("&language=");
	const std::string directionAvoid("&avoid=");
	const std::string directionRouteType("&routeType=");
	const std::string directionTravelMode("&travelMode=");
	const std::string directionHilliness("&hilliness=");
	const std::string directionWindingness("&windingness=");

	const std::string levelDegreeLow("low");
	const std::string levelNormal("normal");
	const std::string levelHigh("high");

	const std::string avoidTollRoads("tollRoads");
	const std::string avoidMotorways("motorways");
	const std::string avoidFerries("ferries");
	const std::string avoidUnpavedRoads("unpavedRoads");
	const std::string avoidCarpools("carpools");
	const std::string avoidAlreadyUsedRoads("alreadyUsedRoads");

	const std::string routeTypeFastest("fastest");
	const std::string routeTypeShortest("shortest");
	const std::string routeTypeEco("eco");
	const std::string routeTypeThrilling("thrilling");

	const std::string travelModeCar("car");
	const std::string travelModeTruck("truck");
	const std::string travelModeTaxi("taxi");
	const std::string travelModeBus("bus");
	const std::string travelModeVan("van");
	const std::string travelModeMotorcycle("motorcycle");
	const std::string travelModeBicycle("bicycle");
	const std::string travelModePedestrian("pedestrian");

	std::chrono::time_point<std::chrono::system_clock> g_lastApiCall(std::chrono::system_clock::now());
	constexpr std::chrono::milliseconds minRequestduration(210); // 5 API calls per second
}

GeoRouteTravelOptions CTomtomApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;

	gRouteTravelOptions[GeoVehicleType::Pedestrian] = std::make_unique<CTomtomApiAcceptedThrillingOptions>();
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<CTomtomApiAcceptedThrillingOptions>();
	gRouteTravelOptions[GeoVehicleType::Bike] = std::make_unique<CTomtomApiAcceptedThrillingOptions>();
	gRouteTravelOptions[GeoVehicleType::Campervan] = std::make_unique<CTomtomApiAcceptedThrillingOptions>();
	gRouteTravelOptions[GeoVehicleType::Bus] = std::make_unique<CTomtomApiAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Motorbike] = std::make_unique<CTomtomApiAcceptedThrillingOptions>();
	//	gRouteTravelOptions[GeoVehicleType::Truck] = std::make_unique<CTomtomApiAcceptedTruckRouteOptions>();

	return gRouteTravelOptions;
}

size_t CTomtomApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE CTomtomApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << directionRequest;

	for (CGeoLatLngs::const_iterator it = cgLatLngs.begin(); it != cgLatLngs.end(); ++it)
	{
		if (it != cgLatLngs.begin())
			ossUrl << ':';

		ossUrl << it->lat() << ',' << it->lng();
	}

	ossUrl << directionKey << providerApi.getKey() << directionOptions;
	if (providerApi.getLang().size())
		ossUrl << directionLanguage << providerApi.getLang();

	switch (vehicleType)
	{
	case GeoVehicleType::Car:
		ossUrl << directionTravelMode << travelModeCar;
		break;

	case GeoVehicleType::Truck:
		ossUrl << directionTravelMode << travelModeTruck;
		break;

	case GeoVehicleType::Pedestrian:
		ossUrl << directionTravelMode << travelModePedestrian;
		break;

	case GeoVehicleType::Bike:
		ossUrl << directionTravelMode << travelModeBicycle;
		break;

	case GeoVehicleType::Motorbike:
		ossUrl << directionTravelMode << travelModeMotorcycle;
		break;

	case GeoVehicleType::Campervan:
		ossUrl << directionTravelMode << travelModeVan;
		break;

	case GeoVehicleType::Bus:
		ossUrl << directionTravelMode << travelModeBus;
		break;

	default:
		break;
	}

	if (cgOptions.typeIs(GeoRouteOptionsType::Road))
	{
		const CGeoRoadRouteOptions& cgRoadOptions = static_cast<const CGeoRoadRouteOptions&>(cgOptions);

		switch (cgRoadOptions.getItineraryType())
		{
		case GeoItineraryType::Quickest:
			ossUrl << directionRouteType << routeTypeFastest;
			break;

		case GeoItineraryType::Shortest:
			ossUrl << directionRouteType << routeTypeShortest;
			break;

		case GeoItineraryType::Economical:
			ossUrl << directionRouteType << routeTypeEco;
			break;

		case GeoItineraryType::Thrilling:
			ossUrl << directionRouteType << routeTypeThrilling;
			break;

		default:
			break;
		}

		if (!cgRoadOptions.takeHighway())
			ossUrl << directionAvoid << avoidMotorways;
		if (!cgRoadOptions.takeTolls())
			ossUrl << directionAvoid << avoidTollRoads;
		if (!cgRoadOptions.takeBoatFerry())
			ossUrl << directionAvoid << avoidFerries;
		if (!cgRoadOptions.takeDirtRoad())
			ossUrl << directionAvoid << avoidUnpavedRoads;
	}

	if (cgOptions.typeIs(GeoRouteOptionsType::Thrilling) && cgOptions.getItineraryType() == GeoItineraryType::Thrilling)
	{
		const CGeoThrillingRouteOptions& cgThrillingOptions = static_cast<const CGeoThrillingRouteOptions&>(cgOptions);

		if (!cgThrillingOptions.takeAlreadyUsedRoads())
			ossUrl << directionAvoid << avoidAlreadyUsedRoads;

		switch (cgThrillingOptions.getHilliness())
		{
		case CGeoAcceptedThrillingRouteOptions::degree_low:
			ossUrl << directionHilliness << levelDegreeLow;
			break;

		case CGeoAcceptedThrillingRouteOptions::degree_normal:
			ossUrl << directionHilliness << levelNormal;
			break;

		case CGeoAcceptedThrillingRouteOptions::degree_high:
			ossUrl << directionHilliness << levelHigh;
			break;

		default:
			break;
		}

		switch (cgThrillingOptions.getWindingness())
		{
		case CGeoAcceptedThrillingRouteOptions::level_low:
			ossUrl << directionWindingness << levelDegreeLow;
			break;

		case CGeoAcceptedThrillingRouteOptions::level_normal:
			ossUrl << directionWindingness << levelNormal;
			break;

		case CGeoAcceptedThrillingRouteOptions::level_high:
			ossUrl << directionWindingness << levelHigh;
			break;

		default:
			break;
		}
	}

	request.strReferrer = providerApi.getReferer();
	request.strUrl = ossUrl.str();

	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - g_lastApiCall);
	if (elapsed_ms < minRequestduration)
		std::this_thread::sleep_for(minRequestduration - elapsed_ms);

	g_lastApiCall = std::chrono::system_clock::now();

	return E_GEO_OK;
}

E_GEO_STATUS_CODE CTomtomApiDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		// Read header
		const CJsonObject& jsRoute = jsParser("routes")[0];

		if (cgOptions.isLinked())
		{
			CGeoRoute gRoute(vehicleType, cgOptions);

			// Read summary
			const CJsonObject& jsSummary = jsRoute("summary");
			gRoute.summary().assign(jsSummary("lengthInMeters"), jsSummary("travelTimeInSeconds"));

			// Create polyline
			CGeoLatLngs& gPathLatLngs = gRoute.polyline().getPath();

			const CJsonArray& jsLegs = jsRoute("legs");
			for (size_t i = 0; i < jsLegs.size(); ++i)
			{
				const CJsonArray& jsPoints = jsLegs[i]("points");
				for (size_t j = 0; j < jsPoints.size(); ++j)
				{
					CGeoLatLng geoLatLng;
					CTomtomTools::fromJsonToLatLng(jsPoints[j], geoLatLng);
					gPathLatLngs.push_back(geoLatLng);
				}

				if (gPathLatLngs.size() > 1)
				{
					if (gRoute.locations().empty())
						gRoute.locations().push_back(gPathLatLngs.front());

					gRoute.locations().push_back(gPathLatLngs.back());
				}
			}

			if (gPathLatLngs.size() < 2)
				return E_GEO_ZERO_RESULTS;

			if (vecRoutes.empty())
				vecRoutes.push_back(gRoute);
			else
				vecRoutes.front() += gRoute;
		}
		else
		{
			const CJsonArray& jsLegs = jsRoute("legs");
			for (size_t i = 0; i < jsLegs.size(); ++i)
			{
				CGeoRoute gRoute(vehicleType, cgOptions);

				// Read summary
				const CJsonObject& jsSummary = jsLegs[i]("summary");
				gRoute.summary().assign(jsSummary("lengthInMeters"), jsSummary("travelTimeInSeconds"));

				// Create polyline
				CGeoLatLngs& gPathLatLngs = gRoute.polyline().getPath();

				const CJsonArray& jsPoints = jsLegs[i]("points");
				for (size_t j = 0; j < jsPoints.size(); ++j)
				{
					CGeoLatLng geoLatLng;
					CTomtomTools::fromJsonToLatLng(jsPoints[j], geoLatLng);
					gPathLatLngs.push_back(geoLatLng);
				}

				if (gPathLatLngs.size() < 2)
					return E_GEO_ZERO_RESULTS;

				gRoute.locations().push_back(gPathLatLngs.front());
				gRoute.locations().push_back(gPathLatLngs.back());

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
