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

#ifndef __ASYNC_ROUTE_CALCULATION_H_
#define __ASYNC_ROUTE_CALCULATION_H_

#include <functional>
#include "stdx/concurrent_queue.h"
#include "GeoServices/GeoLatLngs.h"
#include "GeoServices/GeoApi.h"
#include "GeoServices/GeoDirectionsFactory.h"

class CAsyncRouteCalculation
{
public:
	CAsyncRouteCalculation();
	~CAsyncRouteCalculation();

	typedef std::function<void(geo::E_GEO_STATUS_CODE, const geo::GeoRoutes&)> CallbackFunction;

	void setProvider(geo::E_GEO_PROVIDER eGeoProvider);

	void PostRequest(const geo::CGeoLatLngs& cgLatLngs, geo::GeoVehicleType::type_t vehicleType, const geo::CGeoRouteOptions& cgOptions, const CallbackFunction& EndCallback);
	void PostRequest(const geo::CGeoLatLng& gStart, const geo::CGeoLatLng& gStop, geo::GeoVehicleType::type_t vehicleType, const geo::CGeoRouteOptions& cgOptions, const CallbackFunction& EndCallback);

private:
	CAsyncRouteCalculation(CAsyncRouteCalculation&) = delete;
	CAsyncRouteCalculation& operator=(CAsyncRouteCalculation&) = delete;

	struct Request
	{
		geo::CGeoLatLngs cgLatLngs;
		geo::GeoVehicleType::type_t vehicleType;
		stdx::clone_ptr<geo::CGeoRouteOptions> cgOptions;
		CallbackFunction EndCallback;

		Request() { }

		Request(const geo::CGeoLatLngs& _cgLatLngs, geo::GeoVehicleType::type_t _vehicleType, const geo::CGeoRouteOptions& _cgOptions, const CallbackFunction& _EndCallback) :
			cgLatLngs(_cgLatLngs),
			vehicleType(_vehicleType),
			cgOptions(_cgOptions),
			EndCallback(_EndCallback)
		{
		}
	};

	geo::CGeoDirections m_gDirections;
	std::recursive_mutex m_Mutex;
	stdx::concurrent_queue<Request> m_Requests;
	CallbackFunction m_EndCallback;
};

#endif // __ASYNC_ROUTE_CALCULATION_H_
