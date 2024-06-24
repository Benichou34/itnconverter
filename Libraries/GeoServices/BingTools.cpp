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
#include "BingTools.h"
#include "GeoLocations.h"
#include "GeoSummary.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

E_GEO_STATUS_CODE CBingTools::GetStatusCode(int iStatus)
{
	switch (iStatus)
	{
	case 200: // OK. The request is successful.
	case 201: // Created. A new resource is created.
		return E_GEO_OK;

	case 400: // Bad Request. The request contained an error.
		return E_GEO_INVALID_REQUEST;

	case 401: // Unauthorized. Access was denied. You may have entered your credentials incorrectly, or you might not have access to the requested resource or operation.
		return E_GEO_REQUEST_DENIED;

	case 404: // Not Found. The requested resource was not found.
		return E_GEO_NOT_FOUND;

	case 500: // Internal Server Error. Your request could not be completed because there was a problem with the service.
	case 503: // Service Unavailable. There's a problem with the service right now. Please try again later.
	default:
		break;
	}

	return E_GEO_UNKNOWN_ERROR;
}

void CBingTools::fromJsonToLatLng(const CJsonArray& jsCoordinates, CGeoLatLng& geoLatLng)
{
	// Read coordinates
	geoLatLng.coords(jsCoordinates[0], jsCoordinates[1]);
}

void CBingTools::fromJsonToLocation(const CJsonObject& jsObject, CGeoLocation& geoLocation)
{
	// Read address
	geoLocation.name(jsObject("name"));
	geoLocation.comment(std::string());

	fromJsonToLatLng(jsObject("point")("coordinates"), geoLocation);
}

void CBingTools::fromJsonToLocations(const CJsonArray& jsArray, CGeoLocations& geoLocations)
{
	for (size_t i = 0; i < jsArray.size(); i++)
	{
		CGeoLocation gLocation;
		fromJsonToLocation(jsArray[i], gLocation);

		if (gLocation)
			geoLocations.push_back(gLocation);
	}
}

void CBingTools::fromJsonToSummary(const CJsonObject& jsObject, CGeoSummary& geoSummary)
{
	double dDistance = static_cast<double>(jsObject("travelDistance")) * 1000;
	geoSummary.assign(static_cast<size_t>(dDistance), jsObject("travelDuration"));
}

std::string CBingTools::fromLatLngToUrlValue(const CGeoLatLng& geoLatLng)
{
	std::ostringstream oss;
	oss << geoLatLng.lat() << ',' << geoLatLng.lng();
	return oss.str();
}
