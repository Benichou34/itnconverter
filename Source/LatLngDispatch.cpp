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
 * Purpose : Dispatch class for a CGeoLatLng
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <string>
#include "LatLngDispatch.h"
#include "GeoServices/GeoLatLng.h"
#include "Variant.h"

namespace
{
	enum
	{
		DISPID_LATITUDE = 1,
		DISPID_LONGITUDE,
		DISPID_ALTITUDE
	};

	const std::wstring c_strLat(L"lat");
	const std::wstring c_strLng(L"lng");
	const std::wstring c_strAlt(L"alt");
}

CLatLngDispatch::CLatLngDispatch(const geo::CGeoLatLng& geoLatLng) :
	m_pGeoLatLng(&geoLatLng),
	m_bDelete(false)
{
}

CLatLngDispatch::CLatLngDispatch(const geo::CGeoLatLng* pGeoLatLng, bool bDelete) :
	m_pGeoLatLng(pGeoLatLng),
	m_bDelete(bDelete)
{
}

CLatLngDispatch::~CLatLngDispatch()
{
	if (m_bDelete)
		delete m_pGeoLatLng;
}

HRESULT STDMETHODCALLTYPE CLatLngDispatch::GetIDsOfNames(
	/* [in] */ __RPC__in REFIID /*riid*/,
	/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
	/* [range][in] */ UINT cNames,
	/* [in] */ LCID /*lcid*/,
	/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
{
	if (!m_pGeoLatLng)
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
	else
		return DISP_E_UNKNOWNNAME;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CLatLngDispatch::Invoke(
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID /*riid*/,
	/* [in] */ LCID /*lcid*/,
	/* [in] */ WORD /*wFlags*/,
	/* [out][in] */ DISPPARAMS* /*pDispParams*/,
	/* [out] */ VARIANT* pVarResult,
	/* [out] */ EXCEPINFO* /*pExcepInfo*/,
	/* [out] */ UINT* /*puArgErr*/)
{
	if (!m_pGeoLatLng)
		return DISP_E_BADINDEX;

	if (!pVarResult)
		return DISP_E_BADPARAMCOUNT;

	switch (dispIdMember)
	{
	case DISPID_LATITUDE:
		*pVarResult = CVariant(m_pGeoLatLng->lat()).variant();
		break;

	case DISPID_LONGITUDE:
		*pVarResult = CVariant(m_pGeoLatLng->lng()).variant();
		break;

	case DISPID_ALTITUDE:
		*pVarResult = CVariant(m_pGeoLatLng->alt()).variant();
		break;

	default:
		return DISP_E_MEMBERNOTFOUND;
	}

	return S_OK;
}
