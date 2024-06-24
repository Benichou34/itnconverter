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
#include "TomtomApiGeocoder.h"
#include "TomtomTools.h"
#include "GeoLocation.h"
#include "jsonParser/JsonParser.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "stdx/string_helper.h"

using namespace geo;

namespace
{
	const std::string geocoderUrl("https://api.tomtom.com/search/2/geocode/");
	const std::string geocoderKey(".json?key=");
}

CTomtomApiGeocoder::CTomtomApiGeocoder() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

CTomtomApiGeocoder::CTomtomApiGeocoder(const std::string& strAddress)
{
	Load(strAddress);
}

E_GEO_STATUS_CODE CTomtomApiGeocoder::Load(const std::string& strAddress)
{
	std::ostringstream ossUrl;
	CJsonParser jsParser;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_GeoResults.clear();

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << geocoderUrl << strAddress << geocoderKey << providerApi.getKey();

	try
	{
		if (jsParser.parse(CInternet::AjaxHttpRequest(ossUrl.str(), providerApi.getReferer())) == CJsonParser::JSON_SUCCESS)
		{
			// Read results
			const CJsonArray& results = jsParser("results");
			for (size_t i = 0; i < results.size(); ++i)
			{
				const CJsonObject& result = results[0];

				CGeoLocation gLocation;
				gLocation.name(result("address")("freeformAddress"));

				stdx::string_helper::vector vecStrResult;
				stdx::string_helper::split(result("position"), vecStrResult, stdx::find_first(","));
				if (vecStrResult.size() > 1)
				{
					gLocation.lat(stdx::string_helper::string_to<double>(vecStrResult[0]));
					gLocation.lng(stdx::string_helper::string_to<double>(vecStrResult[1]));
				}

				if (gLocation)
					m_GeoResults.push_back(gLocation);
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
