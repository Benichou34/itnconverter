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

template <typename TReg> 
CRegParam<TReg>::~CRegParam()
{
	ToRegistry();
	m_regKey.close();
}

template <typename TReg> 
void CRegParam<TReg>::Flush()
{
	ToRegistry(); 
	m_regKey.flush();
}

template <typename TReg> 
void CRegParam<TReg>::ToRegistry()
{
	m_regKey["ConverterPosition"] = m_wdConverterPlacement;
	m_regKey["EditorPosition"] = m_wdEditorPlacement;
	m_regKey["AUTO_CALC"] = m_bAutoCalc;
	m_regKey["DIST_MILES"] = m_bDistMiles;
	m_regKey["OPTIMIZE_USE_DIRECTION"] = m_bOptUseDirection;
	m_regKey["AVOID_HIGHWAY"] = m_bAvoidHighway;
	m_regKey["AVOID_TOLLS"] = m_bAvoidTolls;
	m_regKey["TravelMode"] = static_cast<int32_t>(m_eTravelMode);
	m_regKey["RouteProvider"] = static_cast<int32_t>(m_eRouteProvider);
	m_regKey["ForceRoad"] = m_bForceRoad;
	m_regKey["AutoAddStep"] = m_bAutoAddStep;
	m_regKey["Export"] = m_nExportId;
	m_regKey["Map"] = m_strMap;
	m_regKey["sat"] = m_strSat;
	m_regKey["topo"] = m_strTopo;
}

template <typename TReg> 
void CRegParam<TReg>::FromRegistry()
{
	m_regKey["ConverterPosition"].asBinary(&m_wdConverterPlacement, sizeof(WINDOWPLACEMENT), 0x00);
	m_regKey["EditorPosition"].asBinary(&m_wdEditorPlacement, sizeof(WINDOWPLACEMENT), 0x00);
	m_bAutoCalc = m_regKey["AUTO_CALC"](false);
	m_bDistMiles = m_regKey["DIST_MILES"](false);
	m_bOptUseDirection = m_regKey["OPTIMIZE_USE_DIRECTION"](false);
	m_bAvoidHighway = m_regKey["AVOID_HIGHWAY"](false);
	m_bAvoidTolls = m_regKey["AVOID_TOLLS"](false);
	m_eTravelMode = static_cast<geo::E_GEO_TRAVEL_MODE>(m_regKey["TravelMode"](static_cast<int32_t>(geo::E_GEO_TRAVEL_DEFAULT)));
	m_eRouteProvider = static_cast<geo::E_GEO_PROVIDER>(m_regKey["RouteProvider"](static_cast<int32_t>(geo::E_GEO_PROVIDER_GOOGLE_MAP)));
	m_bForceRoad = m_regKey["ForceRoad"](true);
	m_bAutoAddStep = m_regKey["AutoAddStep"](false);
	m_nExportId = m_regKey["Export"](static_cast<int32_t>(-1));
	m_strMap = m_regKey["Map"]("GM");
	m_strSat = m_regKey["sat"]("GM");
	m_strTopo = m_regKey["topo"]("GM");
	m_uiLanguage =	m_regKey["Language"](static_cast<int32_t>(0));
	
	// Check validity of route provider
	try
	{
		geo::CGeoDirections geoDirections(m_eRouteProvider);
		if(!geoDirections().getSupportedTravelOptions().count(m_eTravelMode))
			m_eTravelMode = geo::E_GEO_TRAVEL_DEFAULT;
	}
	catch(std::invalid_argument&)
	{
		m_eRouteProvider = geo::E_GEO_PROVIDER_GOOGLE_MAP;
		m_eTravelMode = geo::E_GEO_TRAVEL_DEFAULT;
	}
}

template <typename TReg> 
void CRegParam<TReg>::ReadFavorites(CGpsPointArray& cGpsPointArray)
{
	const float* pfCoords;
	std::string strReg;
	size_t i;
	TReg cRegFavorites("Favorites", m_regKey);

	size_t ulNumber = cRegFavorites["Number"](static_cast<uint32_t>(0));
	for(i=0;i<ulNumber;i++)
	{
		strReg = cRegFavorites[CToolsString::ToString(i)].asBinary(std::string());
		if(strReg.size())
		{
			CGpsPoint cGpsPoint;
		
			pfCoords = reinterpret_cast<const float*>(strReg.data());

			cGpsPoint.lng() = pfCoords[0];
			cGpsPoint.lat() = pfCoords[1];
			cGpsPoint.alt() = pfCoords[2];
			cGpsPoint.name() = strReg.substr(sizeof(float)*3);

			cGpsPointArray.push_back(cGpsPoint);
		}
	}

	cGpsPointArray.sortByAddress();
}

template <typename TReg> 
void CRegParam<TReg>::WriteFavorites(const CGpsPointArray& cGpsPointArray)
{
	float afCoords[3];
	size_t i;
	TReg cRegFavorites("Favorites", m_regKey);

	cRegFavorites["Number"] = static_cast<uint32_t>(cGpsPointArray.size());
	for(i=0;i<cGpsPointArray.size();i++)
	{
		const CGpsPoint& cGpsPoint = cGpsPointArray[i];
	
		afCoords[0] = static_cast<float>(cGpsPoint.lng());
		afCoords[1] = static_cast<float>(cGpsPoint.lat());
		afCoords[2] = static_cast<float>(cGpsPoint.alt());

		std::string strReg(reinterpret_cast<char*>(&afCoords[0]), sizeof(afCoords));
		strReg += cGpsPoint.name();
		
		cRegFavorites[CToolsString::ToString(i)].toBinary(strReg);
	}
}
