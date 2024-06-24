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

#include "stdafx.h"
#include "itn converter.h"
#include "RegParam.h"
#include "Storage/Profile.h"
#include "Storage/Registry.h"
#include "GpsPointArray.h"
#ifdef _MOBILE_
#include "ToolsLibrary/ToolsString.h"
#endif

namespace
{
	const std::wstring sConverterPosition(_T("ConverterPosition"));
	const std::wstring sEditorPosition(_T("EditorPosition"));
	const std::wstring sAutoCalc(_T("AUTO_CALC"));
	const std::wstring sDistMiles(_T("DIST_MILES"));
	const std::wstring sOptimizeUseDirection(_T("OPTIMIZE_USE_DIRECTION"));
	const std::wstring sAvoidHighway(_T("AVOID_HIGHWAY"));
	const std::wstring sAvoidTolls(_T("AVOID_TOLLS"));
	const std::wstring sVehicleType(_T("VehicleType"));
	const std::wstring sItineraryType(_T("ItineraryType"));
	const std::wstring sRouteProvider(_T("RouteProvider"));
	const std::wstring sForceRoad(_T("ForceRoad"));
	const std::wstring sAutoAddStep(_T("AutoAddStep"));
	const std::wstring sDisplayNumber(_T("DisplayNumber"));
	const std::wstring sExportFormat(_T("Export"));
	const std::wstring sCoordinates(_T("Coordinates"));
	const std::wstring sDefaultMap(_T("DefaultMap"));
	const std::wstring sLanguage(_T("Language"));
	const std::wstring sFavorites(_T("Favorites"));
	const std::wstring sMaps(_T("Maps"));
	const std::wstring sNumber(_T("Number"));
	const std::wstring sTruckHeight(_T("TruckHeight"));
	const std::wstring sTruckWidth(_T("TruckWidth"));
	const std::wstring sTruckLength(_T("TruckLength"));
	const std::wstring sTruckTrailersCount(_T("TruckTrailersCount"));
	const std::wstring sTruckLimitedWeight(_T("TruckLimitedWeight"));
	const std::wstring sTruckWeightAxle(_T("TruckWeightAxle"));
	const std::wstring sTruckCategory(_T("TruckCategory"));
	const std::wstring sUserId(_T("UserID"));

	std::string favoritesToUtf8(const std::string& ss)
	{
		try
		{
			// Check if is a valid UTF8 string
			stdx::wstring_helper::from_utf8(ss);
		}
		catch (std::exception&)
		{
			return stdx::string_helper::to_utf8(ss);
		}

		return ss;
	}
}

CRegParam::CRegParam()
{
}

CRegParam::~CRegParam()
{
	ToRegistry();
	m_ParamStorage.close();
}

void CRegParam::Init(const std::wstring& strStorageFolder)
{
	m_strFolder = strStorageFolder;

#ifdef _MOBILE_
	stdx::wstring_helper::vector vecString;
	CITNConverterApp::GetCommandLineArgv(vecString, true);

	m_ParamStorage = storage::Profile(CWToolsString::FileDirectory(vecString[0]) + CWToolsString::FileTitle(vecString[0]) + _T(".stg"));
#else
	m_ParamStorage = storage::Registry();
#endif

	FromRegistry();
}

const storage::SFolder CRegParam::getStorageFolder() const
{
	return m_ParamStorage(m_strFolder);
}

const storage::SFolder CRegParam::getStorageFolder(const std::wstring& strSubFolder) const
{
	return m_ParamStorage(m_strFolder)(strSubFolder);
}

storage::SFolder CRegParam::getStorageFolder()
{
	return m_ParamStorage(m_strFolder);
}

storage::SFolder CRegParam::getStorageFolder(const std::wstring& strSubFolder)
{
	return m_ParamStorage(m_strFolder)(strSubFolder);
}

void CRegParam::Flush()
{
	ToRegistry();
	m_ParamStorage.flush();
}

void CRegParam::ToRegistry()
{
	storage::SFolder StorageFolder = getStorageFolder();

	StorageFolder[sConverterPosition] = m_wdConverterPlacement;
	StorageFolder[sEditorPosition] = m_wdEditorPlacement;
	StorageFolder[sAutoCalc] = m_bAutoCalc;
	StorageFolder[sDistMiles] = m_bDistMiles;
	StorageFolder[sOptimizeUseDirection] = m_bOptUseDirection;
	StorageFolder[sAvoidHighway] = m_bAvoidHighway;
	StorageFolder[sAvoidTolls] = m_bAvoidTolls;
	StorageFolder[sVehicleType] = m_vehicleType;
	StorageFolder[sItineraryType] = m_itineraryType;
	StorageFolder[sRouteProvider] = m_eRouteProvider;
	StorageFolder[sForceRoad] = m_bForceRoad;
	StorageFolder[sAutoAddStep] = m_bAutoAddStep;
	StorageFolder[sDisplayNumber] = m_bDisplayNumber;
	StorageFolder[sExportFormat] = m_nExportId;
	StorageFolder[sCoordinates] = m_eCoordinates;
	StorageFolder[sTruckHeight] = m_strTruckHeight;
	StorageFolder[sTruckWidth] = m_strTruckWidth;
	StorageFolder[sTruckLength] = m_strTruckLength;
	StorageFolder[sTruckTrailersCount] = m_strTruckTrailersCount;
	StorageFolder[sTruckLimitedWeight] = m_strTruckLimitedWeight;
	StorageFolder[sTruckWeightAxle] = m_strTruckWeightAxle;
	StorageFolder[sTruckCategory] = m_strTruckCategory;
	StorageFolder[sUserId] = m_UserId;

	std::string strReg(reinterpret_cast<char*>(&m_curMap.second), sizeof(int32_t));
	strReg += m_curMap.first;
	StorageFolder[sDefaultMap].toBinary(strReg.data(), strReg.size());
}

void CRegParam::FromRegistry()
{
	const storage::SFolder StorageFolder = getStorageFolder();

	StorageFolder[sConverterPosition].asBinary(&m_wdConverterPlacement, sizeof(WINDOWPLACEMENT), 0x00);
	StorageFolder[sEditorPosition].asBinary(&m_wdEditorPlacement, sizeof(WINDOWPLACEMENT), 0x00);
	m_bAutoCalc = StorageFolder[sAutoCalc](false);
	m_bDistMiles = StorageFolder[sDistMiles](false);
	m_bOptUseDirection = StorageFolder[sOptimizeUseDirection](false);
	m_bAvoidHighway = StorageFolder[sAvoidHighway](false);
	m_bAvoidTolls = StorageFolder[sAvoidTolls](false);
	m_vehicleType = StorageFolder[sVehicleType](geo::GeoVehicleType::Default);
	m_itineraryType = StorageFolder[sItineraryType](geo::GeoItineraryType::Quickest);
	m_eRouteProvider = StorageFolder[sRouteProvider](geo::CGeoProviders::instance().getDefaultProvider());
	m_bForceRoad = StorageFolder[sForceRoad](true);
	m_bAutoAddStep = StorageFolder[sAutoAddStep](false);
	m_bDisplayNumber = StorageFolder[sDisplayNumber](true);
	m_nExportId = StorageFolder[sExportFormat](static_cast<int32_t>(-1));
	m_uiLanguage = StorageFolder[sLanguage](static_cast<int32_t>(0));
	m_eCoordinates = StorageFolder[sCoordinates](E_COORDINATES_DD);
	m_strTruckHeight = StorageFolder[sTruckHeight](20);
	m_strTruckWidth = StorageFolder[sTruckWidth](30);
	m_strTruckLength = StorageFolder[sTruckLength](80);
	m_strTruckTrailersCount = StorageFolder[sTruckTrailersCount](1);
	m_strTruckLimitedWeight = StorageFolder[sTruckLimitedWeight](30);
	m_strTruckWeightAxle = StorageFolder[sTruckWeightAxle](50);
	m_strTruckCategory = StorageFolder[sTruckCategory](geo::GeoTruckCategoryType::no_catory);
	m_UserId = StorageFolder[sUserId](std::wstring());

	std::vector<unsigned char> valueReg = StorageFolder[sDefaultMap].asBinary(std::vector<unsigned char>());
	if (valueReg.size() > sizeof(uint32_t))
	{
		m_curMap.second = *reinterpret_cast<const int32_t*>(valueReg.data());
		m_curMap.first = std::string(reinterpret_cast<char*>(valueReg.data()) + sizeof(uint32_t), valueReg.size() - sizeof(uint32_t));
	}

	// Check validity of route provider
	geo::CGeoDirections geoDirections(m_eRouteProvider, std::nothrow);
	if (!geoDirections)
	{
		m_eRouteProvider = geo::CGeoProviders::instance().getDefaultProvider();
		m_vehicleType = geo::GeoVehicleType::Default;
	}
	else if (!geoDirections->getSupportedTravelOptions().count(m_vehicleType))
	{
		m_vehicleType = geo::GeoVehicleType::Default;
	}
}

void CRegParam::ReadFavorites(CGpsPointArray& cGpsPointArray) const
{
	const storage::SFolder favFolder = getStorageFolder()(sFavorites);
	const float* pfCoords;

	size_t ulNumber = favFolder[sNumber](static_cast<uint32_t>(0));
	for (size_t i = 0; i < ulNumber; ++i)
	{
		std::vector<unsigned char> valueReg = favFolder[stdx::wstring_helper::to_string(i)].asBinary(std::vector<unsigned char>());
		if (valueReg.size() > sizeof(float) * 3)
		{
			CGpsPoint cGpsPoint;

			pfCoords = reinterpret_cast<const float*>(valueReg.data());

			cGpsPoint.coords(pfCoords[1], pfCoords[0], pfCoords[2]);
			std::string strName(reinterpret_cast<char*>(valueReg.data()) + sizeof(float) * 3, valueReg.size() - sizeof(float) * 3);
			cGpsPoint.name(favoritesToUtf8(strName));

			cGpsPointArray.push_back(cGpsPoint);
		}
	}

	cGpsPointArray.sortByAddress();
}

void CRegParam::WriteFavorites(const CGpsPointArray& cGpsPointArray)
{
	storage::SFolder favFolder = getStorageFolder()(sFavorites);

	favFolder[sNumber] = static_cast<uint32_t>(cGpsPointArray.size());
	for (size_t i = 0; i < cGpsPointArray.size(); ++i)
	{
		const CGpsPoint& cGpsPoint = cGpsPointArray[i];
		float afCoords[3];

		afCoords[0] = static_cast<float>(cGpsPoint.lng());
		afCoords[1] = static_cast<float>(cGpsPoint.lat());
		afCoords[2] = static_cast<float>(cGpsPoint.alt());

		std::string strReg(reinterpret_cast<char*>(&afCoords[0]), sizeof(afCoords));
		strReg += cGpsPoint.name();

		favFolder[stdx::wstring_helper::to_string(i)].toBinary(strReg.data(), strReg.size());
	}
}

void CRegParam::ReadMapsOptions(std::map<std::string, S_MAP_OPTION>& mapMapsOptions) const
{
	const storage::SFolder mapsFolder = getStorageFolder()(sMaps);

	size_t ulNumber = mapsFolder[sNumber](static_cast<uint32_t>(0));
	for (size_t i = 0; i < ulNumber; ++i)
	{
		std::vector<unsigned char> valueReg = mapsFolder[stdx::wstring_helper::to_string(i)].asBinary(std::vector<unsigned char>());
		if (valueReg.size() > sizeof(uint32_t))
		{
			uint32_t nOption = *reinterpret_cast<const uint32_t*>(valueReg.data());

			S_MAP_OPTION mapOption;
			mapOption.bMap = ((nOption & 0x00000001) == 0x00000001);
			mapOption.bSat = ((nOption & 0x00000002) == 0x00000002);
			mapOption.bHybrid = ((nOption & 0x00000004) == 0x00000004);
			mapOption.bTopo = ((nOption & 0x00000008) == 0x00000008);

			mapMapsOptions[std::string(reinterpret_cast<char*>(valueReg.data()) + sizeof(uint32_t), valueReg.size() - sizeof(uint32_t))] = mapOption;
		}
	}
}

void CRegParam::WriteMapsOptions(const std::map<std::string, S_MAP_OPTION>& mapMapsOptions)
{
	storage::SFolder mapsFolder = getStorageFolder()(sMaps);

	mapsFolder[sNumber] = static_cast<uint32_t>(mapMapsOptions.size());

	std::map<std::string, S_MAP_OPTION>::const_iterator cit;
	for (cit = mapMapsOptions.begin(); cit != mapMapsOptions.end(); ++cit)
	{
		uint32_t nOption = 0;
		const S_MAP_OPTION& mapOption = cit->second;

		if (mapOption.bMap)
			nOption |= 0x00000001;
		if (mapOption.bSat)
			nOption |= 0x00000002;
		if (mapOption.bHybrid)
			nOption |= 0x00000004;
		if (mapOption.bTopo)
			nOption |= 0x00000008;

		std::string strReg(reinterpret_cast<char*>(&nOption), sizeof(uint32_t));
		strReg += cit->first;

		mapsFolder[stdx::wstring_helper::to_string(std::distance(mapMapsOptions.begin(), cit))].toBinary(strReg.data(), strReg.size());
	}
}
