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
#include "WazeMapGeocoder.h"
#include "jsonParser/JsonParser.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "stdx/string_helper.h"

using namespace geo;

#define GEOCODER_URL		"/mozi?lon=0&lat=0&q="
#define GEOCODER_LANGUAGE	"&lang="

CWazeMapGeocoder::CWazeMapGeocoder(const std::string& strBaseUrl) :
	m_strBaseUrl(strBaseUrl),
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

E_GEO_STATUS_CODE CWazeMapGeocoder::Load(const std::string& strAddress)
{
	std::ostringstream ossUrl;
	CJsonParser jsParser;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_GeoResults.clear();

	std::string strUrlAddress(strAddress);
	stdx::string_helper::replace(strUrlAddress, ' ', '+');

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider(), std::nothrow);

	// Build request
	ossUrl << m_strBaseUrl << GEOCODER_URL << strUrlAddress;

	if (providerApi.getLang().size())
		ossUrl << GEOCODER_LANGUAGE << providerApi.getLang();

	try
	{
		if (jsParser.parse(CInternet::AjaxHttpRequest(ossUrl.str())) == CJsonParser::JSON_SUCCESS)
		{
			const CJsonArray& jsResults = jsParser;

			for (size_t i = 0; i < jsResults.size(); i++)
			{
				const CJsonObject& jsResult = jsResults[i];

				CGeoLocation geoLocation;

				// Read coordinates
				geoLocation.lat(jsResult("location")("lat"));
				geoLocation.lng(jsResult("location")("lon"));

				geoLocation.name(jsResult("name"));
				geoLocation.comment(jsResult("state"));

				if (geoLocation)
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
