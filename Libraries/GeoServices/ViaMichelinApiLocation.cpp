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
#include "ViaMichelinApiLocation.h"
#include "ViaMichelinTools.h"
#include "jsonParser/JsonParser.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"

using namespace geo;

namespace
{
	const std::string geocoderRequest("http://apir.viamichelin.com/apir/1/location.json/");
	const std::string geocoderCharset("?charset=UTF-8&ie=UTF-8&callback=");
	const std::string geocoderCallback("HTTPResponseLoaded");
	const std::string geocoderAuthKey("&authkey=");
	const std::string geocoderlanguage("&lg=");
}

CViaMichelinApiLocation::CViaMichelinApiLocation() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

CViaMichelinApiLocation::CViaMichelinApiLocation(const std::string& strId)
{
	Load(strId);
}

E_GEO_STATUS_CODE CViaMichelinApiLocation::Load(const std::string& strId)
{
	std::ostringstream ossUrl;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_GeoResults.clear();

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << geocoderRequest << strId << geocoderCharset << geocoderCallback;
	ossUrl << geocoderlanguage << providerApi.getLang() << geocoderAuthKey << providerApi.getKey();

	try
	{
		std::string strResponse = CInternet::AjaxHttpRequest(ossUrl.str());
		size_t pos = strResponse.find(geocoderCallback);
		if (pos == std::string::npos)
			return E_GEO_INVALID_REQUEST;

		CJsonParser jsParser;
		jsParser << strResponse.substr(pos + geocoderCallback.size());

		if (jsParser.exist("error"))
			return CViaMichelinTools::GetStatusCode(jsParser("error")("errorCode"));

		// Read Placemarks
		std::vector<std::string> ids;
		CViaMichelinTools::fromJsonApiToLocations(jsParser("locationList"), m_GeoResults, ids);

		m_eStatus = m_GeoResults.empty() ? E_GEO_ZERO_RESULTS : E_GEO_OK;
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
