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
#include "GoogleApiLocalSearch.h"
#include "jsonParser/JsonParser.h"
#include "stdx/string_helper.h"

using namespace geo;

namespace
{
	constexpr size_t defaultRadius = 30000; // in meters

	const std::string searchRequest("https://maps.googleapis.com/maps/api/place/textsearch/json?query=");
	const std::string searchKey("&key=");
	const std::string searchLanguage("&language=");
	const std::string searchLocation("&location=");
	const std::string searchRadius("&radius=");
}

E_GEO_STATUS_CODE CGoogleApiLocalSearch::getRequestUrl(const std::string& strText, const CGeoLatLng& gLocation, size_t radius, Request& request)
{
	std::ostringstream ossUrl;

	if (!radius)
		radius = defaultRadius;

	std::string strUrlText(strText);
	stdx::string_helper::replace(strUrlText, ' ', '+');

	try
	{
		const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

		// Build request
		ossUrl << searchRequest << strUrlText;

		if (gLocation)
			ossUrl << searchLocation << CGoogleTools::fromLatLngToUrlValue(gLocation) << searchRadius << radius;

		ossUrl << searchKey << providerApi.getKey();
		if (!providerApi.getLang().empty())
			ossUrl << searchLanguage << providerApi.getLang();

		request.strReferrer = providerApi.getReferer();
		request.strUrl = ossUrl.str();
	}
	catch (...)
	{
		return E_GEO_REQUEST_DENIED;
	}

	return E_GEO_OK;
}

E_GEO_STATUS_CODE CGoogleApiLocalSearch::parseRequest(const std::string& strRequest, CGeoLocations& gLocations)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		// Read status
		E_GEO_STATUS_CODE status = CGoogleTools::GetStatusCode(jsParser("status"));
		if (status != E_GEO_OK)
			return status;

		// Read Placemarks
		CGoogleTools::fromJsonToLocations(jsParser("results"), gLocations);
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return E_GEO_OK;
}
