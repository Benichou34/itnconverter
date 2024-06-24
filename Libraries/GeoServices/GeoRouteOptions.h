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

#ifndef _GEO_ROUTE_OPTIONS_H_INCLUDED_
#define _GEO_ROUTE_OPTIONS_H_INCLUDED_

#include <set>
#include <algorithm>
#include "stdx/prototype.h"

namespace geo
{
	struct GeoRouteOptionsType
	{
		typedef enum
		{
			Basic = 0x0001,
			Road = 0x0002,
			Truck = 0x0004 | Road,
			Thrilling = 0x0008 | Road,
			Bike = 0x0010,
			Pedestrian = 0x0020
		} type_t;
	};

	struct GeoVehicleType
	{
		typedef enum // Type of vehicle used by the itinerary calculation.
		{
			Default,
			Car,
			Truck,
			Pedestrian,
			Bike,
			Motorbike,
			Campervan,
			Bus
		} type_t;
	};

	struct GeoItineraryType
	{
		typedef enum // Type of itinerary to be calculated.
		{
			Recommended,
			Quickest,
			Shortest,
			Sightseeing,
			Economical,
			Thrilling
		} type_t;
	};

	struct GeoTruckCategoryType
	{
		typedef enum
		{
			no_catory,
			public_transport,
			commun_goods,
			hazardous_explosive,
			hazardous_gas,
			hazardous_flammable,
			hazardous_combustible,
			hazardous_organic,
			hazardous_poison,
			hazardous_radioActive,
			hazardous_corrosive,
			hazardous_poisonousInhalation,
			hazardous_harmfulToWater,
			hazardous_other,
			hazardous_all
		} type_t;
	};

	template <class T, class D = T>
	struct GeoParameter
	{
		T acceptedValue;
		D defaultValue;
	};

	// This class describe the options accepted by a direction service
	class CGeoAcceptedRouteOptions
	{
	public:
		virtual ~CGeoAcceptedRouteOptions() = default;

		virtual bool preview() const = 0;
		virtual GeoParameter<bool> linked() const = 0;
		virtual GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const = 0; // list of supported itineraries.

		virtual GeoRouteOptionsType::type_t type() const { return GeoRouteOptionsType::Basic; }
		bool typeIs(GeoRouteOptionsType::type_t t) const { return (type() & t) == t; }
	};

	class CGeoAcceptedBicycleRouteOptions : public CGeoAcceptedRouteOptions
	{
	public:
		~CGeoAcceptedBicycleRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Bike; }
	};

	class CGeoAcceptedPedestrianRouteOptions : public CGeoAcceptedRouteOptions
	{
	public:
		~CGeoAcceptedPedestrianRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Pedestrian; }

		virtual GeoParameter<bool> park() const = 0; // links through parks
	};

	class CGeoAcceptedRoadRouteOptions : public CGeoAcceptedRouteOptions
	{
	public:
		~CGeoAcceptedRoadRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Road; }

		virtual GeoParameter<bool> highway() const = 0;
		virtual GeoParameter<bool> tolls() const = 0;
		virtual GeoParameter<bool> boatFerry() const = 0;
		virtual GeoParameter<bool> railFerry() const = 0;
		virtual GeoParameter<bool> tunnel() const = 0;
		virtual GeoParameter<bool> dirtRoad() const = 0;
	};

	class CGeoAcceptedTruckRouteOptions : public CGeoAcceptedRoadRouteOptions
	{
	public:
		~CGeoAcceptedTruckRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Truck; }

		virtual GeoParameter<bool> tractor() const = 0; // Tractors with semi-trailers
		virtual GeoParameter<size_t> trailersCount() const = 0; // Maximum number of trailers pulled by a vehicle
		virtual GeoParameter<size_t> axleCount() const = 0; // Maximum number of axles
		virtual GeoParameter<std::set<GeoTruckCategoryType::type_t>, GeoTruckCategoryType::type_t> categories() const = 0; // list of hazardous materials in the vehicle
		virtual GeoParameter<size_t> limitedWeight() const = 0; // Maximum vehicle weight including trailers and shipped goods, in tenth of tons
		virtual GeoParameter<size_t> weightPerAxle() const = 0; // Maximum vehicle weight per axle in tenth of tons
		virtual GeoParameter<size_t> height() const = 0; // Maximum vehicle height in tenth of meters
		virtual GeoParameter<size_t> width() const = 0; // Maximum vehicle width in tenth of meters
		virtual GeoParameter<size_t> length() const = 0; // Maximum vehicle length in tenth of meters
	};

	class CGeoAcceptedThrillingRouteOptions : public CGeoAcceptedRoadRouteOptions
	{
	public:
		typedef enum
		{
			degree_low,
			degree_normal,
			degree_high
		} degree_t;

		typedef enum
		{
			level_low,
			level_normal,
			level_high
		} level_t;

		~CGeoAcceptedThrillingRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Thrilling; }

		virtual GeoParameter<std::set<degree_t>, degree_t> hilliness() const = 0; // Degrees of hilliness for thrilling route.
		virtual GeoParameter<std::set<level_t>, level_t> windingness() const = 0; // Levels of turns for thrilling route.
		virtual GeoParameter<bool> alreadyUsedRoads() const = 0; // Avoids using the same road multiple times.
	};

	class CGeoBasicAcceptedRouteOptions : public CGeoAcceptedRouteOptions
	{
	public:
		~CGeoBasicAcceptedRouteOptions() override = default;

		bool preview() const override { return true; }
		GeoParameter<bool> linked() const override { return { true, true }; }
		GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const  override { return { std::set<GeoItineraryType::type_t>(), GeoItineraryType::Recommended }; }
	};

	class CGeoBasicAcceptedRoadRouteOptions : public CGeoAcceptedRoadRouteOptions
	{
	public:
		~CGeoBasicAcceptedRoadRouteOptions() override = default;

		bool preview() const override { return true; }
		GeoParameter<bool> linked() const override { return { true, true }; }
		GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const  override { return { std::set<GeoItineraryType::type_t>(), GeoItineraryType::Recommended }; }

		GeoParameter<bool> highway() const override { return { false, false }; }
		GeoParameter<bool> tolls() const override { return { false, false }; }
		GeoParameter<bool> boatFerry() const override { return { false, false }; }
		GeoParameter<bool> railFerry() const override { return { false, false }; }
		GeoParameter<bool> tunnel() const override { return { false, false }; }
		GeoParameter<bool> dirtRoad() const override { return { false, false }; }
	};

	class CGeoBasicAcceptedBicycleRouteOptions : public CGeoAcceptedBicycleRouteOptions
	{
	public:
		~CGeoBasicAcceptedBicycleRouteOptions() override = default;

		bool preview() const override { return true; }
		GeoParameter<bool> linked() const override { return { true, true }; }
		GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const  override { return { std::set<GeoItineraryType::type_t>(), GeoItineraryType::Recommended }; }
	};

	class CGeoBasicAcceptedPedestrianRouteOptions : public CGeoAcceptedPedestrianRouteOptions
	{
	public:
		~CGeoBasicAcceptedPedestrianRouteOptions() override = default;

		bool preview() const override { return true; }
		GeoParameter<bool> linked() const override { return { true, true }; }
		GeoParameter<std::set<GeoItineraryType::type_t>, GeoItineraryType::type_t> itineraries() const  override { return { std::set<GeoItineraryType::type_t>(), GeoItineraryType::Recommended }; }

		GeoParameter<bool> park() const override { return { false, false }; }
	};

	class CGeoRouteOptions : public stdx::cloneable<CGeoRouteOptions>
	{
	public:
		CLONEABLE(CGeoRouteOptions);

		CGeoRouteOptions();
		CGeoRouteOptions(const CGeoAcceptedRouteOptions& options);
		~CGeoRouteOptions() override = default;

		virtual GeoRouteOptionsType::type_t type() const { return GeoRouteOptionsType::Basic; }
		bool typeIs(GeoRouteOptionsType::type_t t) const { return (type() & t) == t; }

		bool isLinked() const { return m_bLinked; }
		void setLinked(bool b) { m_bLinked = b; }

		GeoItineraryType::type_t getItineraryType() const { return m_itiType; }
		void setItineraryType(GeoItineraryType::type_t itiType) { m_itiType = itiType; }

		virtual void merge(const CGeoRouteOptions& other);

		static std::unique_ptr<CGeoRouteOptions> getFromType(GeoRouteOptionsType::type_t optionsType);
		static std::unique_ptr<CGeoRouteOptions> getFromVehicleType(GeoVehicleType::type_t vehicleType);

	private:
		GeoItineraryType::type_t m_itiType;
		bool m_bLinked;
	};

	class CGeoRoadRouteOptions : public CGeoRouteOptions
	{
	public:
		CLONEABLE(CGeoRoadRouteOptions);

		CGeoRoadRouteOptions();
		CGeoRoadRouteOptions(const CGeoAcceptedRoadRouteOptions& options);
		~CGeoRoadRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Road; }

		bool takeHighway() const { return m_bHighway; }
		void setHighway(bool b) { m_bHighway = b; }

		bool takeTolls() const { return m_bTolls; }
		void setTolls(bool b) { m_bTolls = b; }

		bool takeBoatFerry() const { return m_bBoatFerry; }
		void setBoatFerry(bool b) { m_bBoatFerry = b; }

		bool takeRailFerry() const { return m_bRailFerry; }
		void setRailFerry(bool b) { m_bRailFerry = b; }

		bool takeTunnel() const { return m_bTunnel; }
		void setTunnel(bool b) { m_bTunnel = b; }

		bool takeDirtRoad() const { return m_bDirtRoad; }
		void setDirtRoad(bool b) { m_bDirtRoad = b; }

		void merge(const CGeoRouteOptions& other) override;

	private:
		bool m_bHighway;
		bool m_bTolls;
		bool m_bBoatFerry;
		bool m_bRailFerry;
		bool m_bTunnel;
		bool m_bDirtRoad;
	};

	class CGeoTruckRouteOptions : public CGeoRoadRouteOptions
	{
	public:
		CLONEABLE(CGeoTruckRouteOptions);

		CGeoTruckRouteOptions();
		CGeoTruckRouteOptions(const CGeoAcceptedTruckRouteOptions& options);
		~CGeoTruckRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Truck; }

		bool isTractor() const { return m_bTractor; }
		void setTractor(bool b) { m_bTractor = b; }

		size_t getTrailersCount() const { return m_trailersCount; }
		void setTrailersCount(size_t n) { m_trailersCount = n; }

		size_t getAxleCount() const { return m_axleCount; }
		void setAxleCount(size_t n) { m_axleCount = n; }

		GeoTruckCategoryType::type_t getCategory() const { return m_category; }
		void setCategory(GeoTruckCategoryType::type_t category) { m_category = category; }

		size_t getLimitedWeight() const { return m_limitedWeight; }
		void setLimitedWeight(size_t n) { m_limitedWeight = n; }

		size_t getWeightPerAxle() const { return m_weightPerAxle; }
		void setWeightPerAxle(size_t n) { m_weightPerAxle = n; }

		size_t getHeight() const { return m_height; }
		void setHeight(size_t n) { m_height = n; }

		size_t getWidth() const { return m_width; }
		void setWidth(size_t n) { m_width = n; }

		size_t getLength() const { return m_length; }
		void setLength(size_t n) { m_length = n; }

		virtual void merge(const CGeoRouteOptions& other) override;

	private:
		GeoTruckCategoryType::type_t m_category;
		bool m_bTractor;
		size_t m_trailersCount;
		size_t m_axleCount;
		size_t m_limitedWeight;
		size_t m_weightPerAxle;
		size_t m_height;
		size_t m_width;
		size_t m_length;
	};

	class CGeoThrillingRouteOptions : public CGeoRoadRouteOptions
	{
	public:
		CLONEABLE(CGeoThrillingRouteOptions);

		CGeoThrillingRouteOptions();
		CGeoThrillingRouteOptions(const CGeoAcceptedThrillingRouteOptions& options);
		~CGeoThrillingRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Thrilling; }

		bool takeAlreadyUsedRoads() const { return m_bAlreadyUsedRoads; }
		void setAlreadyUsedRoads(bool b) { m_bAlreadyUsedRoads = b; }

		CGeoAcceptedThrillingRouteOptions::degree_t getHilliness() const { return m_hilliness; }
		void setHilliness(CGeoAcceptedThrillingRouteOptions::degree_t degree) { m_hilliness = degree; }

		CGeoAcceptedThrillingRouteOptions::level_t getWindingness() const { return m_windingness; }
		void setWindingness(CGeoAcceptedThrillingRouteOptions::level_t level) { m_windingness = level; }

		void merge(const CGeoRouteOptions& other) override;

	private:
		bool m_bAlreadyUsedRoads;
		CGeoAcceptedThrillingRouteOptions::degree_t m_hilliness;
		CGeoAcceptedThrillingRouteOptions::level_t m_windingness;
	};

	class CGeoBicycleRouteOptions : public CGeoRouteOptions
	{
	public:
		CLONEABLE(CGeoBicycleRouteOptions);

		CGeoBicycleRouteOptions() = default;
		CGeoBicycleRouteOptions(const CGeoAcceptedBicycleRouteOptions& options) : CGeoRouteOptions(options) {}
		~CGeoBicycleRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Bike; }
	};

	class CGeoPedestrianRouteOptions : public CGeoRouteOptions
	{
	public:
		CLONEABLE(CGeoPedestrianRouteOptions);

		CGeoPedestrianRouteOptions();
		CGeoPedestrianRouteOptions(const CGeoAcceptedPedestrianRouteOptions& options);
		~CGeoPedestrianRouteOptions() override = default;

		GeoRouteOptionsType::type_t type() const override { return GeoRouteOptionsType::Pedestrian; }

		bool takePark() const { return m_bPark; }
		void setPark(bool b) { m_bPark = b; }

		void merge(const CGeoRouteOptions& other) override;

	private:
		bool m_bPark;
	};
} // namespace geo

#endif // _GEO_ROUTE_OPTIONS_H_INCLUDED_
