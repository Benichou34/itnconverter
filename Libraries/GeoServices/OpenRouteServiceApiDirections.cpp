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
 * http://wiki.openstreetmap.org/wiki/OpenRouteService
 */

#include <sstream>
#include "OpenRouteServiceApiDirections.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

namespace
{
	class COpenRouteServiceApiAcceptedRouteOptions : public CGeoBasicAcceptedRoadRouteOptions
	{
	public:
		~COpenRouteServiceApiAcceptedRouteOptions() final = default;

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

		GeoParameter<bool> tolls() const final { return { true, true }; }
		GeoParameter<bool> highway() const final { return { true, true }; }
	};

	constexpr size_t maxRequestStep = 25;

	const std::string methodQuickest("Fastest");
	const std::string methodShortest("Shortest");
	const std::string methodWalking("Pedestrian");
	const std::string methodBicycling("Bicycle");

	const std::string directionUrl("https://openrouteservice.org/php/OpenLSRS_DetermineRoute.php");
	const std::string directionStart("Start=");
	const std::string directionEnd("&End=");
	const std::string directionVia("&Via=");
	const std::string directionPreference("&routepref=");
	const std::string directionLanguage("&lang=");
	const std::string avoidHighway("&noMotorways=true");
	const std::string avoidTolls("&noTollways=true");
}

GeoRouteTravelOptions COpenRouteServiceApiDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;

	gRouteTravelOptions[GeoVehicleType::Bike] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Pedestrian] = std::make_unique<CGeoBasicAcceptedRouteOptions>();
	gRouteTravelOptions[GeoVehicleType::Car] = std::make_unique<COpenRouteServiceApiAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

size_t COpenRouteServiceApiDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE COpenRouteServiceApiDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	// Build request
	ossUrl << directionStart << cgLatLngs.front().lng() << ',' << cgLatLngs.front().lat();
	ossUrl << directionEnd << cgLatLngs.back().lng() << ',' << cgLatLngs.back().lat();

	if (cgLatLngs.size() > 2)
	{
		ossUrl << directionVia;

		CGeoLatLngs::const_iterator it = cgLatLngs.begin();
		CGeoLatLngs::const_iterator itEnd = cgLatLngs.end();

		for (++it, --itEnd; it != itEnd; ++it)
			ossUrl << it->lng() << ',' << it->lat() << ' ';
	}

	ossUrl << directionPreference;
	switch (vehicleType)
	{
	case GeoVehicleType::Pedestrian:
		ossUrl << methodWalking;
		break;

	case GeoVehicleType::Bike:
		ossUrl << methodBicycling;
		break;

	case GeoVehicleType::Car:
		if (cgOptions.getItineraryType() == GeoItineraryType::Shortest)
			ossUrl << methodShortest;
		else
			ossUrl << methodQuickest;
		break;

	default:
		ossUrl << methodQuickest;
		break;
	}

	if (cgOptions.typeIs(GeoRouteOptionsType::Road))
	{
		if (!static_cast<const CGeoRoadRouteOptions&>(cgOptions).takeHighway())
			ossUrl << avoidHighway;
		if (!static_cast<const CGeoRoadRouteOptions&>(cgOptions).takeTolls())
			ossUrl << avoidTolls;
	}

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider(), std::nothrow);

	if (providerApi.getLang().size())
		ossUrl << directionLanguage << providerApi.getLang();

	request.strUrl = directionUrl;
	request.strPostData = ossUrl.str();
	request.strReferrer = providerApi.getReferer();

	return E_GEO_OK;
}

E_GEO_STATUS_CODE COpenRouteServiceApiDirections::parseRequest(const std::string&, GeoVehicleType::type_t, const CGeoRouteOptions&, GeoRoutes&)
{
	return E_GEO_INVALID_REQUEST;
}
