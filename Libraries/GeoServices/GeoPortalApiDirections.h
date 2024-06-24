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

#ifndef _GEOPORTAL_API_DIRECTIONS_H_INCLUDED_
#define _GEOPORTAL_API_DIRECTIONS_H_INCLUDED_

#include "GeoBaseDirections.h"

namespace geo
{
	class CGeoPortalApiDirections : public CGeoBaseDirections
	{
	public:
		~CGeoPortalApiDirections() override = default;

		E_GEO_PROVIDER getProvider() const noexcept override { return E_GEO_PROVIDER_GEOPORTAL_API; }
		GeoRouteTravelOptions getSupportedTravelOptions() const noexcept override;

	private:
		size_t getMaximumStepsByRequest() const noexcept override;
		E_GEO_STATUS_CODE getRequestUrl(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, Request& request) override;
		E_GEO_STATUS_CODE parseRequest(const std::string& strRequest, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions, GeoRoutes& vecRoutes) override;
	};
} // namespace geo

#endif // _GEOPORTAL_API_DIRECTIONS_H_INCLUDED_
