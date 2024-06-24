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

#include "stdafx.h"
#include "AsyncRouteCalculation.h"

CAsyncRouteCalculation::CAsyncRouteCalculation() :
	m_Requests(1)
{
}

CAsyncRouteCalculation::~CAsyncRouteCalculation()
{
	m_Requests.clear();
	bool bDirections = false;

	{
		std::lock_guard<std::recursive_mutex> mlg(m_Mutex);
		bDirections = m_gDirections;
	}

	if (bDirections)
		m_gDirections->getStatus();
}

void CAsyncRouteCalculation::setProvider(geo::E_GEO_PROVIDER eGeoProvider)
{
	std::lock_guard<std::recursive_mutex> mlg(m_Mutex);

	if (!m_gDirections || m_gDirections->getProvider() != eGeoProvider)
	{
		m_gDirections = eGeoProvider;
		m_gDirections->setEndCallback([&](geo::E_GEO_STATUS_CODE eStatusCode, const geo::GeoRoutes& gRoutes)
			{
				std::lock_guard<std::recursive_mutex> mlg(m_Mutex);

				if (m_EndCallback)
					try { m_EndCallback(eStatusCode, gRoutes); }
				catch (...) {}

				Request request;
				if (m_Requests.try_pop_front(request) == stdx::cq_status::no_timeout)
				{
					m_EndCallback = request.EndCallback;
					m_gDirections->Load(request.cgLatLngs, request.vehicleType, *request.cgOptions);
				}
				else
				{
					m_EndCallback = nullptr;
				}
			});
	}
}

void CAsyncRouteCalculation::PostRequest(const geo::CGeoLatLngs& cgLatLngs, geo::GeoVehicleType::type_t vehicleType, const geo::CGeoRouteOptions& cgOptions, const CallbackFunction& EndCallback)
{
	if (!EndCallback)
		return;

	std::lock_guard<std::recursive_mutex> mlg(m_Mutex);

	if (!m_EndCallback) // No current request
	{
		m_EndCallback = EndCallback;
		m_gDirections->Load(cgLatLngs, vehicleType, cgOptions);
	}
	else
	{
		m_Requests.push_front(std::move(Request(cgLatLngs, vehicleType, cgOptions, EndCallback)), false);
	}
}

void CAsyncRouteCalculation::PostRequest(const geo::CGeoLatLng& gStart, const geo::CGeoLatLng& gStop, geo::GeoVehicleType::type_t vehicleType, const geo::CGeoRouteOptions& cgOptions, const CallbackFunction& EndCallback)
{
	geo::CGeoLatLngs cgLatLngs;

	cgLatLngs.push_back(gStart);
	cgLatLngs.push_back(gStop);

	PostRequest(cgLatLngs, vehicleType, cgOptions, EndCallback);
}
