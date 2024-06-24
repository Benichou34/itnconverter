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
#include "stdx/format.h"
#include "stdx/string_helper.h"
#include "GeoServices/GeoServices.h"
#include "ToolsLibrary/HttpClient.h"
#include "NavRouteView.h"
#include "travel.h"

CNavRouteView::CNavRouteView() : CNavPointView()
{
	m_pInfoLabel = nullptr;
	m_pDistanceLabel = nullptr;
	m_bTempAutoCalc = false;
	m_bRefreshAll = false;
}

CNavRouteView::~CNavRouteView()
{
}

void CNavRouteView::SetControls(CStatic* pDistanceLabel, CStatic* pInfoLabel)
{
	m_pDistanceLabel = pDistanceLabel;
	m_pInfoLabel = pInfoLabel;
}

bool CNavRouteView::OnInsert(int nIndex)
{
	CNavPointView::OnInsert(nIndex);
	if (m_bRefreshAll)
	{
		m_cProgressDlg.StepIt();
		return !m_cProgressDlg.DoEvents();
	}

	return true;
}

void CNavRouteView::Refresh()
{
	m_bRefreshAll = true;
	m_cProgressDlg.Display(CWToolsString::Load(IDS_DOWNLOADING).c_str(), true, this);
	m_cProgressDlg.SetRange(0, m_pcGpsPointArray->size());
	m_cProgressDlg.SetMessage(CWToolsString::Load(IDS_PLEASEWAIT).c_str());

	CNavPointView::Refresh();

	m_bRefreshAll = false;
	m_cProgressDlg.Close();

	RefreshDriving();
}

void CNavRouteView::Clear()
{
	ClearDriving();
	CNavPointView::Clear();
	RefreshDriving();
}

void CNavRouteView::Up()
{
	CNavPointView::Up();
	RefreshDriving();
}

void CNavRouteView::Down()
{
	CNavPointView::Down();
	RefreshDriving();
}

void CNavRouteView::Add(const CGpsPoint& cGpsPoint)
{
	CNavPointView::Add(cGpsPoint);
	RefreshDriving();
}

void CNavRouteView::Insert(int nIndex, const CGpsPoint& cGpsPoint)
{
	CNavPointView::Insert(nIndex, cGpsPoint);
	RefreshDriving();
}

void CNavRouteView::Remove(int nIndex)
{
	CNavPointView::Remove(nIndex);
	RefreshDriving();
}

void CNavRouteView::Invert()
{
	ClearDriving();
	CNavPointView::Invert();
	RefreshDriving();
}

void CNavRouteView::Deplace(int nIndex, const geo::CGeoLatLng& cgLatLng)
{
	CNavPointView::Deplace(nIndex, cgLatLng);
	RefreshDriving();
}

void CNavRouteView::DisableAutoCalc()
{
	m_bTempAutoCalc = false;
}

void CNavRouteView::ClearDriving()
{
	CGpsPointArray::iterator it;

	for (it = m_pcGpsPointArray->begin(); it != m_pcGpsPointArray->end(); ++it)
		it->clearRouteInfo();

	if (m_pDistanceLabel)
		m_pDistanceLabel->SetWindowText(_T(""));

	if (m_pNavigator)
	{
		m_pNavigator->JavaScript_CloseInfoWindow();
		m_pNavigator->JavaScript_ClearRoutes(m_nTabIndex);
	}
}

void CNavRouteView::RefreshDriving()
{
	if (m_bTempAutoCalc)
		CalculateAll(false);
	else
		UpdateSummary();
}

int CNavRouteView::AddIntermediate(int nSrcIndex, int nDstIndex)
{
	CGpsPoint& cSrcGpsPoint = m_pcGpsPointArray->at(nSrcIndex);
	CGpsPoint& cDstGpsPoint = m_pcGpsPointArray->at(nDstIndex);

	// Display progress
	std::wstring strProgress = stdx::wformat(CWToolsString::Load(IDS_INTERMEDIATE_PROGRESS))(stdx::wstring_helper::from_utf8(cSrcGpsPoint.name()))(stdx::wstring_helper::from_utf8(cDstGpsPoint.name()));
	m_cProgressDlg.SetMessage(strProgress.c_str());
	if (m_pInfoLabel)
		m_pInfoLabel->SetWindowText(strProgress.c_str());

	std::unique_ptr<geo::CGeoRouteOptions> cgOptions;
	geo::GeoVehicleType::type_t vehicleType = FillRouteOptionsWithSettings(cgOptions);

	geo::CGeoDirections gDirections(geo::E_GEO_PROVIDER_GOOGLE_API);
	geo::E_GEO_STATUS_CODE eStatusCode = geo::E_GEO_UNKNOWN_ERROR;

	int nRet;
	do
	{
		nRet = IDOK;

		gDirections->Load(cSrcGpsPoint, cDstGpsPoint, vehicleType, *cgOptions);
		eStatusCode = gDirections->getStatus();

		if (eStatusCode > geo::E_HTTP_ERROR)
			nRet = HTTPErrorBox(eStatusCode - geo::E_HTTP_ERROR);
	} while (nRet == IDRETRY);

	if (nRet != IDOK)
		return S_FALSE;

	if (eStatusCode == geo::E_GEO_OK)
	{
		geo::CGeoRvsGeocoder gGeocoder(geo::CGeoProviders::instance().getDefaultProvider());

		const geo::CGeoRoute& gRoute = gDirections->getRoutes().front();
		size_t i = 0;
		geo::CGeoRoute::const_iterator it;

		for (it = gRoute.begin(); it != gRoute.end(); ++it)
		{
			CGpsPoint gpsPoint(*it);

			if (gpsPoint != cSrcGpsPoint && gpsPoint != cDstGpsPoint)
			{
				gGeocoder->Load(gpsPoint);
				if (gGeocoder->getStatus() == geo::E_GEO_OK && !gGeocoder->getResults().empty())
					gpsPoint.name(gGeocoder->getResults().front().name());

				m_pcGpsPointArray->insert(nDstIndex + i++, gpsPoint);
			}
		}

		if (m_pInfoLabel)
		{
			// Display end of calculation
			strProgress = stdx::wformat(CWToolsString::Load(IDS_INTERMEDIATE_SUCESS))(stdx::wstring_helper::from_utf8(cSrcGpsPoint.name()))(stdx::wstring_helper::from_utf8(cDstGpsPoint.name()));
			m_pInfoLabel->SetWindowText(strProgress.c_str());
		}
	}
	else if (m_pInfoLabel)
	{
		cDstGpsPoint.clearRouteInfo();

		// Display routing error
		strProgress = stdx::wformat(CWToolsString::Load(IDS_ROUTING_ERROR))(stdx::wstring_helper::from_utf8(cSrcGpsPoint.name()))(stdx::wstring_helper::from_utf8(cDstGpsPoint.name()));

		if (MessageBox(strProgress.c_str(), CWToolsString::Load(IDS_TOOLTIP_CALCULATE).c_str(), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
			return S_FALSE;
		m_pInfoLabel->SetWindowText(strProgress.c_str());
	}

	return S_OK;
}

int CNavRouteView::DrivingInstruction(int nSrcIndex, int nDstIndex)
{
	CGpsPoint& cSrcGpsPoint = m_pcGpsPointArray->at(nSrcIndex);
	CGpsPoint& cDstGpsPoint = m_pcGpsPointArray->at(nDstIndex);

	if (!cDstGpsPoint.routeInfo())
	{
		// Display progress
		std::wstring strProgress = stdx::wformat(CWToolsString::Load(IDS_ROUTING_PROGRESS))(stdx::wstring_helper::from_utf8(cSrcGpsPoint.name()))(stdx::wstring_helper::from_utf8(cDstGpsPoint.name()));
		m_cProgressDlg.SetMessage(strProgress.c_str());
		if (m_pInfoLabel)
			m_pInfoLabel->SetWindowText(strProgress.c_str());

		std::unique_ptr<geo::CGeoRouteOptions> cgOptions;
		geo::GeoVehicleType::type_t vehicleType = FillRouteOptionsWithSettings(cgOptions);

		geo::E_GEO_PROVIDER eGeoProvider = CITNConverterApp::RegParam().RouteProvider();
		geo::CGeoDirections gDirections(eGeoProvider);
		geo::E_GEO_STATUS_CODE eStatusCode = geo::E_GEO_UNKNOWN_ERROR;

		int nRet;
		do
		{
			nRet = IDOK;

			gDirections->Load(cSrcGpsPoint, cDstGpsPoint, vehicleType, *cgOptions);
			eStatusCode = gDirections->getStatus();
			if (eStatusCode > geo::E_HTTP_ERROR)
				nRet = HTTPErrorBox(eStatusCode - geo::E_HTTP_ERROR);
		} while (nRet == IDRETRY);

		if (nRet != IDOK)
			return S_FALSE;

		if (eStatusCode == geo::E_GEO_OK)
		{
			cDstGpsPoint.setRouteInfo(gDirections->getRoutes().front());
			if (m_pNavigator)
				m_pNavigator->JavaScript_AddRoute(m_nTabIndex, nDstIndex);

			if (m_pInfoLabel)
			{
				// Display end of calculation
				strProgress = stdx::wformat(CWToolsString::Load(IDS_ROUTING_SUCESS))(stdx::wstring_helper::from_utf8(cSrcGpsPoint.name()))(stdx::wstring_helper::from_utf8(cDstGpsPoint.name()));
				m_pInfoLabel->SetWindowText(strProgress.c_str());
			}
		}
		else if (m_pInfoLabel)
		{
			cDstGpsPoint.clearRouteInfo();

			// Display routing error
			strProgress = stdx::wformat(CWToolsString::Load(IDS_ROUTING_ERROR))(stdx::wstring_helper::from_utf8(cSrcGpsPoint.name()))(stdx::wstring_helper::from_utf8(cDstGpsPoint.name()));

			if (MessageBox(strProgress.c_str(), CWToolsString::Load(IDS_TOOLTIP_CALCULATE).c_str(), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDCANCEL)
				return S_FALSE;
			m_pInfoLabel->SetWindowText(strProgress.c_str());
		}
	}

	return S_OK;
}

int CNavRouteView::UpdateSummary()
{
	if (!m_pNavigator)
		return S_FALSE;

	size_t i;
	bool bBreak = false;
	size_t ulCumulativeDistance = 0;
	size_t ulCumulativeDuration = 0;

	for (i = 1; i < m_pcGpsPointArray->size(); i++)
	{
		CRouteInfo* pcRouteInfo = m_pcGpsPointArray->at(i).routeInfo();

		if (pcRouteInfo)
		{
			if (!pcRouteInfo->summary().isValid())
				bBreak = true;

			if (!bBreak)
			{
				ulCumulativeDistance += pcRouteInfo->summary().distance();
				ulCumulativeDuration += pcRouteInfo->summary().duration();

				pcRouteInfo->cumulativeDistance() = ulCumulativeDistance;
				pcRouteInfo->cumulativeDuration() = ulCumulativeDuration;
			}
			else
			{
				pcRouteInfo->cumulativeDistance() = 0;
				pcRouteInfo->cumulativeDuration() = 0;
			}
		}
		else
		{
			bBreak = true;
		}
	}

	if (m_pDistanceLabel)
	{
		std::wstring strSummary;

		if (!bBreak)
		{
			strSummary = CWToolsString::Load(IDS_SUMMARY) + GetStringDistance(ulCumulativeDistance, CITNConverterApp::RegParam().DistMiles());
			if (ulCumulativeDuration)
				strSummary += stdx::wformat(_T(" (%s)"))(GetStringDuration(ulCumulativeDuration)).str();
		}

		m_pDistanceLabel->SetWindowText(strSummary.c_str());
	}

	return S_OK;
}

void CNavRouteView::CalculateSegment(int nIndex)
{
	HRESULT hr = S_OK;

	if (nIndex == -1)
		return CalculateAll();

	hr = DrivingInstruction(nIndex - 1, nIndex);
	if (hr == S_OK)
	{
		UpdateSummary();
		SelectItem(nIndex);
	}
}

geo::CGeoLatLngs CNavRouteView::GetRoutePreview(int nIndex, const geo::CGeoLatLng& gLatLng, bool bStep)
{
	geo::CGeoLatLngs cgLatLngs;

	if (m_pcGpsPointArray->size() < 2 || nIndex < 0)
		return cgLatLngs;

	if (bStep)
	{
		CGpsPoint& gpsPoint = m_pcGpsPointArray->at((nIndex > 0) ? nIndex : 1);

		if (!m_bTempAutoCalc || !gpsPoint.routeInfo() || (gpsPoint.routeInfo() && !gpsPoint.routeInfo()->summary().isValid()))
			return cgLatLngs;

		if (nIndex > 0)
			cgLatLngs.push_back(m_pcGpsPointArray->at(nIndex - 1));

		cgLatLngs.push_back(gLatLng);
		if (static_cast<size_t>(nIndex) < m_pcGpsPointArray->upper_bound())
			cgLatLngs.push_back(m_pcGpsPointArray->at(nIndex + 1));
	}
	else
	{
		if (nIndex > 0)
			cgLatLngs.push_back(m_pcGpsPointArray->at(nIndex - 1));
		cgLatLngs.push_back(gLatLng);
		cgLatLngs.push_back(m_pcGpsPointArray->at(nIndex));
	}

	return cgLatLngs;
}

void CNavRouteView::AddIntermediateAll(bool bUseProgressDlg)
{
	int i = 0;
	int nWayPointNumber = m_pcGpsPointArray->upper_bound();
	HRESULT hr = S_OK;

	if (m_pNavigator)
		m_pNavigator->JavaScript_CloseInfoWindow();

	if (bUseProgressDlg)
	{
		std::wstring strProgress = stdx::wformat(CWToolsString::Load(IDS_INTERMEDIATE_PROGRESS))(stdx::wstring_helper::from_utf8(m_pcGpsPointArray->front().name()))(stdx::wstring_helper::from_utf8(m_pcGpsPointArray->back().name()));

		m_cProgressDlg.Display(strProgress.c_str(), true, this);
		m_cProgressDlg.SetRange(0, nWayPointNumber);
	}

	while (i < nWayPointNumber)
	{
		hr = AddIntermediate(i, i + 1);
		if (hr != S_OK)
			break;

		i += m_pcGpsPointArray->size() - nWayPointNumber;
		nWayPointNumber = m_pcGpsPointArray->upper_bound();

		if (bUseProgressDlg)
		{
			m_cProgressDlg.StepIt();
			if (m_cProgressDlg.DoEvents())
				break;
		}
	}

	if (bUseProgressDlg)
		m_cProgressDlg.Close();

	if (m_pInfoLabel && hr == S_OK)
	{
		// Display end of calculation
		std::wstring strProgress = stdx::wformat(CWToolsString::Load(IDS_INTERMEDIATE_SUCESS))(stdx::wstring_helper::from_utf8(m_pcGpsPointArray->front().name()))(stdx::wstring_helper::from_utf8(m_pcGpsPointArray->back().name()));
		m_pInfoLabel->SetWindowText(strProgress.c_str());
	}

	Refresh();
}

void CNavRouteView::CalculateAll(bool bUseProgressDlg)
{
	int nWayPointNumber = m_pcGpsPointArray->upper_bound();
	HRESULT hr = S_OK;

	m_bTempAutoCalc = CITNConverterApp::RegParam().AutoCalc();
	if (m_pcGpsPointArray->empty())
		return;

	if (m_pNavigator)
		m_pNavigator->JavaScript_CloseInfoWindow();

	if (bUseProgressDlg)
	{
		std::wstring strProgress = stdx::wformat(CWToolsString::Load(IDS_ROUTING_PROGRESS))(stdx::wstring_helper::from_utf8(m_pcGpsPointArray->front().name()))(stdx::wstring_helper::from_utf8(m_pcGpsPointArray->back().name()));

		m_cProgressDlg.Display(strProgress.c_str(), true, this);
		m_cProgressDlg.SetRange(0, nWayPointNumber);

		switch (CITNConverterApp::RegParam().RouteProvider())
		{
#ifdef IDB_GOOGLE
		case geo::E_GEO_PROVIDER_GOOGLE_API:
			m_cProgressDlg.SetIcon(IDB_GOOGLE);
			break;
#endif
#ifdef IDB_BING
		case geo::E_GEO_PROVIDER_BING_API:
			m_cProgressDlg.SetIcon(IDB_BING);
			break;
#endif
#ifdef IDB_VIAMICHELIN
		case geo::E_GEO_PROVIDER_VIAMICHELIN_API:
			m_cProgressDlg.SetIcon(IDB_VIAMICHELIN);
			break;
#endif
#ifdef IDB_OSM
		case geo::E_GEO_PROVIDER_OSRM_API:
			m_cProgressDlg.SetIcon(IDB_OSM);
			break;
#endif
#ifdef IDB_WAZE
		case geo::E_GEO_PROVIDER_WAZE_MAP:
		case geo::E_GEO_PROVIDER_WAZE_WORLD_MAP:
			m_cProgressDlg.SetIcon(IDB_WAZE);
			break;
#endif
#ifdef IDB_HERE
		case geo::E_GEO_PROVIDER_HERE_API:
			m_cProgressDlg.SetIcon(IDB_HERE);
			break;
#endif
#ifdef IDB_TOMTOM
		case geo::E_GEO_PROVIDER_TOMTOM_API:
			m_cProgressDlg.SetIcon(IDB_TOMTOM);
			break;
#endif
#ifdef IDB_GEOPORTAL
		case geo::E_GEO_PROVIDER_GEOPORTAL_API:
			m_cProgressDlg.SetIcon(IDB_GEOPORTAL);
			break;
#endif
		default:
			break;
		}
	}

	for (int i = 0; i < nWayPointNumber; i++)
	{
		hr = DrivingInstruction(i, i + 1);
		if (hr != S_OK)
			break;

		if (bUseProgressDlg)
		{
			m_cProgressDlg.StepIt();
			if (m_cProgressDlg.DoEvents())
				break;
		}
	}

	if (hr == S_OK)
		UpdateSummary();

	if (bUseProgressDlg)
		m_cProgressDlg.Close();

	if (m_pInfoLabel && nWayPointNumber && hr == S_OK)
	{
		// Display end of calculation
		std::wstring strProgress = stdx::wformat(CWToolsString::Load(IDS_ROUTING_SUCESS))(stdx::wstring_helper::from_utf8(m_pcGpsPointArray->front().name()))(stdx::wstring_helper::from_utf8(m_pcGpsPointArray->back().name()));
		m_pInfoLabel->SetWindowText(strProgress.c_str());
	}
}

void CNavRouteView::Optimize()
{
	ClearDriving();
	CTravel(*m_pcGpsPointArray).Optimize(CITNConverterApp::RegParam().OptUseDirection());

	OnRefresh(0, m_pcGpsPointArray->size());
	SelectItem(-1);
	RefreshDriving();
}

void CNavRouteView::OnEndDrop(int nDragIndex, int nDropIndex)
{
	CNavPointView::OnEndDrop(nDragIndex, nDropIndex);
	RefreshDriving();
}
