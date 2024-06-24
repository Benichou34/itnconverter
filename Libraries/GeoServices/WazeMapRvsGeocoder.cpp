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
#include "WazeMapRvsGeocoder.h"
#include "jsonParser/JsonParser.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "stdx/string_helper.h"

using namespace geo;

namespace
{
	const std::string geocoderUrl("/app/getStreet?radius=0.0005");
	const std::string geocoderLatitude("&lat=");
	const std::string geocoderLongitude("&lon=");
}

CWazeMapRvsGeocoder::CWazeMapRvsGeocoder(const std::string& strBaseUrl) :
	m_strBaseUrl(strBaseUrl),
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

E_GEO_STATUS_CODE CWazeMapRvsGeocoder::Load(const CGeoLatLng& gLatLng)
{
	std::ostringstream ossUrl;
	CJsonParser jsParser;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_GeoResults.clear();

	// Build request
	ossUrl << m_strBaseUrl << geocoderUrl << geocoderLatitude << gLatLng.lat() << geocoderLongitude << gLatLng.lng();

	try
	{
		if (jsParser.parse(CInternet::AjaxHttpRequest(ossUrl.str())) == CJsonParser::JSON_SUCCESS)
		{
			const CJsonArray& jsResults = jsParser;
			if (!jsResults.empty())
			{
				const CJsonObject& jsResult = jsResults[0];
				CGeoLocation geoLocation(gLatLng);

				const CJsonValue& jsStreet = jsResult("street");
				const CJsonValue& jsCity = jsResult("city");
				const CJsonValue& jsCountry = jsResult("country");

				std::string strName;
				if (jsStreet.getType() == CJsonValue::JSON_TYPE_STRING)
					strName = jsStreet;

				if (jsCity.getType() == CJsonValue::JSON_TYPE_STRING)
				{
					if (!strName.empty())
						strName += ", ";
					strName += jsCity;
				}

				if (jsCountry.getType() == CJsonValue::JSON_TYPE_STRING)
				{
					if (!strName.empty())
						strName += ", ";
					strName += jsCountry;
				}

				geoLocation.name(strName);
				if (geoLocation && !geoLocation.name().empty())
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
