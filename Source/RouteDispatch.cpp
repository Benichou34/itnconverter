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
 * Purpose : Dispatch class for a GPS route
 */

#include "stdafx.h"
#include <string>
#include "ITN Tools.h"
#include "RouteDispatch.h"
#include "PointDispatch.h"
#include "LatLngDispatch.h"
#include "ArrayDispatch.h"
#include "GpsPoint.h"
#include "Variant.h"

namespace
{
	enum
	{
		DISPID_DISTANCE = 1,
		DISPID_DURATION,
		DISPID_AVOID_HIGHWAY,
		DISPID_AVOID_TOLLS,
		DISPID_TRAVEL_MODE,
		DISPID_ROUTE_PROVIDER,
		DISPID_POINTS,
		DISPID_COLOR
	};

	const std::wstring c_strDistance(L"distance");
	const std::wstring c_strDuration(L"duration");
	const std::wstring c_strHighway(L"avoid_highway");
	const std::wstring c_strTolls(L"avoid_tolls");
	const std::wstring c_strDriving(L"driving");
	const std::wstring c_strProvider(L"provider");
	const std::wstring c_strPoints(L"points");
	const std::wstring c_strColor(L"color");
}

CRouteDispatch::CRouteDispatch(CRouteInfo* pcRouteInfo, bool bDelete) :
	m_pcRouteInfo(pcRouteInfo),
	m_bDelete(bDelete)
{
}

CRouteDispatch::~CRouteDispatch()
{
	if (m_bDelete)
	{
		delete m_pcRouteInfo;
		m_pcRouteInfo = nullptr;
	}
}

HRESULT STDMETHODCALLTYPE CRouteDispatch::GetIDsOfNames(
	/* [in] */ __RPC__in REFIID /*riid*/,
	/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
	/* [range][in] */ UINT cNames,
	/* [in] */ LCID /*lcid*/,
	/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
{
	if (!m_pcRouteInfo)
		return DISP_E_BADINDEX;

	if (!cNames)
		return DISP_E_UNKNOWNNAME;

	std::wstring strName(rgszNames[0]);

	if (strName == c_strDistance)
		*rgDispId = DISPID_DISTANCE;
	else if (strName == c_strDuration)
		*rgDispId = DISPID_DURATION;
	else if (strName == c_strHighway)
		*rgDispId = DISPID_AVOID_HIGHWAY;
	else if (strName == c_strTolls)
		*rgDispId = DISPID_AVOID_TOLLS;
	else if (strName == c_strDriving)
		*rgDispId = DISPID_TRAVEL_MODE;
	else if (strName == c_strProvider)
		*rgDispId = DISPID_ROUTE_PROVIDER;
	else if (strName == c_strPoints)
		*rgDispId = DISPID_POINTS;
	else if (strName == c_strColor && m_pcRouteInfo->polyline().getColor() != 0xFFFFFF)
		*rgDispId = DISPID_COLOR;
	else
		return DISP_E_UNKNOWNNAME;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRouteDispatch::Invoke(
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID /*riid*/,
	/* [in] */ LCID /*lcid*/,
	/* [in] */ WORD /*wFlags*/,
	/* [out][in] */ DISPPARAMS* /*pDispParams*/,
	/* [out] */ VARIANT* pVarResult,
	/* [out] */ EXCEPINFO* /*pExcepInfo*/,
	/* [out] */ UINT* /*puArgErr*/)
{
	if (!m_pcRouteInfo)
		return DISP_E_BADINDEX;

	if (!pVarResult)
		return DISP_E_BADPARAMCOUNT;

	switch (dispIdMember)
	{
	case DISPID_DISTANCE:
	{
		std::wstring strDistance;
		size_t ulDistance = m_pcRouteInfo->summary().distance();
		size_t ulCumulativeDistance = m_pcRouteInfo->cumulativeDistance();

		if (ulDistance)
		{
			strDistance = GetStringDistance(ulDistance, CITNConverterApp::RegParam().DistMiles());
			if (!ulCumulativeDistance)
				strDistance += _T(" / ---");
			else if (ulCumulativeDistance > ulDistance)
				strDistance += _T(" / ") + GetStringDistance(ulCumulativeDistance, CITNConverterApp::RegParam().DistMiles());
		}

		*pVarResult = CVariant(strDistance).variant();
	}
	break;

	case DISPID_DURATION:
	{
		std::wstring strDuration;
		size_t ulDuration = m_pcRouteInfo->summary().duration();
		size_t ulCumulativeDuration = m_pcRouteInfo->cumulativeDuration();

		if (ulDuration)
		{
			strDuration = GetStringDuration(ulDuration);
			if (!ulCumulativeDuration)
				strDuration += _T(" / --:--");
			else if (ulCumulativeDuration > ulDuration)
				strDuration += _T(" / ") + GetStringDuration(ulCumulativeDuration);
		}

		*pVarResult = CVariant(strDuration).variant();
	}
	break;

	case DISPID_AVOID_HIGHWAY:
		*pVarResult = CVariant(m_pcRouteInfo->route().options().typeIs(geo::GeoRouteOptionsType::Road) && !static_cast<const geo::CGeoRoadRouteOptions&>(m_pcRouteInfo->route().options()).takeHighway()).variant();
		break;

	case DISPID_AVOID_TOLLS:
		*pVarResult = CVariant(m_pcRouteInfo->route().options().typeIs(geo::GeoRouteOptionsType::Road) && !static_cast<const geo::CGeoRoadRouteOptions&>(m_pcRouteInfo->route().options()).takeTolls()).variant();
		break;

	case DISPID_TRAVEL_MODE:
		*pVarResult = CVariant(m_pcRouteInfo->route().vehicle()).variant();
		break;

	case DISPID_ROUTE_PROVIDER:
		*pVarResult = CVariant(CITNConverterApp::RegParam().RouteProvider()).variant();
		break;

	case DISPID_POINTS:
		*pVarResult = CVariant(new CArrayDispatch<geo::CGeoLatLngs, CLatLngDispatch>(m_pcRouteInfo->polyline().getPath())).variant();
		break;

	case DISPID_COLOR:
		*pVarResult = CVariant(stdx::wformat(_T("#%06X"))(m_pcRouteInfo->polyline().getColor()).str()).variant();
		break;

	default:
		return DISP_E_MEMBERNOTFOUND;
	}

	return S_OK;
}
