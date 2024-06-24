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

#include "GeoRouteOptions.h"

namespace geo
{
	CGeoRouteOptions::CGeoRouteOptions()
		: m_itiType(GeoItineraryType::Recommended)
		, m_bLinked(true)
	{}
	
	CGeoRouteOptions::CGeoRouteOptions(const CGeoAcceptedRouteOptions& options)
		: m_itiType(options.itineraries().defaultValue)
		, m_bLinked(options.linked().defaultValue)
	{}

	void CGeoRouteOptions::merge(const CGeoRouteOptions& other)
	{
		if (m_itiType == GeoItineraryType::Recommended)
			m_itiType = other.m_itiType;

		m_bLinked |= other.m_bLinked;
	}

	std::unique_ptr<CGeoRouteOptions> CGeoRouteOptions::getFromType(GeoRouteOptionsType::type_t optionsType)
	{
		switch (optionsType)
		{
		case GeoRouteOptionsType::Road:
			return std::make_unique<CGeoRoadRouteOptions>();

		case GeoRouteOptionsType::Truck:
			return std::make_unique<CGeoTruckRouteOptions>();

		case GeoRouteOptionsType::Pedestrian:
			return std::make_unique<CGeoPedestrianRouteOptions>();

		case GeoRouteOptionsType::Bike:
			return std::make_unique<CGeoBicycleRouteOptions>();

		case GeoRouteOptionsType::Thrilling:
			return std::make_unique<CGeoThrillingRouteOptions>();

		default:
			return std::make_unique<CGeoRouteOptions>();
		}
	}

	std::unique_ptr<CGeoRouteOptions> CGeoRouteOptions::getFromVehicleType(GeoVehicleType::type_t vehicleType)
	{
		switch (vehicleType)
		{
		case GeoVehicleType::Car:
			return getFromType(GeoRouteOptionsType::Road);

		case GeoVehicleType::Truck:
			return getFromType(GeoRouteOptionsType::Truck);

		case GeoVehicleType::Pedestrian:
			return getFromType(GeoRouteOptionsType::Pedestrian);

		case GeoVehicleType::Bike:
			return getFromType(GeoRouteOptionsType::Bike);

		case GeoVehicleType::Motorbike:
			return getFromType(GeoRouteOptionsType::Thrilling);

		case GeoVehicleType::Default:
		default:
			return getFromType(GeoRouteOptionsType::Basic);
		}
	}

	CGeoRoadRouteOptions::CGeoRoadRouteOptions()
		: m_bHighway(true)
		, m_bTolls(true)
		, m_bBoatFerry(true)
		, m_bRailFerry(true)
		, m_bTunnel(true)
		, m_bDirtRoad(true)
	{}

	CGeoRoadRouteOptions::CGeoRoadRouteOptions(const CGeoAcceptedRoadRouteOptions& options)
		: CGeoRouteOptions(options)
		, m_bHighway(options.highway().defaultValue)
		, m_bTolls(options.tolls().defaultValue)
		, m_bBoatFerry(options.boatFerry().defaultValue)
		, m_bRailFerry(options.railFerry().defaultValue)
		, m_bTunnel(options.tunnel().defaultValue)
		, m_bDirtRoad(options.dirtRoad().defaultValue)
	{}

	void CGeoRoadRouteOptions::merge(const CGeoRouteOptions& other)
	{
		CGeoRouteOptions::merge(other);
			
		if (typeIs(other.type()))
		{
			const CGeoRoadRouteOptions& opt = static_cast<const CGeoRoadRouteOptions&>(other);

			m_bHighway |= opt.m_bHighway;
			m_bTolls |= opt.m_bTolls;
			m_bBoatFerry |= opt.m_bBoatFerry;
			m_bRailFerry |= opt.m_bRailFerry;
			m_bTunnel |= opt.m_bTunnel;
			m_bDirtRoad |= opt.m_bDirtRoad;
		}
	}

	CGeoTruckRouteOptions::CGeoTruckRouteOptions()
		: m_category(GeoTruckCategoryType::no_catory)
		, m_bTractor(false)
		, m_trailersCount(0)
		, m_axleCount(0)
		, m_limitedWeight(0)
		, m_weightPerAxle(0)
		, m_height(0)
		, m_width(0)
		, m_length(0)
	{}

	CGeoTruckRouteOptions::CGeoTruckRouteOptions(const CGeoAcceptedTruckRouteOptions& options)
		: CGeoRoadRouteOptions(options)
		, m_category(options.categories().defaultValue)
		, m_bTractor(options.tractor().defaultValue)
		, m_trailersCount(options.trailersCount().defaultValue)
		, m_axleCount(options.axleCount().defaultValue)
		, m_limitedWeight(options.limitedWeight().defaultValue)
		, m_weightPerAxle(options.weightPerAxle().defaultValue)
		, m_height(options.height().defaultValue)
		, m_width(options.width().defaultValue)
		, m_length(options.length().defaultValue)
	{}

	void CGeoTruckRouteOptions::merge(const CGeoRouteOptions& other)
	{
		CGeoRoadRouteOptions::merge(other);

		if (typeIs(other.type()))
		{
			const CGeoTruckRouteOptions& opt = static_cast<const CGeoTruckRouteOptions&>(other);

			if (m_category == GeoTruckCategoryType::no_catory)
				m_category = opt.m_category;

			m_bTractor |= opt.m_bTractor;
			m_trailersCount = std::max(m_trailersCount, opt.m_trailersCount);
			m_axleCount = std::max(m_axleCount, opt.m_axleCount);
			m_limitedWeight = std::max(m_limitedWeight, opt.m_limitedWeight);
			m_weightPerAxle = std::max(m_weightPerAxle, opt.m_weightPerAxle);
			m_height = std::max(m_height, opt.m_height);
			m_width = std::max(m_width, opt.m_width);
			m_length = std::max(m_length, opt.m_length);
		}
	}

	CGeoThrillingRouteOptions::CGeoThrillingRouteOptions()
		: m_bAlreadyUsedRoads(true)
		, m_hilliness(CGeoAcceptedThrillingRouteOptions::degree_normal)
		, m_windingness(CGeoAcceptedThrillingRouteOptions::level_normal)
	{}

	CGeoThrillingRouteOptions::CGeoThrillingRouteOptions(const CGeoAcceptedThrillingRouteOptions& options)
		: CGeoRoadRouteOptions(options)
		, m_bAlreadyUsedRoads(options.alreadyUsedRoads().defaultValue)
		, m_hilliness(options.hilliness().defaultValue)
		, m_windingness(options.windingness().defaultValue)
	{}
	
	void CGeoThrillingRouteOptions::merge(const CGeoRouteOptions& other)
	{
		CGeoRoadRouteOptions::merge(other);

		if (typeIs(other.type()))
		{
			const CGeoThrillingRouteOptions& opt = static_cast<const CGeoThrillingRouteOptions&>(other);

			m_bAlreadyUsedRoads |= opt.m_bAlreadyUsedRoads;

			if (m_hilliness == CGeoAcceptedThrillingRouteOptions::degree_normal)
				m_hilliness = opt.m_hilliness;

			if (m_windingness == CGeoAcceptedThrillingRouteOptions::level_normal)
				m_windingness = opt.m_windingness;
		}
	}

	CGeoPedestrianRouteOptions::CGeoPedestrianRouteOptions()
		: m_bPark(false)
	{}

	CGeoPedestrianRouteOptions::CGeoPedestrianRouteOptions(const CGeoAcceptedPedestrianRouteOptions& options)
		: CGeoRouteOptions(options)
		, m_bPark(options.park().defaultValue)
	{}
	
	void CGeoPedestrianRouteOptions::merge(const CGeoRouteOptions& other)
	{
		CGeoRouteOptions::merge(other);

		if (typeIs(other.type()))
		{
			const CGeoPedestrianRouteOptions& opt = static_cast<const CGeoPedestrianRouteOptions&>(other);
			m_bPark |= opt.m_bPark;
		}
	}
} // namespace geo

