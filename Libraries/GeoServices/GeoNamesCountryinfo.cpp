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
#include <map>
#include "GeoNamesCountryinfo.h"
#include "GeoNamesTools.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "jsonParser/JsonParser.h"

#define GEONAME_URL			"http://api.geonames.org"
#define GEONAME_REQUEST		"/countryInfo?type=JSON"
#define GEONAME_USERNAME	"&username="
#define GEONAME_LANGUAGE	"&lang="
#define GEONAME_COUNTRY		"&country="

using namespace geo;

CGeoNamesCountryInfo::CGeoNamesCountryInfo() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

CGeoNamesCountryInfo::CGeoNamesCountryInfo(const std::string& strCountryCode)
{
	Load(strCountryCode);
}

E_GEO_STATUS_CODE CGeoNamesCountryInfo::Load(const std::string& strCountryCode)
{
	static std::map<std::string, CGeoNamesCountryInfo> ms_CountryInfo;

	std::map<std::string, CGeoNamesCountryInfo>::iterator it = ms_CountryInfo.find(strCountryCode);
	if(it != ms_CountryInfo.end())
	{
		*this = it->second;
		return m_eStatus;
	}

	std::ostringstream ossUrl;
	CJsonParser jsParser;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_strCountryName.clear();
	m_strCurrencyCode.clear();
	m_strCountryCode.clear();
	m_strCapital.clear();
	m_strLanguages.clear();
	m_strIsoAlpha3.clear();
	m_strContinent.clear();

	const CGeoProvider& providerApi = CGeoProviders::instance().get(getProvider());

	// Build request
	ossUrl << GEONAME_URL << GEONAME_REQUEST << GEONAME_USERNAME << providerApi.getKey();
	if (!providerApi.getLang().empty())
		ossUrl << GEONAME_LANGUAGE << providerApi.getLang();

	ossUrl << GEONAME_COUNTRY << strCountryCode;

	try
	{
		if (jsParser.parse(CInternet::AjaxHttpRequest(ossUrl.str())) == CJsonParser::JSON_SUCCESS)
		{
			if (jsParser.exist("status"))
			{
				m_eStatus = CGeoNamesTools::GetStatusCode(jsParser("status")("value"));
				return m_eStatus;
			}

			const CJsonValue& jsCountryInfo = jsParser("geonames")[0];

			m_strCountryName = static_cast<std::string>(jsCountryInfo("countryName"));
			m_strCurrencyCode = static_cast<std::string>(jsCountryInfo("currencyCode"));
			m_strCountryCode = static_cast<std::string>(jsCountryInfo("countryCode"));
			m_strCapital = static_cast<std::string>(jsCountryInfo("capital"));
			m_strLanguages = static_cast<std::string>(jsCountryInfo("languages"));
			m_strIsoAlpha3 = static_cast<std::string>(jsCountryInfo("isoAlpha3"));
			m_strContinent = static_cast<std::string>(jsCountryInfo("continent"));

			// Set status
			m_eStatus = E_GEO_OK;
			ms_CountryInfo[strCountryCode] = *this;
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
