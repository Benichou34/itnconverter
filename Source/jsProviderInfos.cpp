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

#include "stdafx.h"
#include "ITN Converter.h"
#include "jsProviderInfos.h"
#include "jsonParser/JsonParser.h"
#include "stdx/string_helper.h"
#include "GeoServices/GeoDirectionsFactory.h"

jsProviderInfos::jsProviderInfos(geo::E_GEO_PROVIDER provider) :
	m_provider(provider),
	m_bAvailable(false)
{
}

bool jsProviderInfos::operator== (const jsProviderInfos& providerInfos) const
{
	return m_provider == providerInfos.m_provider
		&& m_bAvailable == providerInfos.m_bAvailable
		&& m_strKey == providerInfos.m_strKey
		&& m_strReferer == providerInfos.m_strReferer;
}

bool jsProviderInfos::operator!= (const jsProviderInfos& providerInfos) const
{
	return !operator== (providerInfos);
}

bool jsProvidersInfos::fromJs(const std::string& strContent)
{
	CJsonParser jsParser;
	if (!strContent.empty() && jsParser.parse(strContent) == CJsonParser::JSON_SUCCESS)
	{
		// Lecture du fichier de config des cartes
		const CJsonArray& jsProviders = jsParser;
		for (size_t i = 0; i < jsProviders.size(); ++i)
		{
			const CJsonValue& jsProvider = jsProviders[i];

			geo::E_GEO_PROVIDER eRouteProvider = static_cast<geo::E_GEO_PROVIDER>(static_cast<int>(jsProvider("id")));

			// Check validity of route provider
			geo::CGeoDirections geoDirections(eRouteProvider, std::nothrow);
			if (!geoDirections)
				continue;

			jsProviderInfos providerInfos(eRouteProvider);
			if (jsProvider.exist("key"))
				providerInfos.setKey(jsProvider("key"));
			if (jsProvider.exist("lang"))
				providerInfos.setLang(jsProvider("lang"));
			if (jsProvider.exist("referer"))
				providerInfos.setReferer(jsProvider("referer"));
			if (jsProvider.exist("available"))
				providerInfos.setAvailable(jsProvider("available"));

			insert(std::make_pair(eRouteProvider, providerInfos));
		}
	}

	return true;
}
