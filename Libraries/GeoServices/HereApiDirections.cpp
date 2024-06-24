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
#include "HereApiDirections.h"
#include "HereApi.h"
#include "GeoRouteOptions.h"
#include "jsonParser/JsonParser.h"
#include "stdx/string_helper.h"

using namespace geo;

namespace
{
	class CHereApiAcceptedRoadRouteOptions : public CGeoBasicAcceptedRoadRouteOptions
	{
	public:
		~CHereApiAcceptedRoadRouteOptions() final = default;

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
		GeoParameter<bool> boatFerry() const final { return { true, true }; }
		GeoParameter<bool> railFerry() const final { return { true, true }; }
		GeoParameter<bool> tunnel() const final { return { true, true }; }
		GeoParameter<bool> dirtRoad() const final { return { true, true }; }
	};

	class CHereApiAcceptedTruckRouteOptions : public CGeoAcceptedTruckRouteOptions
	{
	public:
		~CHereApiAcceptedTruckRouteOptions() final = default;

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
				},
				GeoItineraryType::Quickest
			};
		}

		GeoParameter<bool> highway() const final { return { true, true }; }
		GeoParameter<bool> tolls() const final { return { true, true }; }
		GeoParameter<bool> boatFerry() const final { return { true, true }; }
		GeoParameter<bool> railFerry() const final { return { true, true }; }
		GeoParameter<bool> tunnel() const final { return { true, true }; }
		GeoParameter<bool> dirtRoad() const final { return { true, true }; }

		GeoParameter<bool> tractor() const final { return { true, false }; }
		GeoParameter<size_t> trailersCount() const final { return { 4, 0 }; } // The provided value must be between 0 and 4. Defaults to 0.
		GeoParameter<size_t> axleCount() const final { return { 0, 0 }; } // Not supported
		GeoParameter<std::set<GeoTruckCategoryType::type_t>, GeoTruckCategoryType::type_t> categories() const final
		{
			return
			{
				std::set<GeoTruckCategoryType::type_t>
				{
					GeoTruckCategoryType::no_catory,
					GeoTruckCategoryType::hazardous_explosive,
					GeoTruckCategoryType::hazardous_gas,
					GeoTruckCategoryType::hazardous_flammable,
					GeoTruckCategoryType::hazardous_combustible,
					GeoTruckCategoryType::hazardous_organic,
					GeoTruckCategoryType::hazardous_poison,
					GeoTruckCategoryType::hazardous_radioActive,
					GeoTruckCategoryType::hazardous_corrosive,
					GeoTruckCategoryType::hazardous_poisonousInhalation,
					GeoTruckCategoryType::hazardous_harmfulToWater,
					GeoTruckCategoryType::hazardous_other,
					GeoTruckCategoryType::hazardous_all
				},
				GeoTruckCategoryType::no_catory,
			};
		}
		GeoParameter<size_t> limitedWeight() const final { return { 10000, 30 }; } // The provided value must be between 0 and 10000.
		GeoParameter<size_t> weightPerAxle() const final { return { 10000, 50 }; } // The provided value must be between 0 and 10000.
		GeoParameter<size_t> height() const final { return { 500, 20 }; } // The provided value must be between 0 and 50.
		GeoParameter<size_t> width() const final { return { 500, 30 }; } // The provided value must be between 0 and 50.
		GeoParameter<size_t> length() const final { return { 3000, 80 }; } // The provided value must be between 0 and 300.
	};

	constexpr size_t maxRequestStep = 10;

	const std::string routingUrl("http://route.api.here.com/routing/7.2/calculateroute.json?metricsystem=metric&representation=overview&routeattributes=shape");
	const std::string routingAppId("&app_id=");
	const std::string routingAppCode("&app_code=");
	const std::string routingLanguage("&language=");
	const std::string routingMode("&mode="); // RoutingMode = Type ; [TransportModes] ; [TrafficMode] ; [Feature]
	const std::string routingTypeFastest("fastest"); // Routing favors the least amount of travel time, can consider traffic if enabled.
	const std::string routingTypeShortest("shortest"); // Routing favors the least amount of travel distance, disregards traffic.
	const std::string routingModeCar(";car"); // Routing for cars, adheres to link rules and restrictions(for example, speed limits, direction of travel, etc.), can consider traffic if enabled
	const std::string routingModePedestrian(";pedestrian"); // Routing for pedestrians, considers pedestrian maneuverability of links, disregards traffic
	const std::string routingModeBicycle(";bicycle"); // Route calculation for bicycles. This mode uses the bicycle speed on links dedicated for both cars and pedestrians and the pedestrian speed for the roads that are only for pedestrians.
	const std::string routingModeTruck(";truck"); // Route calculation for trucks. This mode considers truck limitations on links and uses different speed assumptions when calculating the route.
	const std::string routingTrafficDisabled(";traffic:disabled");
	const std::string routingFeatureToll(";tollroad:");
	const std::string routingFeatureHighway(";motorway:");
	const std::string routingFeatureBoatFerry(";boatFerry:");
	const std::string routingFeatureRailFerry(";railFerry:");
	const std::string routingFeatureTunnel(";tunnel:");
	const std::string routingFeatureDirtRoad(";dirtRoad:");
	const std::string routingFeaturePark(";park:-2"); // links through parks.This route feature is only applicable for pedestrian routing.
	const std::string routingFeatureWeightNormal("0"); // The routing engine does not alter the ranking of links containing the corresponding feature.
	const std::string routingFeatureWeightAvoid("-1"); // Avoid The routing engine assigns penalties for links containing the corresponding feature.
	const std::string routingFeatureWeightSoftExclude("-2"); // SoftExclude The routing engine does not consider links containing the corresponding feature.If no route can be found because of these limitations the condition is weakened.
	const std::string routingFeatureWeightStrictExclude("-3"); // StrictExclude The routing engine guarantees that the route does not contain strictly excluded features.If the condition cannot be fulfilled no route is returned.
	const std::string routingTruckTrator("&truckType=tractorTruck"); // Truck routing only, relevant for restrictions that apply to tractors with semi-trailers (mostly in North America).
	const std::string routingTruckTailersCount("&trailersCount="); // Truck routing only, specifies number of trailers pulled by a vehicle.The provided value must be between 0 and 4. Defaults to 0.
	const std::string routingTruckShippedHazardousGoods("&shippedHazardousGoods="); // Truck routing only, list of hazardous materials in the vehicle.Please refer to the enumeration type HazardousGoodTypeType for available values.
	const std::string routingTruckLimitedWeight("&limitedWeight="); // Truck routing only, vehicle weight including trailers and shipped goods, in tons.The provided value must be between 0 and 1000.
	const std::string routingTruckWeightPerAxle("&weightPerAxle="); // Truck routing only, vehicle weight per axle in tons.The provided value must be between 0 and 1000.
	const std::string routingTruckHeight("&height="); // Truck routing only, vehicle height in meters.The provided value must be between 0 and 50.
	const std::string routingTruckWidth("&width="); // Truck routing only, vehicle width in meters.The provided value must be between 0 and 50.
	const std::string routingTruckLength("&length="); // Truck routing only, vehicle length in meters.The provided value must be between 0 and 300.
	const std::string routingTruckCategoryExplosive("explosive");
	const std::string routingTruckCategoryGas("gas");
	const std::string routingTruckCategoryFlammable("flammable");
	const std::string routingTruckCategoryCombustible("combustible");
	const std::string routingTruckCategoryOrganic("organic");
	const std::string routingTruckCategoryPoison("poison");
	const std::string routingTruckCategoryRadioActive("radioActive");
	const std::string routingTruckCategoryCorrosive("corrosive");
	const std::string routingTruckCategoryPoisonousInhalation("poisonousInhalation");
	const std::string routingTruckCategoryHarmfulToWater("harmfulToWater");
	const std::string routingTruckCategoryOther("other");
	const std::string routingTruckCategoryAllhazardousGoods("allhazardousGoods");
	const std::string routingWaypointA("&waypoint"); // waypoint = geo + [Type] + Position
	const std::string routingWaypointB("=geo!");	// Latitude, Longitude, [Altitude]
	const std::string routingWaypointPassThrought("passThrough!");	// 180 degree turns are allowed for	stopOver but not for passThrough. PassThrough waypoints will not appear in the list of maneuvers.
}

GeoRouteTravelOptions CHereApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;

	gRouteTravelOptions[GeoVehicleType::Pedestrian] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<CHereApiAcceptedRoadRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Truck] = std::make_unique<CHereApiAcceptedTruckRouteOptions>();

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider(), std::nothrow);

	if (providerApi.getLang().empty() || providerApi.getLang() == std::string("en")) // Bicycle mode is available only for English language
		gRouteTravelOptions[GeoVehicleType::Bike] = std::make_unique<CGeoBasicAcceptedRouteOptions>();

	return gRouteTravelOptions;
}
size_t CHereApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE CHereApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	const CGeoProviderHereApi& providerApi = static_cast<const CGeoProviderHereApi&>(CGeoProviders::instance().get(getProvider()));

	// Build request
	ossUrl << routingUrl << routingAppId << providerApi.getId() << routingAppCode << providerApi.getKey();

	if (!providerApi.getLang().empty())
		ossUrl << routingLanguage << providerApi.getLang();

	ossUrl << routingMode;
	if (cgOptions.getItineraryType() == GeoItineraryType::Shortest)
		ossUrl << routingTypeShortest;
	else
		ossUrl << routingTypeFastest;

	switch (vehicleType)
	{
	case GeoVehicleType::Pedestrian:
		ossUrl << routingModePedestrian;
		break;

	case GeoVehicleType::Bike:
		ossUrl << routingModeBicycle;
		break;

	case GeoVehicleType::Truck:
		ossUrl << routingModeTruck;
		break;

	case GeoVehicleType::Car:
		ossUrl << routingModeCar;
		break;

	default:
		break;
	}

	ossUrl << routingTrafficDisabled;

	if (cgOptions.typeIs(GeoRouteOptionsType::Pedestrian) && !static_cast<const CGeoPedestrianRouteOptions&>(cgOptions).takePark())
		ossUrl << routingFeaturePark << routingFeatureWeightSoftExclude;

	if (cgOptions.typeIs(GeoRouteOptionsType::Road))
	{
		const CGeoRoadRouteOptions& cgRoadOptions = static_cast<const CGeoRoadRouteOptions&>(cgOptions);

		if (!cgRoadOptions.takeHighway())
			ossUrl << routingFeatureHighway << routingFeatureWeightSoftExclude;
		if (!cgRoadOptions.takeTolls())
			ossUrl << routingFeatureToll << routingFeatureWeightSoftExclude;
		if (!cgRoadOptions.takeBoatFerry())
			ossUrl << routingFeatureBoatFerry << routingFeatureWeightSoftExclude;
		if (!cgRoadOptions.takeRailFerry())
			ossUrl << routingFeatureRailFerry << routingFeatureWeightSoftExclude;
		if (!cgRoadOptions.takeTunnel())
			ossUrl << routingFeatureTunnel << routingFeatureWeightSoftExclude;
		if (!cgRoadOptions.takeDirtRoad())
			ossUrl << routingFeatureDirtRoad << routingFeatureWeightSoftExclude;
	}

	if (cgOptions.typeIs(GeoRouteOptionsType::Truck))
	{
		const CGeoTruckRouteOptions& cgTruckOptions = static_cast<const CGeoTruckRouteOptions&>(cgOptions);

		if (cgTruckOptions.isTractor())
			ossUrl << routingTruckTrator;
		if (cgTruckOptions.getTrailersCount() > 0)
			ossUrl << routingTruckTailersCount << cgTruckOptions.getTrailersCount();
		if (cgTruckOptions.getLimitedWeight() > 0)
			ossUrl << routingTruckLimitedWeight << cgTruckOptions.getLimitedWeight() / 10;
		if (cgTruckOptions.getWeightPerAxle() > 0)
			ossUrl << routingTruckWeightPerAxle << cgTruckOptions.getWeightPerAxle() / 10;
		if (cgTruckOptions.getHeight() > 0)
			ossUrl << routingTruckHeight << static_cast<double>(static_cast<int>(cgTruckOptions.getHeight())) / 10;
		if (cgTruckOptions.getWidth() > 0)
			ossUrl << routingTruckWidth << static_cast<double>(static_cast<int>(cgTruckOptions.getWidth())) / 10;
		if (cgTruckOptions.getLength() > 0)
			ossUrl << routingTruckLength << static_cast<double>(static_cast<int>(cgTruckOptions.getLength())) / 10;

		switch (cgTruckOptions.getCategory())
		{
		case GeoTruckCategoryType::hazardous_explosive:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryExplosive;
			break;
		case GeoTruckCategoryType::hazardous_gas:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryGas;
			break;
		case GeoTruckCategoryType::hazardous_flammable:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryFlammable;
			break;
		case GeoTruckCategoryType::hazardous_combustible:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryCombustible;
			break;
		case GeoTruckCategoryType::hazardous_organic:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryOrganic;
			break;
		case GeoTruckCategoryType::hazardous_poison:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryPoison;
			break;
		case GeoTruckCategoryType::hazardous_radioActive:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryRadioActive;
			break;
		case GeoTruckCategoryType::hazardous_corrosive:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryCorrosive;
			break;
		case GeoTruckCategoryType::hazardous_poisonousInhalation:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryPoisonousInhalation;
			break;
		case GeoTruckCategoryType::hazardous_harmfulToWater:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryHarmfulToWater;
			break;
		case GeoTruckCategoryType::hazardous_other:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryOther;
			break;
		case GeoTruckCategoryType::hazardous_all:
			ossUrl << routingTruckShippedHazardousGoods << routingTruckCategoryAllhazardousGoods;
			break;
		default:
			break;
		}
	}

	CGeoLatLngs::const_iterator itBegin = cgLatLngs.begin();
	for (CGeoLatLngs::const_iterator it = itBegin; it != cgLatLngs.end(); ++it)
		ossUrl << routingWaypointA << std::distance(itBegin, it) << routingWaypointB << it->lat() << ',' << it->lng();

	request.strUrl = ossUrl.str();
	request.strReferrer = providerApi.getReferer();

	return E_GEO_OK;
}

E_GEO_STATUS_CODE CHereApiDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		vecRoutes.push_back(CGeoRoute(vehicleType, cgOptions));
		CGeoRoute& gRoute = vecRoutes.back();

		// Read Waypoints
		const CJsonObject& jsRoute = jsParser("response")("route")[0];
		const CJsonArray& jsWaypoints = jsRoute("waypoint");
		for (size_t i = 0; i < jsWaypoints.size(); i++)
		{
			const CJsonObject& jsWaypoint = jsWaypoints[i];
			CGeoLatLng gLatLng(
				jsWaypoint("mappedPosition")("latitude"),
				jsWaypoint("mappedPosition")("longitude"));

			gRoute.locations().push_back(CGeoLocation(gLatLng, jsWaypoint("label")));
		}

		// Read Shape
		CGeoLatLngs& polylinePath = gRoute.polyline().getPath();
		const CJsonArray& jsShape = jsRoute("shape");
		for (size_t i = 0; i < jsShape.size(); i++)
		{
			stdx::string_helper::vector spltLatLng;
			stdx::string_helper::split(jsShape[i], spltLatLng, stdx::find_first(","));
			if (spltLatLng.size() >= 2)
			{
				CGeoLatLng gLatLng(
					stdx::string_helper::string_to<double>(spltLatLng[0]),
					stdx::string_helper::string_to<double>(spltLatLng[1]));

				polylinePath.push_back(gLatLng);
			}
		}

		// Read Summary
		const CJsonObject& jsSummary = jsRoute("summary");
		gRoute.summary().assign(jsSummary("distance"), jsSummary("travelTime"));
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return E_GEO_OK;
}
