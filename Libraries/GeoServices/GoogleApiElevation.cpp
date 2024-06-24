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
#include "GoogleApiElevation.h"
#include "GoogleTools.h"
#include "GeoLocation.h"
#include "jsonParser/JsonParser.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"

using namespace geo;

#define GEOCODER_URL		"https://maps.googleapis.com/maps/api/elevation/json?sensor=false&locations="

CGoogleApiElevation::CGoogleApiElevation() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

CGoogleApiElevation::CGoogleApiElevation(const CGeoLatLng& gLatLng)
{
	Load(gLatLng);
}

E_GEO_STATUS_CODE CGoogleApiElevation::Load(const CGeoLatLng& gLatLng)
{
	std::ostringstream ossUrl;
	CJsonParser jsParser;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_ElvResults.clear();

	// Build request
	ossUrl << GEOCODER_URL << gLatLng;

	try
	{
		if (jsParser.parse(CInternet::AjaxHttpRequest(ossUrl.str())) == CJsonParser::JSON_SUCCESS)
		{
			// Read status
			m_eStatus = CGoogleTools::GetStatusCode(jsParser("status"));
			if (m_eStatus != E_GEO_OK)
				return m_eStatus;

			// Read results
			const CJsonArray& jsArray = jsParser("results");
			for (size_t i = 0; i < jsArray.size(); i++)
			{
				const CJsonObject& jsObject = jsArray[i];
				CGeoLatLng gAltLatLng;

				CGoogleTools::fromJsonToLatLng(jsObject("location"), gAltLatLng);
				gAltLatLng.alt(jsObject("elevation"));

				if (gAltLatLng)
					m_ElvResults.push_back(gAltLatLng);
			}
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
