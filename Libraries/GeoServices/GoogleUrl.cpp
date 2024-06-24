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
#include <algorithm>
#include <cctype>
#include "GoogleUrl.h"
#include "GoogleTools.h"
#include "GoogleApiGeocoder.h"
#include "GeoLocation.h"
#include "jsonParser/JsonParser.h"
#include "ToolsLibrary/Base64.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "stdx/string_helper.h"
#include "stdx/uri_helper.h"
#include "stdx/guard.h"

using namespace geo;

namespace
{
	const std::string c_strMapsDir("/maps/dir/");
	const std::string c_strMapsPlace("/maps/place/");
	const std::string c_strUrlData("data=");
	const std::string c_strViewAll("am=");
	const std::string c_strLanguageFlag("hl");
	// /am=t = view all with steps
}

bool CGoogleUrl::isValidUrl(const std::string& strUrl)
{
	return (strUrl.find("maps.google.") != std::string::npos || strUrl.find("www.google.") != std::string::npos);
}

void CGoogleUrl::UrlToLocations(const std::string& strUrl, CGeoLocations& glocations)
{
	stdx::string_helper::map spltRequest;
	stdx::string_helper::split(stdx::uri_helper::unescape(strUrl), spltRequest, stdx::find_first_of("?&"), stdx::find_first("="));
	if (spltRequest.empty())
		return;

	CGeoProvider& providerApi = CGeoProviders::instance().get(E_GEO_PROVIDER_GOOGLE_API, std::nothrow);

	std::string strApiLang = providerApi.getLang(); // Save Api language
	stdx::function_guard endGuard([&]() { providerApi.setLang(strApiLang); });// Restore Api language

	if (spltRequest.count(c_strLanguageFlag))
		providerApi.setLang(spltRequest[c_strLanguageFlag]);

	std::string::size_type pos = strUrl.find(c_strMapsDir);
	if (pos != std::string::npos)
		pos += c_strMapsDir.size();
	else
	{
		pos = strUrl.find(c_strMapsPlace);
		if (pos != std::string::npos)
			pos += c_strMapsPlace.size();
	}

	if (pos != std::string::npos)
	{
		stdx::string_helper::vector spltStep;
		stdx::string_helper::split(strUrl.substr(pos), spltStep, stdx::find_first("/"));

		for (stdx::string_helper::vector::iterator it = spltStep.begin(); it != spltStep.end(); ++it)
		{
			if (it->find(c_strUrlData) != std::string::npos)
			{
				stdx::string_helper::vector spltSegment;
				stdx::string_helper::split(*it, spltSegment, stdx::find_first("!"));

				if (spltSegment.empty() || glocations.empty())
					continue;

				std::vector<CGeoLocations> vecSegments;
				vecSegments.reserve(glocations.size());

				stdx::string_helper::vector::iterator itSeg = spltSegment.begin();
				while (itSeg != spltSegment.end())
				{
					if (itSeg->size() > 2 && (*itSeg)[0] == '1' && (*itSeg)[1] == 'm')
					{
						size_t counter = stdx::string_helper::string_to<size_t>(itSeg->substr(2));
						++itSeg;

						vecSegments.push_back(CGeoLocations());
						CGeoLocation glocation;

						while (itSeg != spltSegment.end() && counter > 0)
						{
							if ((*itSeg)[0] == '2' && (*itSeg)[1] == 's')
							{
								CGoogleTools::fromUrlNameToLocationName(itSeg->substr(2), glocation);
							}
							else if ((*itSeg)[0] == '1' && (*itSeg)[1] == 'd')
							{
								glocation.lng(stdx::string_helper::string_to<double>(itSeg->substr(2)));
							}
							else if ((*itSeg)[0] == '2' && (*itSeg)[1] == 'd')
							{
								glocation.lat(stdx::string_helper::string_to<double>(itSeg->substr(2)));
								vecSegments.back().push_back(glocation);
								glocation.clear();
							}
							--counter;
							++itSeg;
						}
					}
					else
					{
						++itSeg;
					}
				}

				if (vecSegments.size() > glocations.size())
					vecSegments.resize(glocations.size());

				CGeoLocations::iterator itLocation = glocations.begin();
				for (std::vector<CGeoLocations>::iterator itSegtments = vecSegments.begin(); itSegtments != vecSegments.end(); ++itSegtments)
				{
					if (itSegtments->empty())
						continue;

					CGeoLocations::iterator itSegtment = itSegtments->begin();
					if (!(*itLocation))
					{
						if (itSegtment->name().empty())
							static_cast<CGeoLatLng&>(*itLocation) = *itSegtment;
						else
							*itLocation = *itSegtment;
						++itSegtment;
					}

					glocations.insert(++itLocation, itSegtment, itSegtments->end());
				}
			}
			else if ((*it)[0] != '@' && it->find(c_strViewAll) == std::string::npos) // The character @ indicates the center of the screen and the zoom factor
			{
				auto containAlpha = [](const std::string& str)
				{
					for (std::string::const_iterator cit = str.begin(); cit != str.end(); ++cit)
					{
						if (std::isalpha(*cit))
							return true;
					}

					return false;
				};

				stdx::string_helper::vector spltLocation;

				if (!containAlpha(*it))
					stdx::string_helper::split(*it, spltLocation, stdx::find_first(","));

				CGeoLocation glocation;
				if (spltLocation.size() > 1)
				{
					glocation.lat(stdx::string_helper::string_to<double>(spltLocation[0]));
					glocation.lng(stdx::string_helper::string_to<double>(spltLocation[1]));
				}
				else
				{
					CGoogleTools::fromUrlNameToLocationName(*it, glocation);
				}
				glocations.push_back(glocation);
			}
		}
	}
	else if (spltRequest.count("saddr"))
	{
		CGeoLocation geoLocation;
		stdx::string_helper::vector::iterator it;

		CGoogleTools::fromUrlValueToLocation(spltRequest["saddr"], geoLocation);
		glocations.push_back(geoLocation);

		if (spltRequest.count("daddr"))
		{
			stdx::string_helper::vector spltStep;
			stdx::string_helper::split(spltRequest["daddr"], spltStep, stdx::find_first("+to:"));

			for (it = spltStep.begin(); it != spltStep.end(); ++it)
			{
				CGoogleTools::fromUrlValueToLocation(*it, geoLocation);
				glocations.push_back(geoLocation);
			}
		}

		if (spltRequest.count("geocode"))
		{
			stdx::string_helper::vector spltGeocode;
			stdx::string_helper::split(spltRequest["geocode"], spltGeocode, stdx::find_first(";"));

			if (spltGeocode.size() == glocations.size())
			{
				for (it = spltGeocode.begin(); it != spltGeocode.end(); ++it)
				{
					// Transfrom from base64 (MIME) to base64url (RFC 4648)
					stdx::string_helper::replace(*it, '-', '+');
					stdx::string_helper::replace(*it, '_', '/');

					std::string strGeocode = CBase64::Decode(*it);

					if (strGeocode.size() >= 10)
					{
						CGeoLocation& glocation = glocations[std::distance(spltGeocode.begin(), it)];

						int iLat = (strGeocode[1] & 0xFF) | ((strGeocode[2] & 0xFF) << 8) | ((strGeocode[3] & 0xFF) << 16) | ((strGeocode[4] & 0xFF) << 24);
						int iLng = (strGeocode[6] & 0xFF) | ((strGeocode[7] & 0xFF) << 8) | ((strGeocode[8] & 0xFF) << 16) | ((strGeocode[9] & 0xFF) << 24);

						glocation.lat(iLat / 1000000.);
						glocation.lng(iLng / 1000000.);
						glocation.alt(0);
					}
				}
			}
		}
	}
	else if (spltRequest.count("msid"))
	{
		try
		{
			std::string strNewRequest = strUrl + "&output=js";
			std::string strResponse = CInternet::AjaxHttpRequest(strNewRequest);

			pos = strResponse.find("loadVPage(");
			if (pos != std::string::npos)
				strResponse.erase(0, pos + 10);

			CJsonParser jsParser;
			if (jsParser.parse(strResponse) == CJsonParser::JSON_SUCCESS)
			{
				const CJsonArray& jsMarkersArray = jsParser("ms_map")("kmlOverlays")("markers");
				for (size_t i = 0; i < jsMarkersArray.size(); i++)
				{
					const CJsonObject& jsMarker = jsMarkersArray[i];
					CGeoLocation geoLocation;

					CGoogleTools::fromJsonToLatLng(jsMarker("latlng"), geoLocation);
					geoLocation.name(jsMarker("name"));
					geoLocation.comment(jsMarker("description"));

					glocations.push_back(geoLocation);
				}
			}
		}
		catch (CJsonException&)
		{
		}
		catch (CInternetException&)
		{
		}
	}

	// Try to geocode empty locations
	for (CGeoLocations::iterator itLocations = glocations.begin(); itLocations != glocations.end(); ++itLocations)
	{
		if (!(*itLocations))
		{
			try
			{
				CGoogleApiGeocoder gGeocoder(itLocations->name());
				if (gGeocoder.getStatus() == E_GEO_OK)
				{
					const CGeoLocations& geoResults = gGeocoder.getResults();
					if (geoResults.size())
						static_cast<CGeoLatLng&>(*itLocations) = geoResults.front();
				}
			}
			catch (CInternetException&)
			{
			}
		}
	}

	// Remove duplicated locations
	glocations.resize(std::distance(glocations.begin(), std::unique(glocations.begin(), glocations.end(), [](const CGeoLatLng& latlng1, const CGeoLatLng& latlng2)->bool
		{
			return latlng1 == latlng2;
		})));
}

void CGoogleUrl::LocationsToUrl(const CGeoLocations& glocations, std::string& strUrl, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, bool bPrint)
{
	std::ostringstream ossUrl;

	// Build request
	ossUrl << "https://www.google.com/maps/dir";

	// Add steps
	for (CGeoLocations::const_iterator it = glocations.begin(); it != glocations.end(); ++it)
		ossUrl << '/' << CGoogleTools::fromLatLngToUrlValue(*it);

	// Print
	if (bPrint)
		ossUrl << '/' << c_strViewAll << 't';

	std::vector<std::string> datas;

	if (cgOptions.typeIs(GeoRouteOptionsType::Road))
	{
		std::vector<std::string> options;

		if (!static_cast<const CGeoRoadRouteOptions&>(cgOptions).takeHighway())
			options.push_back("1b1");
		if (!static_cast<const CGeoRoadRouteOptions&>(cgOptions).takeTolls())
			options.push_back("2b1");

		if (!options.empty())
		{
			std::ostringstream oss;

			oss << "2m" << options.size();
			datas.push_back(oss.str());
			datas.insert(datas.end(), options.begin(), options.end());
		}
	}

	switch (vehicleType)
	{
	case GeoVehicleType::Pedestrian:
		datas.push_back("3e2");
		break;

	case GeoVehicleType::Bike:
		datas.push_back("3e1");
		break;

	default:
		if (!datas.empty())
			datas.push_back("3e0"); // Car
		break;
	}

	if (!datas.empty())
	{
		size_t lenght = glocations.size() + datas.size();

		ossUrl << '/' << c_strUrlData << "4m" << lenght + 1;
		ossUrl << '!' << "4m" << lenght;

		for (size_t i = 0; i < glocations.size(); ++i)
			ossUrl << "!1m0";

		for (std::vector<std::string>::iterator it = datas.begin(); it != datas.end(); ++it)
			ossUrl << '!' << *it;
	}

	const CGeoProvider& providerApi = CGeoProviders::instance().get(E_GEO_PROVIDER_GOOGLE_API, std::nothrow);
	if (!providerApi.getLang().empty())
		ossUrl << '?' << c_strLanguageFlag << '=' << providerApi.getLang();

	strUrl = ossUrl.str();
}
