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
#include "HereApiGeocoder.h"
#include "HereApi.h"
#include "jsonParser/JsonParser.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"

using namespace geo;

namespace
{
	const std::string geocoderUrl("http://geocoder.api.here.com/6.2/geocode.json?gen=4&searchtext=");
	const std::string geocoderAppId("&app_id=");
	const std::string geocoderAppCode("&app_code=");
	const std::string geocoderLanguage("&language=");
}

CHereApiGeocoder::CHereApiGeocoder() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

CHereApiGeocoder::CHereApiGeocoder(const std::string& strAddress)
{
	Load(strAddress);
}

E_GEO_STATUS_CODE CHereApiGeocoder::Load(const std::string& strAddress)
{
	std::ostringstream ossUrl;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_GeoResults.clear();

	const CGeoProviderHereApi& providerApi = static_cast<const CGeoProviderHereApi&>(CGeoProviders::instance().get(getProvider()));

	// Build request
	ossUrl << geocoderUrl << strAddress << geocoderAppId << providerApi.getId() << geocoderAppCode << providerApi.getKey();

	if (!providerApi.getLang().empty())
		ossUrl << geocoderLanguage << providerApi.getLang();

	try
	{
		std::ostringstream oss;
		CInternetHttpSession httpSession;
		httpSession.send(oss, ossUrl.str());
		httpSession.wait();

		CJsonParser jsParser;
		if (jsParser.parse(oss.str()) == CJsonParser::JSON_SUCCESS)
		{
			const CJsonArray& jsArray = jsParser("Response")("View")[0]("Result");
			for (size_t i = 0; i < jsArray.size(); ++i)
			{
				const CJsonObject& jsLocation = jsArray[i]("Location");
				CGeoLocation geoLocation;
				geoLocation.lat(jsLocation("DisplayPosition")("Latitude"));
				geoLocation.lng(jsLocation("DisplayPosition")("Longitude"));

				geoLocation.name(jsLocation("Address")("Label"));
				//				geoLocation.comment(jsLocation("Address")("Label"));

				m_GeoResults.push_back(geoLocation);
			}

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
