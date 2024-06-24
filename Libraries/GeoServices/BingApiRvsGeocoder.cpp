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
#include "BingApiRvsGeocoder.h"
#include "BingTools.h"
#include "jsonParser/JsonParser.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"

using namespace geo;

#define GEOCODER_URL		"http://dev.virtualearth.net/REST/v1/Locations/"
#define GEOCODER_OUTPUT		"?o=json"
#define GEOCODER_KEY		"&key="
#define GEOCODER_LANGUAGE	"&c="

CBingApiRvsGeocoder::CBingApiRvsGeocoder() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

CBingApiRvsGeocoder::CBingApiRvsGeocoder(const CGeoLatLng& gLatLng)
{
	Load(gLatLng);
}

E_GEO_STATUS_CODE CBingApiRvsGeocoder::Load(const CGeoLatLng& gLatLng)
{
	std::ostringstream ossUrl;
	CJsonParser jsParser;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_GeoResults.clear();

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << GEOCODER_URL << CBingTools::fromLatLngToUrlValue(gLatLng) << GEOCODER_OUTPUT << GEOCODER_KEY << providerApi.getKey();

	if (!providerApi.getLang().empty())
		ossUrl << GEOCODER_LANGUAGE << providerApi.getLang();

	try
	{
		if (jsParser.parse(CInternet::AjaxHttpRequest(ossUrl.str(), providerApi.getReferer())) == CJsonParser::JSON_SUCCESS)
		{
			// Read status
			m_eStatus = CBingTools::GetStatusCode(jsParser("statusCode"));
			if (m_eStatus != E_GEO_OK)
				return m_eStatus;

			// Read Placemarks
			CBingTools::fromJsonToLocations(jsParser("resourceSets")[0]("resources"), m_GeoResults);

			m_eStatus = m_GeoResults.empty() ? E_GEO_ZERO_RESULTS : E_GEO_OK;
		}
	}
	catch (CJsonException&)
	{
		m_eStatus = E_GEO_INVALID_REQUEST;
	}
	catch (CInternetException&)
	{
		m_eStatus = E_HTTP_ERROR;
	}

	return m_eStatus;
}
