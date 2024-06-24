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
#include "GoogleTools.h"
#include "GeoLocations.h"
#include "GeoSummary.h"
#include "jsonParser/JsonParser.h"
#include "stdx/string_helper.h"
#include "stdx/uri_helper.h"

using namespace geo;

E_GEO_STATUS_CODE CGoogleTools::GetStatusCode(const std::string& strStatus)
{
	if (!strStatus.compare("OK"))
		return E_GEO_OK;

	if (!strStatus.compare("ZERO_RESULTS"))
		return E_GEO_ZERO_RESULTS;

	if (!strStatus.compare("OVER_QUERY_LIMIT"))
		return E_GEO_OVER_QUERY_LIMIT;

	if (!strStatus.compare("REQUEST_DENIED"))
		return E_GEO_REQUEST_DENIED;

	if (!strStatus.compare("INVALID_REQUEST"))
		return E_GEO_INVALID_REQUEST;

	if (!strStatus.compare("NOT_FOUND"))
		return E_GEO_NOT_FOUND;

	if (!strStatus.compare("MAX_WAYPOINTS_EXCEEDED"))
		return E_GEO_MAX_WAYPOINTS_EXCEEDED;

	return E_GEO_UNKNOWN_ERROR;
}

void CGoogleTools::fromJsonToLatLng(const CJsonObject& jsObject, CGeoLatLng& geoLatLng)
{
	// Read coordinates
	geoLatLng.coords(jsObject("lat"), jsObject("lng"));
}

void CGoogleTools::fromJsonToLocation(const CJsonObject& jsObject, CGeoLocation& geoLocation)
{
	// Read address
	if (jsObject.exist("name"))
	{
		geoLocation.name(jsObject("name"));
		geoLocation.comment(jsObject("formatted_address"));
	}
	else
	{
		geoLocation.name(jsObject("formatted_address"));
		geoLocation.comment(std::string());
	}

	fromJsonToLatLng(jsObject("geometry")("location"), geoLocation);
}

void CGoogleTools::fromJsonToLocations(const CJsonArray& jsArray, CGeoLocations& geoLocations)
{
	for (size_t i = 0; i < jsArray.size(); i++)
	{
		CGeoLocation gLocation;
		fromJsonToLocation(jsArray[i], gLocation);

		if (gLocation)
			geoLocations.push_back(gLocation);
	}
}

void CGoogleTools::fromJsonToSummary(const CJsonObject& jsObject, CGeoSummary& geoSummary)
{
	geoSummary.assign(jsObject("distance")("value"), jsObject("duration")("value"));
}

std::string CGoogleTools::fromLatLngToUrlValue(const CGeoLatLng& geoLatLng)
{
	std::ostringstream oss;
	oss << geoLatLng.lat() << ',' << geoLatLng.lng();
	return oss.str();
}

void CGoogleTools::fromUrlValueToLatLng(const std::string& strUrlValue, CGeoLatLng& geoLatLng)
{
	std::istringstream iss(strUrlValue);

	double dLat, dLon;

	iss >> dLat;
	iss.ignore(1);
	iss >> dLon;

	geoLatLng.coords(dLat, dLon);
}

void CGoogleTools::fromUrlNameToLocationName(const std::string& strUrlName, CGeoLocation& geoLocation)
{
	std::string strName(strUrlName);
	stdx::string_helper::replace(strName, '+', ' ');
	geoLocation.name(stdx::uri_helper::unescape(strName));
}

void CGoogleTools::fromUrlValueToLocation(const std::string& strUrlValue, CGeoLocation& geoLocation)
{
	size_t pos = strUrlValue.find('@');
	std::string strName;

	if (pos == std::string::npos)
	{
		strName = strUrlValue;
		pos = 0;
	}
	else
	{
		strName = strUrlValue.substr(0, pos);
		pos++;
	}

	fromUrlNameToLocationName(strName, geoLocation);
	fromUrlValueToLatLng(strUrlValue.substr(pos), geoLocation);
}
