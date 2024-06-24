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
#include "GeoNamesCountryCode.h"
#include "GeoNamesTools.h"
#include "GeoLatLng.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "jsonParser/JsonParser.h"

#define GEONAME_URL			"http://api.geonames.org"
#define GEONAME_REQUEST		"/countryCode?type=JSON"
#define GEONAME_USERNAME	"&username="
#define GEONAME_LANGUAGE	"&lang="
#define GEONAME_LAT			"&lat="
#define GEONAME_LNG			"&lng="

using namespace geo;

CGeoNamesCountryCode::CGeoNamesCountryCode() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

CGeoNamesCountryCode::CGeoNamesCountryCode(const CGeoLatLng& gLatLng)
{
	Load(gLatLng);
}

E_GEO_STATUS_CODE CGeoNamesCountryCode::Load(const CGeoLatLng& gLatLng)
{
	std::ostringstream ossUrl;
	CJsonParser jsParser;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_strCountryName.clear();
	m_strCountryCode.clear();

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << GEONAME_URL << GEONAME_REQUEST << GEONAME_USERNAME << providerApi.getKey();
	if (!providerApi.getLang().empty())
		ossUrl << GEONAME_LANGUAGE << providerApi.getLang();

	ossUrl << GEONAME_LAT << gLatLng.lat() << GEONAME_LNG << gLatLng.lng();

	try
	{
		if (jsParser.parse(CInternet::AjaxHttpRequest(ossUrl.str())) == CJsonParser::JSON_SUCCESS)
		{
			if (jsParser.exist("status"))
			{
				m_eStatus = CGeoNamesTools::GetStatusCode(jsParser("status")("value"));
				return m_eStatus;
			}

			m_strCountryName = static_cast<std::string>(jsParser("countryName"));
			m_strCountryCode = static_cast<std::string>(jsParser("countryCode"));

			// Set status
			m_eStatus = E_GEO_OK;
		}
	}
	catch(CJsonException&)
	{
		m_eStatus = E_GEO_INVALID_REQUEST;
	}
	catch (CInternetException&)
	{
		m_eStatus = E_HTTP_ERROR;
	}

	return m_eStatus;
}
