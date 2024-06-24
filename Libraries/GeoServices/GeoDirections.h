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

#ifndef _GEO_IDIRECTIONS_H_INCLUDED_
#define _GEO_IDIRECTIONS_H_INCLUDED_

#include <functional>
#include <vector>
#include <map>
#include "GeoApi.h"
#include "GeoRoute.h"

namespace geo
{
	class CGeoLatLng;
	class CGeoLatLngs;

	typedef std::vector<CGeoRoute> GeoRoutes;
	typedef std::map<GeoVehicleType::type_t, std::unique_ptr<CGeoAcceptedRouteOptions>> GeoRouteTravelOptions;

	class IGeoDirections
	{
	public:
		virtual ~IGeoDirections() = default;

		virtual void Load(const CGeoLatLng& gStart, const CGeoLatLng& gStop, GeoVehicleType::type_t vehicleType = GeoVehicleType::Default, const CGeoRouteOptions& cgOptions = CGeoRouteOptions()) = 0;
		virtual void Load(const CGeoLatLngs& CGeoLatLngs, GeoVehicleType::type_t vehicleType = GeoVehicleType::Default, const CGeoRouteOptions& cgOptions = CGeoRouteOptions()) = 0;
		virtual void cancel() = 0;

		virtual const GeoRoutes& getRoutes() const = 0;
		virtual E_GEO_STATUS_CODE getStatus(size_t msTimeOut = InfiniteTimeOut) const = 0;

		virtual E_GEO_PROVIDER getProvider() const noexcept = 0;
		virtual GeoRouteTravelOptions getSupportedTravelOptions() const noexcept = 0;

		typedef std::function<void(E_GEO_STATUS_CODE, const GeoRoutes&)> CallbackFunction;
		virtual void setEndCallback(const CallbackFunction& EndCallback) = 0;
	};
} // namespace geo

#endif // _GEO_IDIRECTIONS_H_INCLUDED_