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
#include "HereApi.h"
#include "HereApiLocalSearch.h"
#include "jsonParser/JsonParser.h"
#include "stdx/string_helper.h"

using namespace geo;

namespace
{
	const std::string searchRequest("http://places.api.here.com/places/v1/discover/search?refinements=false&Accept=application/json&q=");
	const std::string searchAppId("&app_id=");
	const std::string searchAppCode("&app_code=");
	const std::string searchLanguage("&language=");
	const std::string searchLocation("&at=");
}

E_GEO_STATUS_CODE CHereApiLocalSearch::getRequestUrl(const std::string& strText, const CGeoLatLng& gLocation, size_t, Request& request)
{
	std::ostringstream ossUrl;

	const CGeoProviderHereApi& providerApi = static_cast<const CGeoProviderHereApi&>(CGeoProviders::instance().get(getProvider()));

	// Build request
	ossUrl << searchRequest << strText << searchAppId << providerApi.getId() << searchAppCode << providerApi.getKey();

	if (!providerApi.getLang().empty())
		ossUrl << searchLanguage << providerApi.getLang();

	ossUrl << searchLocation << gLocation.lat() << ',' << gLocation.lng();

	request.strUrl = ossUrl.str();
	request.strReferrer = providerApi.getReferer();

	return E_GEO_OK;
}

E_GEO_STATUS_CODE CHereApiLocalSearch::parseRequest(const std::string& strRequest, CGeoLocations& gLocations)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		const CJsonArray& jsArray = jsParser("results")("items");
		for (size_t i = 0; i < jsArray.size(); ++i)
		{
			const CJsonObject& jsLocation = jsArray[i];

			CGeoLocation geoLocation;
			geoLocation.lat(jsLocation("position")[0]);
			geoLocation.lng(jsLocation("position")[1]);

			geoLocation.name(jsLocation("title"));
			geoLocation.comment(jsLocation("vicinity"));

			gLocations.push_back(geoLocation);
		}
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return gLocations.empty() ? E_GEO_ZERO_RESULTS : E_GEO_OK;
}
