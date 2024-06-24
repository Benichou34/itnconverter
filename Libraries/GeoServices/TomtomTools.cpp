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
#include "TomtomTools.h"
#include "GeoLocation.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

E_GEO_STATUS_CODE CTomtomTools::GetStatusCode(int iStatus)
{
	switch (iStatus)
	{
	case 200: // OK. The request is successful.
		return E_GEO_OK;

	case 400: // Bad request, usually due to malformed syntax.
		return E_GEO_INVALID_REQUEST;

	case 401: // Unauthorized.
	case 403: // Forbidden: Supplied API key is not valid for this request.
		return E_GEO_REQUEST_DENIED;

	case 404: // Not Found : the requested resource could not be found, but it may be available again in the future.
	case 405: // Method Not Allowed : the client used a HTTP method other than GET or POST.
	case 408: // Request timeout.
	case 414: // Requested uri is too long.
	case 500: // An error occured while processing the request.Please try again later.
	case 502: // Internal network connectivity issue.
	case 503: // Service currently unavailable.
	case 504: // Internal network connectivity issue or a request that has taken too long to complete.
	case 596: // Service not found.
	default:
		break;
	}

	return E_GEO_UNKNOWN_ERROR;
}

void CTomtomTools::fromJsonToLatLng(const CJsonObject& jsObject, CGeoLatLng& geoLatLng)
{
	// Read coordinates
	geoLatLng.coords(jsObject("latitude"), jsObject("longitude"));
}

void CTomtomTools::fromJsonToLocation(const CJsonObject& jsObject, CGeoLocation& geoLocation)
{
	// Read address
	geoLocation.name(jsObject("formattedAddress"));
	if (jsObject.exist("phone"))
		geoLocation.comment(jsObject("phone"));
	else
		geoLocation.comment(std::string());

	fromJsonToLatLng(jsObject, geoLocation);
}

std::string CTomtomTools::fromLatLngToUrlValue(const CGeoLatLng& geoLatLng)
{
	std::ostringstream oss;
	oss << geoLatLng.lat() << ',' << geoLatLng.lng();
	return oss.str();
}
