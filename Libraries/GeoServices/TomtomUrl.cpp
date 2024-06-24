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
#include "TomtomUrl.h"
#include "TomtomApiRvsGeocoder.h"
#include "GeoLocations.h"
#include "jsonParser\JsonParser.h"
#include "stdx/string_helper.h"
#include "stdx/format.h"
#include "stdx/uri_helper.h"
#include "stdx/guard.h"

using namespace geo;

namespace
{
	const std::string c_strRoutes("+routes=");
	const std::string c_strViewport("+viewport=");
	const std::string c_strRouteType("routeType");
	const std::string c_strTravelMode("travelMode");
	const std::string c_strPointPrefix("hw~");
}

bool CTomtomUrl::isValidUrl(const std::string& strUrl)
{
	return (strUrl.find("mydrive.tomtom.com") != std::string::npos);
}

void CTomtomUrl::UrlToLocations(const std::string& strUrl, CGeoLocations& glocations)
{
	std::string strRequest = stdx::uri_helper::unescape(strUrl);

	size_t pos = strRequest.find(c_strRoutes);
	if (pos == std::string::npos)
		return;

	try
	{
		CJsonParser jsParser;
		if (jsParser.parse(strRequest.substr(pos + c_strRoutes.size())) == CJsonParser::JSON_SUCCESS)
		{
			const CJsonArray& jsPoints = jsParser("points");
			for (size_t i = 0; i < jsPoints.size(); i++)
			{
				stdx::string_helper::vector vecStrResult;
				stdx::string_helper::split(jsPoints[i], vecStrResult, stdx::find_first_of("~,"));
				if (vecStrResult.size() > 2)
				{
					CGeoLocation geoLocation;
					geoLocation.lat(stdx::string_helper::string_to<double>(vecStrResult[1]));
					geoLocation.lng(stdx::string_helper::string_to<double>(vecStrResult[2]));

					CTomtomApiRvsGeocoder gGeocoder(geoLocation);
					if (gGeocoder.getStatus() == E_GEO_OK)
					{
						const geo::CGeoLocations& geoResults = gGeocoder.getResults();
						if (!geoResults.empty())
							geoLocation = geoResults.front();
					}

					if (geoLocation)
						glocations.push_back(geoLocation);
				}
			}
		}
	}
	catch (CJsonException&)
	{
	}
}

void CTomtomUrl::LocationsToUrl(const CGeoLocations& glocations, std::string& strUrl, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, bool)
{
	if (glocations.empty())
		return;

	CJsonParser jsParser;
	CJsonObject& jsParamRoot = jsParser.setType(CJsonParser::JSON_TYPE_OBJECT);

	CJsonArray& jsPoints = jsParamRoot.add("points").setType(CJsonValue::JSON_TYPE_ARRAY);
	for (const auto& gLocation : glocations)
		jsPoints.add() = stdx::format("%s%f,%f")(c_strPointPrefix)(gLocation.lat())(gLocation.lng());

	switch (vehicleType)
	{
	case GeoVehicleType::Truck:
		jsParamRoot.add(c_strTravelMode) = std::string("TRUCK");
		break;

	default:
		jsParamRoot.add(c_strTravelMode) = std::string("CAR");
		break;
	}

	switch (cgOptions.getItineraryType())
	{
	case GeoItineraryType::Thrilling:
		jsParamRoot.add(c_strRouteType) = std::string("THRILLING");
		if (cgOptions.typeIs(GeoRouteOptionsType::Thrilling))
		{
			const CGeoThrillingRouteOptions& cgThrillingOptions = static_cast<const CGeoThrillingRouteOptions&>(cgOptions);

			switch (cgThrillingOptions.getHilliness())
			{
			case CGeoAcceptedThrillingRouteOptions::degree_low:
				jsParamRoot.add("hilliness") = std::string("LOW");
				break;

			case CGeoAcceptedThrillingRouteOptions::degree_normal:
				jsParamRoot.add("hilliness") = std::string("NORMAL");
				break;

			case CGeoAcceptedThrillingRouteOptions::degree_high:
				jsParamRoot.add("hilliness") = std::string("LOW");
				break;

			default:
				break;
			}

			switch (cgThrillingOptions.getWindingness())
			{
			case CGeoAcceptedThrillingRouteOptions::level_low:
				jsParamRoot.add("windingness") = std::string("LOW");
				break;

			case CGeoAcceptedThrillingRouteOptions::level_normal:
				jsParamRoot.add("windingness") = std::string("NORMAL");
				break;

			case CGeoAcceptedThrillingRouteOptions::level_high:
				jsParamRoot.add("windingness") = std::string("HIGH");
				break;

			default:
				break;
			}
		}
		break;

	default:
		jsParamRoot.add(c_strRouteType) = std::string("FASTEST");
		break;
	}

	const CGeoLocation& gStartLocation = glocations.front();

	// Build request
	std::ostringstream ossUrl;
	ossUrl << "https://mydrive.tomtom.com/en_gb/#mode=routes" << c_strViewport << stdx::format("%f,%f,10")(gStartLocation.lat())(gStartLocation.lng()) << c_strRoutes << jsParser;
	strUrl = stdx::uri_helper::encode(ossUrl.str());
}
