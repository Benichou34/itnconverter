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
 * Purpose : Dispatch class for a GPSpoint
 */

#include "stdafx.h"
#include <string>
#include "ITN Tools.h"
#include "PointDispatch.h"
#include "RouteDispatch.h"
#include "GpsPoint.h"
#include "Variant.h"

namespace
{
	enum
	{
		DISPID_LATITUDE = 1,
		DISPID_LONGITUDE,
		DISPID_ALTITUDE,
		DISPID_ADDRESS,
		DISPID_SNIPPET,
		DISPID_ROUTE,
		DISPID_TYPE
	};

	const std::wstring c_strLat(L"lat");
	const std::wstring c_strLng(L"lng");
	const std::wstring c_strAlt(L"alt");
	const std::wstring c_strAddress(L"address");
	const std::wstring c_strSnippet(L"snippet");
	const std::wstring c_strRoute(L"route");
	const std::wstring c_strType(L"type");
}

CPointDispatch::CPointDispatch(CGpsPoint& cGpsPoint, int nIcon) :
	m_pcGpsPoint(&cGpsPoint),
	m_nIcon(nIcon),
	m_bDelete(false)
{
}

CPointDispatch::CPointDispatch(CGpsPoint* pcGpsPoint, int nIcon, bool bDelete) :
	m_pcGpsPoint(pcGpsPoint),
	m_nIcon(nIcon),
	m_bDelete(bDelete)
{
}

CPointDispatch::~CPointDispatch()
{
	if (m_bDelete)
		delete m_pcGpsPoint;
}

HRESULT STDMETHODCALLTYPE CPointDispatch::GetIDsOfNames(
	/* [in] */ __RPC__in REFIID /*riid*/,
	/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
	/* [range][in] */ UINT cNames,
	/* [in] */ LCID /*lcid*/,
	/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
{
	if (!m_pcGpsPoint)
		return DISP_E_BADINDEX;

	if (!cNames)
		return DISP_E_UNKNOWNNAME;

	std::wstring strName(rgszNames[0]);

	if (strName == c_strLat)
		*rgDispId = DISPID_LATITUDE;
	else if (strName == c_strLng)
		*rgDispId = DISPID_LONGITUDE;
	else if (strName == c_strAlt)
		*rgDispId = DISPID_ALTITUDE;
	else if (strName == c_strAddress)
		*rgDispId = DISPID_ADDRESS;
	else if (strName == c_strSnippet)
		*rgDispId = DISPID_SNIPPET;
	else if (strName == c_strRoute && m_pcGpsPoint->routeInfo())
		*rgDispId = DISPID_ROUTE;
	else if (strName == c_strType)
		*rgDispId = DISPID_TYPE;
	else
		return DISP_E_UNKNOWNNAME;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CPointDispatch::Invoke(
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID /*riid*/,
	/* [in] */ LCID /*lcid*/,
	/* [in] */ WORD /*wFlags*/,
	/* [out][in] */ DISPPARAMS* /*pDispParams*/,
	/* [out] */ VARIANT* pVarResult,
	/* [out] */ EXCEPINFO* /*pExcepInfo*/,
	/* [out] */ UINT* /*puArgErr*/)
{
	if (!m_pcGpsPoint)
		return DISP_E_BADINDEX;

	if (!pVarResult)
		return DISP_E_BADPARAMCOUNT;

	switch (dispIdMember)
	{
	case DISPID_LATITUDE:
		*pVarResult = CVariant(m_pcGpsPoint->lat()).variant();
		break;

	case DISPID_LONGITUDE:
		*pVarResult = CVariant(m_pcGpsPoint->lng()).variant();
		break;

	case DISPID_ALTITUDE:
		*pVarResult = CVariant(m_pcGpsPoint->alt()).variant();
		break;

	case DISPID_ADDRESS:
		*pVarResult = CVariant(m_pcGpsPoint->name()).variant();
		break;

	case DISPID_SNIPPET:
		*pVarResult = CVariant(m_pcGpsPoint->comment()).variant();
		break;

	case DISPID_ROUTE:
		*pVarResult = CVariant(new CRouteDispatch(m_pcGpsPoint->routeInfo())).variant();
		break;

	case DISPID_TYPE:
		*pVarResult = CVariant(m_nIcon).variant();
		break;

	default:
		return DISP_E_MEMBERNOTFOUND;
	}

	return S_OK;
}
