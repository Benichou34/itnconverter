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

#ifndef _WAZE_MAP_DIRECTIONS_H_INCLUDED_
#define _WAZE_MAP_DIRECTIONS_H_INCLUDED_

#include "GeoBaseDirections.h"

namespace geo
{
	class CWazeMapDirections : public CGeoBaseDirections
	{
	public:
		CWazeMapDirections(const std::string& strBaseUrl);
		~CWazeMapDirections() override = default;

		GeoRouteTravelOptions getSupportedTravelOptions() const noexcept override;

	private:
		size_t getMaximumStepsByRequest() const noexcept override;
		E_GEO_STATUS_CODE getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request) override;
		E_GEO_STATUS_CODE parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes) override;

	private:
		std::string m_strBaseUrl;
	};

	class CWazeUSAMapDirections : public CWazeMapDirections
	{
	public:
		CWazeUSAMapDirections() : CWazeMapDirections("https://www.waze.com/RoutingManager") {}
		~CWazeUSAMapDirections() final = default;

		E_GEO_PROVIDER getProvider() const noexcept final { return E_GEO_PROVIDER_WAZE_MAP; }
	};

	class CWazeWorldMapDirections : public CWazeMapDirections
	{
	public:
		CWazeWorldMapDirections() : CWazeMapDirections("https://www.waze.com/row-RoutingManager") {}
		~CWazeWorldMapDirections() final = default;

		E_GEO_PROVIDER getProvider() const noexcept final { return E_GEO_PROVIDER_WAZE_WORLD_MAP; }
	};
} // namespace geo

#endif // _WAZE_MAP_DIRECTIONS_H_INCLUDED_
