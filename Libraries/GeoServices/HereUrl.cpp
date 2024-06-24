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
#include "HereUrl.h"
#include "HereApi.h"
#include "GeoLocations.h"
#include "ToolsLibrary/Internet.h"
#include "stdx/string_helper.h"
#include "stdx/uri_helper.h"

using namespace geo;

namespace
{
	const std::string c_strMapsDir("here.com/directions/");
	const std::string c_strDrive("drive");
	const std::string c_strWalk("walk");
	const std::string c_strUrlData("map=");
}

bool CHereUrl::isValidUrl(const std::string& strUrl)
{
	return (strUrl.find(c_strMapsDir) != std::string::npos);
}

void CHereUrl::UrlToLocations(const std::string& strUrl, CGeoLocations& glocations)
{
	std::string::size_type pos = strUrl.find(c_strMapsDir);
	if (pos == std::string::npos)
		return;

	stdx::string_helper::vector spltSteps;
	stdx::string_helper::split(strUrl.substr(pos + c_strMapsDir.size()), spltSteps, stdx::find_first_of("/?"), false);

	for (stdx::string_helper::vector::iterator it = spltSteps.begin(); it != spltSteps.end(); ++it)
	{
		if ((*it)[0] == '@' || it->find(c_strUrlData) != std::string::npos)
			continue;

		stdx::string_helper::vector spltStep;
		stdx::string_helper::split(stdx::uri_helper::unescape(*it), spltStep, stdx::find_first(":"));

		if (spltStep.size() < 2)
			continue;

		stdx::string_helper::vector spltLatlng;
		stdx::string_helper::split(spltStep[1], spltLatlng, stdx::find_first(","));

		if (spltLatlng.size() < 2)
			continue;

		CGeoLocation glocation;
		glocation.lat(stdx::string_helper::string_to<double>(spltLatlng[0]));
		glocation.lng(stdx::string_helper::string_to<double>(spltLatlng[1]));

		stdx::string_helper::replace(spltStep[0], '-', ' ');
		glocation.name(spltStep[0]);
		glocations.push_back(glocation);
	}
}

void CHereUrl::LocationsToUrl(const CGeoLocations& glocations, std::string& strUrl, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions&, bool)
{
	std::ostringstream ossUrl;

	// Build request
	ossUrl << "https://" << c_strMapsDir;

	if (vehicleType == GeoVehicleType::Pedestrian)
		ossUrl << c_strWalk;
	else
		ossUrl << c_strDrive;

	// Add steps
	for (CGeoLocations::const_iterator it = glocations.begin(); it != glocations.end(); ++it)
	{
		std::string strName = it->name();
		stdx::string_helper::trimleft(strName);
		stdx::string_helper::trimright(strName);
		stdx::string_helper::remove(strName, '-');
		stdx::string_helper::replace(strName, ' ', '-');

		ossUrl << '/' << strName << ':' << it->lat() << ',' << it->lng();
	}

	strUrl = stdx::uri_helper::encode(ossUrl.str());
}
