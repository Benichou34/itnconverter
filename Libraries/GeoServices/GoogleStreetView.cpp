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
#include "GoogleStreetView.h"
#include "GoogleTools.h"
#include "GeoSummary.h"
#include "GeoLocation.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "jsonParser/JsonParser.h"
#include "stdx/string_helper.h"

using namespace geo;

#define STREETVIEW_URL		"https://cbks0.googleapis.com/cbk?output=json&oe=utf-8&cb_client=apiv3&ll="

CGoogleStreetView::CGoogleStreetView() :
	m_eStatus(E_GEO_INVALID_REQUEST)
{
}

CGoogleStreetView::CGoogleStreetView(const CGeoLatLng& gLatLng)
{
	Load(gLatLng);
}

E_GEO_STATUS_CODE CGoogleStreetView::Load(const CGeoLatLng& gLatLng)
{
	std::ostringstream ossUrl;
	CJsonParser jsParser;

	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_GeoResults.clear();

	// Build request
	ossUrl << STREETVIEW_URL << CGoogleTools::fromLatLngToUrlValue(gLatLng);

	try
	{
		if (jsParser.parse(CInternet::AjaxHttpRequest(ossUrl.str())) == CJsonParser::JSON_SUCCESS)
		{
			if (!jsParser.exist("Location"))
			{
				m_eStatus = E_GEO_ZERO_RESULTS;
				return m_eStatus;
			}

			const CJsonObject& jsLocation = jsParser("Location");

			// Read location
			CGeoLocation gLocation;
			gLocation.name(jsLocation("description"));
			gLocation.comment(jsLocation("region"));
			gLocation.lng(stdx::string_helper::string_to<double>(jsLocation("lng")));
			gLocation.lat(stdx::string_helper::string_to<double>(jsLocation("lat")));

			m_GeoResults.push_back(gLocation);
			m_eStatus = E_GEO_OK;
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
