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
#include "ViaMichelinUrl.h"
#include "ViaMichelinApiRvsGeocoder.h"
#include "ViaMichelinApiLocation.h"
#include "stdx/string_helper.h"
#include "stdx/format.h"
#include "stdx/uri_helper.h"
#include "stdx/guard.h"

using namespace geo;

namespace
{
	const std::string c_strMapsDir1("/web/Itineraires");
	const std::string c_strMapsDir2("/web/Routes");
	const std::string c_strDeparture("departure");
	const std::string c_strDepartureId("departureId");
	const std::string c_strArrival("arrival");
	const std::string c_strArrivalId("arrivalId");
	const std::string c_strStep("step-%d");
	const std::string c_strStepId("step-%dId");
	const std::string c_strStartLocid("strStartLocid");
	const std::string c_strDestLocid("strDestLocid");
	const std::string c_strVehicle("vehicle");
	const std::string c_strTravelType("type");
	const std::string c_strMotorway("motorway");
	const std::string c_strToll("toll");
	const std::string c_strDistance("distance=km");
	const std::string c_strPrint("printRoadSheet=true");

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

bool CViaMichelinUrl::isValidUrl(const std::string& strUrl)
{
	return (strUrl.find("www.viamichelin.") != std::string::npos);
}

void CViaMichelinUrl::UrlToLocations(const std::string& strUrl, CGeoLocations& glocations)
{
	stdx::string_helper::map spltRequest;
	stdx::string_helper::split(stdx::uri_helper::unescape(strUrl), spltRequest, stdx::find_first_of("?&"), stdx::find_first("="));
	if (spltRequest.empty())
		return;

	if ((spltRequest.count(c_strDepartureId) && spltRequest.count(c_strArrivalId)) || (spltRequest.count(c_strStartLocid) && spltRequest.count(c_strDestLocid)))
	{
		auto decodeLocation = [&](const std::string& strId, const std::string& strName = std::string())
		{
			CViaMichelinApiLocation gGeocoder(strId);
			if (gGeocoder.getStatus() == E_GEO_OK)
			{
				const geo::CGeoLocations& geoResults = gGeocoder.getResults();
				if (!geoResults.empty())
				{
					CGeoLocation geoLocation = geoResults.front();
					if (!strName.empty())
						geoLocation.name(strName);

					glocations.push_back(geoLocation);
				}
			}
		};

		if (spltRequest.count(c_strDepartureId))
			decodeLocation(spltRequest[c_strDepartureId], spltRequest[c_strDeparture]);
		else
			decodeLocation(spltRequest[c_strStartLocid]);

		for (size_t counter = 1;; ++counter)
		{
			std::string strStepId = stdx::format(c_strStepId)(counter);
			if (spltRequest.count(strStepId) == 0)
				break;

			decodeLocation(spltRequest[strStepId], spltRequest[stdx::format(c_strStep)(counter)]);
		}

		if (spltRequest.count(c_strArrivalId))
			decodeLocation(spltRequest[c_strArrivalId], spltRequest[c_strArrival]);
		else
			decodeLocation(spltRequest[c_strDestLocid]);
	}
}

void CViaMichelinUrl::LocationsToUrl(const CGeoLocations& glocations, std::string& strUrl, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, bool bPrint)
{
	std::ostringstream ossUrl;

	// Build request
	ossUrl << "https://www.viamichelin.com/web/Routes" << '?' << c_strDistance;

	// Add steps
	for (size_t i = 0; i < glocations.size(); ++i)
	{
		const CGeoLocation& glocation = glocations[i];

		if (i == 0)
			ossUrl << '&' << c_strDeparture << '=';
		else if (i == glocations.upper_bound())
			ossUrl << '&' << c_strArrival << '=';
		else
			ossUrl << '&' << stdx::format(c_strStep)(i) << '=';
		ossUrl << glocation.name();

		CViaMichelinApiRvsGeocoder gGeocoder(glocation);
		if (gGeocoder.getStatus() == E_GEO_OK)
		{
			const std::vector<std::string>& geoIds = gGeocoder.getIds();
			if (!geoIds.empty())
			{
				if (i == 0)
					ossUrl << '&' << c_strDepartureId << '=';
				else if (i == glocations.upper_bound())
					ossUrl << '&' << c_strArrivalId << '=';
				else
					ossUrl << '&' << stdx::format(c_strStepId)(i) << '=';
				ossUrl << geoIds.front();
			}
		}
	}

	ossUrl << '&' << c_strVehicle << '=';
	switch (vehicleType)
	{
	case GeoVehicleType::Car:
		ossUrl << vehicle_car;
		break;

	case GeoVehicleType::Truck:
		ossUrl << vehicle_truck;
		break;

	case GeoVehicleType::Pedestrian:
		ossUrl << vehicle_pedestrian;
		break;

	case GeoVehicleType::Bike:
		ossUrl << vehicle_bike;
		break;

	case GeoVehicleType::Motorbike:
		ossUrl << vehicle_motorbike;
		break;

	default:
		ossUrl << vehicle_car;
		break;
	}

	ossUrl << '&' << c_strTravelType << '=';
	switch (cgOptions.getItineraryType())
	{
	case GeoItineraryType::Recommended:
		ossUrl << iti_recommended;
		break;

	case GeoItineraryType::Quickest:
		ossUrl << iti_quickest;
		break;

	case GeoItineraryType::Shortest:
		ossUrl << iti_shortest;
		break;

	case GeoItineraryType::Sightseeing:
		ossUrl << iti_sightseeing;
		break;

	case GeoItineraryType::Economical:
		ossUrl << iti_economical;
		break;

	default:
		ossUrl << iti_recommended;
		break;
	}

	if (cgOptions.typeIs(GeoRouteOptionsType::Road))
	{
		ossUrl << '&' << c_strMotorway << '=' << std::boolalpha << static_cast<const CGeoRoadRouteOptions&>(cgOptions).takeHighway();
		ossUrl << '&' << c_strToll << '=' << std::boolalpha << static_cast<const CGeoRoadRouteOptions&>(cgOptions).takeTolls();
	}

	// Print
	if (bPrint)
		ossUrl << '&' << c_strPrint;

	strUrl = ossUrl.str();
}
