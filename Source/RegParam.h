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
 *
 * Purpose : Read/Write parameters from/to registry
 */

#ifndef _REGPARAM_H_INCLUDED_
#define _REGPARAM_H_INCLUDED_

#include "GeoServices/GeoServices.h"
#include "GeoServices/GeoLatLngDegrees.h"
#include "Storage/Storage.h"

class CGpsPointArray;

class CRegParam
{
public:
	typedef enum
	{
		E_COORDINATES_DD,
		E_COORDINATES_DM,
		E_COORDINATES_DMS,
		E_COORDINATES_UTM,

		E_COORDINATES_NB
	} E_COORDINATES;

	typedef struct s_map_option
	{
		bool bMap;
		bool bSat;
		bool bHybrid;
		bool bTopo;

		s_map_option() : bMap(true), bSat(true), bHybrid(true), bTopo(true) {}
	} S_MAP_OPTION;

	CRegParam();
	virtual ~CRegParam();

	void Init(const std::wstring& strFolder);

	const storage::SFolder getStorageFolder() const;
	const storage::SFolder getStorageFolder(const std::wstring& strSubFolder) const;
	storage::SFolder getStorageFolder();
	storage::SFolder getStorageFolder(const std::wstring& strSubFolder);

	void Flush();

	void ReadFavorites(CGpsPointArray& cGpsPointArray) const;
	void WriteFavorites(const CGpsPointArray& cGpsPointArray);

	void ReadMapsOptions(std::map<std::string, S_MAP_OPTION>& mapMapsOptions) const;
	void WriteMapsOptions(const std::map<std::string, S_MAP_OPTION>& mapMapsOptions);

	const std::pair<std::string, int>& getCurrentMap() const { return m_curMap; }
	void setCurrentMap(const std::pair<std::string, int>& curMap) { m_curMap = curMap; }

	bool AutoCalc() const { return m_bAutoCalc; }
	bool& AutoCalc() { return m_bAutoCalc; }

	bool DistMiles() const { return m_bDistMiles; }
	bool& DistMiles() { return m_bDistMiles; }

	bool OptUseDirection() const { return m_bOptUseDirection; }
	bool& OptUseDirection() { return m_bOptUseDirection; }

	bool AvoidHighway() const { return m_bAvoidHighway; }
	bool& AvoidHighway() { return m_bAvoidHighway; }

	bool AvoidTolls() const { return m_bAvoidTolls; }
	bool& AvoidTolls() { return m_bAvoidTolls; }

	geo::GeoVehicleType::type_t VehicleType() const { return m_vehicleType; }
	geo::GeoVehicleType::type_t& VehicleType() { return m_vehicleType; }

	geo::GeoItineraryType::type_t ItineraryType() const { return m_itineraryType; }
	geo::GeoItineraryType::type_t& ItineraryType() { return m_itineraryType; }

	geo::E_GEO_PROVIDER RouteProvider() const { return m_eRouteProvider; }
	geo::E_GEO_PROVIDER& RouteProvider() { return m_eRouteProvider; }

	bool ForceRoad() const { return m_bForceRoad; }
	bool& ForceRoad() { return m_bForceRoad; }

	bool AutoAddStep() const { return m_bAutoAddStep; }
	bool& AutoAddStep() { return m_bAutoAddStep; }

	bool DisplayNumber() const { return m_bDisplayNumber; }
	bool& DisplayNumber() { return m_bDisplayNumber; }

	int ExportId() const { return m_nExportId; }
	int& ExportId() { return m_nExportId; }

	unsigned int Language() const { return m_uiLanguage; }

	const WINDOWPLACEMENT& ConverterPlacement() const { return m_wdConverterPlacement; }
	WINDOWPLACEMENT& ConverterPlacement() { return m_wdConverterPlacement; }

	const WINDOWPLACEMENT& EditorPlacement() const { return m_wdEditorPlacement; }
	WINDOWPLACEMENT& EditorPlacement() { return m_wdEditorPlacement; }

	E_COORDINATES Coordinates() const { return m_eCoordinates; }
	E_COORDINATES& Coordinates() { return m_eCoordinates; }

	unsigned int TruckHeight() const { return m_strTruckHeight; }
	unsigned int& TruckHeight() { return m_strTruckHeight; }

	unsigned int TruckWidth() const { return m_strTruckWidth; }
	unsigned int& TruckWidth() { return m_strTruckWidth; }

	unsigned int TruckLength() const { return m_strTruckLength; }
	unsigned int& TruckLength() { return m_strTruckLength; }

	unsigned int TruckTrailersCount() const { return m_strTruckTrailersCount; }
	unsigned int& TruckTrailersCount() { return m_strTruckTrailersCount; }

	unsigned int TruckLimitedWeight() const { return m_strTruckLimitedWeight; }
	unsigned int& TruckLimitedWeight() { return m_strTruckLimitedWeight; }

	unsigned int TruckWeightAxle() const { return m_strTruckWeightAxle; }
	unsigned int& TruckWeightAxle() { return m_strTruckWeightAxle; }

	geo::GeoTruckCategoryType::type_t TruckCategory() const { return m_strTruckCategory; }
	geo::GeoTruckCategoryType::type_t& TruckCategory() { return m_strTruckCategory; }

	const std::wstring& UserId() const { return m_UserId; }
	std::wstring& UserId() { return m_UserId; }

private:
	// Serialize
	void ToRegistry();
	void FromRegistry();

private:
	storage::Storage m_ParamStorage;
	std::wstring m_strFolder;
	bool m_bAutoCalc;
	bool m_bDistMiles;
	bool m_bOptUseDirection;
	bool m_bAvoidHighway;
	bool m_bAvoidTolls;
	bool m_bForceRoad;
	bool m_bAutoAddStep;
	bool m_bDisplayNumber;
	geo::GeoVehicleType::type_t m_vehicleType;
	geo::GeoItineraryType::type_t m_itineraryType;
	geo::E_GEO_PROVIDER m_eRouteProvider;
	int m_nExportId;
	unsigned int m_uiLanguage;
	WINDOWPLACEMENT m_wdConverterPlacement;
	WINDOWPLACEMENT m_wdEditorPlacement;
	E_COORDINATES m_eCoordinates;
	std::pair<std::string, int> m_curMap;
	unsigned int m_strTruckHeight;
	unsigned int m_strTruckWidth;
	unsigned int m_strTruckLength;
	unsigned int m_strTruckTrailersCount;
	unsigned int m_strTruckLimitedWeight;
	unsigned int m_strTruckWeightAxle;
	geo::GeoTruckCategoryType::type_t m_strTruckCategory;
	std::wstring m_UserId;
};

#endif // _REGPARAM_H_INCLUDED_
