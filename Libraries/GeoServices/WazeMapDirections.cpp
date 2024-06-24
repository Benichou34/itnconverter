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

#include <chrono>
#include <thread>
#include <sstream>
#include "WazeMapDirections.h"
#include "jsonParser/JsonParser.h"

using namespace geo;

namespace
{
	constexpr size_t maxRequestStep = 2;
	constexpr std::chrono::milliseconds minRequestduration(500);
	std::chrono::time_point<std::chrono::system_clock> g_lastApiCall(std::chrono::system_clock::now());
}

#define DIRECTION_URL		"/routingRequest?returnJSON=true&returnGeometries=true&returnInstructions=true&timeout=60000&nPaths=1"
#define DIRECTION_FROM		"&from="
#define DIRECTION_TO		"&to="
#define DIRECTION_X         "x:"
#define DIRECTION_Y         "+y:"

CWazeMapDirections::CWazeMapDirections(const std::string& strBaseUrl) :
	m_strBaseUrl(strBaseUrl)
{
}

GeoRouteTravelOptions CWazeMapDirections::getSupportedTravelOptions() const noexcept
{
	GeoRouteTravelOptions gRouteTravelOptions;
	gRouteTravelOptions[GeoVehicleType::Default] = std::make_unique<CGeoBasicAcceptedRouteOptions>();

	return gRouteTravelOptions;
}

size_t CWazeMapDirections::getMaximumStepsByRequest() const noexcept
{
	return maxRequestStep;
}

E_GEO_STATUS_CODE CWazeMapDirections::getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request)
{
	std::ostringstream ossUrl;

	if (vehicleType != GeoVehicleType::Default)
		return E_GEO_INVALID_REQUEST;

	if (cgLatLngs.size() > maxRequestStep)
		return E_GEO_BAD_ARGUMENTS;

	const CGeoLatLng& gLatLng1 = cgLatLngs.front();
	const CGeoLatLng& gLatLng2 = cgLatLngs.back();
	CGeoRoute gRoute(vehicleType, cgOptions);

	// Build cache request
	ossUrl << m_strBaseUrl << DIRECTION_URL;
	ossUrl << DIRECTION_FROM << DIRECTION_X << gLatLng1.lng() << DIRECTION_Y << gLatLng1.lat();
	ossUrl << DIRECTION_TO << DIRECTION_X << gLatLng2.lng() << DIRECTION_Y << gLatLng2.lat();

	request.strReferrer = m_strBaseUrl;
	request.strUrl = ossUrl.str();

	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - g_lastApiCall);
	if (elapsed_ms < minRequestduration)
		std::this_thread::sleep_for(minRequestduration - elapsed_ms);

	g_lastApiCall = std::chrono::system_clock::now();
	return E_GEO_OK;
}

E_GEO_STATUS_CODE CWazeMapDirections::parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes)
{
	try
	{
		CJsonParser jsParser;
		jsParser << strRequest;

		// Error: 502|Could not locate start point
		// Error: 401|Could not locate end point
		if (jsParser.exist("error"))
			return E_GEO_NOT_FOUND;

		CGeoRoute gRoute(vehicleType, cgOptions);

		// Create polyline
		CGeoLatLngs& gPathLatLngs = gRoute.polyline().getPath();
		const CJsonArray& jsCoordinatesArray = jsParser("coords");
		for (size_t j = 0; j < jsCoordinatesArray.size(); j++)
		{
			CGeoLatLng gLatLng;

			gLatLng.lng(jsCoordinatesArray[j]("x"));
			gLatLng.lat(jsCoordinatesArray[j]("y"));

			gPathLatLngs.push_back(gLatLng);
		}

		gRoute.locations().push_back(gPathLatLngs.front());
		gRoute.locations().push_back(gPathLatLngs.back());

		// Read Steps
		const CJsonArray& jsSteps = jsParser("response")("results");
		for (size_t j = 0; j < jsSteps.size(); j++)
		{
			const CJsonObject& jsStep = jsSteps[j];
			CGeoStep gStep;

			gStep.lng(jsStep("path")("x"));
			gStep.lat(jsStep("path")("y"));
			//gStep.instructions() = jsStep("instruction")("name");
			gStep.summary().assign(jsStep("length"), jsStep("crossTime"));

			gRoute.summary() += gStep.summary();
			gRoute.push_back(gStep);
		}

		if (cgOptions.isLinked())
		{
			if (vecRoutes.empty())
				vecRoutes.push_back(gRoute);
			else
				vecRoutes.front() += gRoute;
		}
		else
		{
			vecRoutes.push_back(gRoute);
		}
	}
	catch (CJsonException&)
	{
		return E_GEO_INVALID_REQUEST;
	}

	return E_GEO_OK;
}
