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

#ifndef _GEO_ROUTE_H_INCLUDED_
#define _GEO_ROUTE_H_INCLUDED_

#include "GeoApi.h"
#include "GeoStep.h"
#include "GeoLocations.h"
#include "GeoSummary.h"
#include "GeoPolyline.h"
#include "GeoRouteOptions.h"

namespace geo
{
	class CGeoRoute : public GeoSteps
	{
	public:
		CGeoRoute();
		CGeoRoute(GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& gRouteOptions);
		CGeoRoute(const CGeoRoute& gRoute);
		CGeoRoute(CGeoRoute&& gRoute);
		virtual ~CGeoRoute() = default;

		CGeoRoute& operator+=(const CGeoRoute& gRoute);
		CGeoRoute& operator=(const CGeoRoute& gRoute);
		CGeoRoute& operator+=(CGeoRoute&& gRoute);
		CGeoRoute& operator=(CGeoRoute&& gRoute);

		virtual void clear() throw();

		GeoVehicleType::type_t vehicle() const throw() { return m_vehicleType; }
		const CGeoRouteOptions& options() const throw() { return *m_gRouteOptions; }
		const CGeoLocations& locations() const throw() { return m_cgLocations; }
		const CGeoSummary& summary() const throw() { return m_gSummary; }
		const CGeoPolyline& polyline() const throw() { return m_gPolyLine; }

		CGeoLocations& locations() throw() { return m_cgLocations; }
		CGeoSummary& summary() throw() { return m_gSummary; }
		CGeoPolyline& polyline() throw() { return m_gPolyLine; }

		// Used by Sort Algorithms, operator<
		bool operator< (const CGeoRoute& gRoute) const;

	private:
		GeoVehicleType::type_t m_vehicleType;
		CGeoLocations m_cgLocations;
		CGeoSummary m_gSummary;
		CGeoPolyline m_gPolyLine;
		stdx::clone_ptr<CGeoRouteOptions> m_gRouteOptions;
	};

	CGeoRoute operator+(const CGeoRoute& gRoute1, const CGeoRoute& gRoute2);
} // namespace geo

#endif // _GEO_ROUTE_H_INCLUDED_
