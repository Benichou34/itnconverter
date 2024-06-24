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
#include "ViaMichelinTools.h"
#include "GeoLocations.h"
#include "GeoSummary.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

E_GEO_STATUS_CODE CViaMichelinTools::GetStatusCode(int iStatus)
{
	switch (iStatus)
	{
	case 1: // Bad request
		return E_GEO_INVALID_REQUEST;

	case 2: // Access denied (Invalid Authentication params)
		return E_GEO_REQUEST_DENIED;

	case 3: // Abnormal service termination
		return E_GEO_UNKNOWN_ERROR;

	case 4: // Invalid request parameter value
		return E_GEO_INVALID_REQUEST;

	case 109: // Itinerary calculation error : itinerary arrival not reachable
		return E_GEO_NOT_FOUND;

	default:
		break;
	}

	return E_GEO_UNKNOWN_ERROR;
}

void CViaMichelinTools::fromJsonToLatLng(const CJsonObject& jsObject, CGeoLatLng& geoLatLng)
{
	// Read coordinates
	geoLatLng.coords(jsObject("lat"), jsObject("lon"));
}

void CViaMichelinTools::fromJsonApiToLocation(const CJsonObject& jsObject, CGeoLocation& geoLocation, std::string& id)
{
	id = jsObject("id");

	// Read address
	std::string strName;
	if (jsObject.exist("formattedAddressLine"))
		strName = jsObject("formattedAddressLine");

	if (jsObject.exist("formattedCityLine"))
	{
		if (!strName.empty())
			strName += std::string(" ");
		strName += static_cast<const std::string&>(jsObject("formattedCityLine"));
	}

	geoLocation.name(strName);

	std::string strComment(jsObject("area"));
	if (jsObject.exist("countryLabel"))
		strComment += std::string(", ") + static_cast<const std::string&>(jsObject("countryLabel"));
	geoLocation.comment(strComment);

	fromJsonToLatLng(jsObject("coords"), geoLocation);
}

void CViaMichelinTools::fromJsonApiToLocations(const CJsonArray& jsArray, CGeoLocations& geoLocations, std::vector<std::string>& ids)
{
	for (size_t i = 0; i < jsArray.size(); i++)
	{
		CGeoLocation gLocation;
		std::string id;

		fromJsonApiToLocation(jsArray[i]("location"), gLocation, id);

		if (gLocation)
		{
			geoLocations.push_back(gLocation);
			ids.push_back(id);
		}
	}
}

void CViaMichelinTools::fromJsonToSummary(const CJsonObject& jsObject, CGeoSummary& geoSummary)
{
	const CJsonObject& jsSummary = jsObject("summaries")[0];
	geoSummary.assign(jsSummary("totalDist"), jsSummary("totalTime"));
}

std::string CViaMichelinTools::fromLatLngToApiUrlValue(const CGeoLatLng& geoLatLng)
{
	std::ostringstream oss;
	oss << geoLatLng.lng() << ':' << geoLatLng.lat();
	return oss.str();
}

double CViaMichelinTools::GetCoordFromString(const std::string::const_iterator& citEnd, std::string::const_iterator& citCur)
{
	long long llCoord = 0;
	long long cTmp = 32;

	for (int i = 0; (cTmp & 32) && i < 6 && citCur != citEnd; i++, ++citCur)
	{
		cTmp = *citCur - 63;
		llCoord |= (cTmp & 31) << (i * 5);
	}

	if ((llCoord & 1) == 1)
		llCoord = ~(llCoord >> 1);
	else
		llCoord = llCoord >> 1;

	return static_cast<double>(llCoord) / 100000.;
}

void CViaMichelinTools::getCoordsFromString(const std::string& strCoords, CGeoLatLngs& cgLatLngs)
{
	std::string::const_iterator cit = strCoords.begin() + 2;

	double dRefLat = GetCoordFromString(strCoords.end(), cit);
	double dRefLng = GetCoordFromString(strCoords.end(), cit);

	while (cit != strCoords.end())
	{
		dRefLat += GetCoordFromString(strCoords.end(), cit);
		dRefLng += GetCoordFromString(strCoords.end(), cit);

		cgLatLngs.push_back(CGeoLatLng(dRefLat, dRefLng));
	}
}
