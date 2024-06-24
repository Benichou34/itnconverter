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
 * Purpose : Dispatch class for user settings
 */

#include "stdafx.h"
#include "SettingsDispatch.h"
#include "ITN Converter.h"
#include "Variant.h"

namespace
{
	enum
	{
		DISPID_AUTO_CALC = 1,
		DISPID_DIST_MILES,
		DISPID_AVOID_HIGHWAY,
		DISPID_AVOID_TOLLS,
		DISPID_TRAVEL_MODE,
		DISPID_ROUTE_PROVIDER,
		DISPID_FORCE_ROAD,
		DISPID_AUTO_ADD_STEP,
		DISPID_COORDS_FORMAT,
		DISPID_DISPLAY_NUMBER
	};

	const std::wstring c_strAutoCalc(L"AutoCalc");
	const std::wstring c_strDistMiles(L"DistMiles");
	const std::wstring c_strAvoidHighway(L"AvoidHighway");
	const std::wstring c_strAvoidTolls(L"AvoidTolls");
	const std::wstring c_strTravelMode(L"TravelMode");
	const std::wstring c_strRouteProvider(L"RouteProvider");
	const std::wstring c_strForceRoad(L"ForceRoad");
	const std::wstring c_strAutoAddStep(L"AutoAddStep");
	const std::wstring c_strCoordsFormat(L"CoordsFormat");
	const std::wstring c_strDisplayNumber(L"DisplayNumber");
}

HRESULT STDMETHODCALLTYPE CSettingsDispatch::GetIDsOfNames(
	/* [in] */ __RPC__in REFIID /*riid*/,
	/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
	/* [range][in] */ UINT cNames,
	/* [in] */ LCID /*lcid*/,
	/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
{
	if (!cNames)
		return DISP_E_UNKNOWNNAME;

	std::wstring strName(rgszNames[0]);

	if (strName == c_strAutoCalc)
		*rgDispId = DISPID_AUTO_CALC;
	else if (strName == c_strDistMiles)
		*rgDispId = DISPID_DIST_MILES;
	else if (strName == c_strAvoidHighway)
		*rgDispId = DISPID_AVOID_HIGHWAY;
	else if (strName == c_strAvoidTolls)
		*rgDispId = DISPID_AVOID_TOLLS;
	else if (strName == c_strTravelMode)
		*rgDispId = DISPID_TRAVEL_MODE;
	else if (strName == c_strRouteProvider)
		*rgDispId = DISPID_ROUTE_PROVIDER;
	else if (strName == c_strForceRoad)
		*rgDispId = DISPID_FORCE_ROAD;
	else if (strName == c_strAutoAddStep)
		*rgDispId = DISPID_AUTO_ADD_STEP;
	else if (strName == c_strCoordsFormat)
		*rgDispId = DISPID_COORDS_FORMAT;
	else if (strName == c_strDisplayNumber)
		*rgDispId = DISPID_DISPLAY_NUMBER;
	else
		return DISP_E_UNKNOWNNAME;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CSettingsDispatch::Invoke(
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID /*riid*/,
	/* [in] */ LCID /*lcid*/,
	/* [in] */ WORD /*wFlags*/,
	/* [out][in] */ DISPPARAMS* pDispParams,
	/* [out] */ VARIANT* pVarResult,
	/* [out] */ EXCEPINFO* /*pExcepInfo*/,
	/* [out] */ UINT* /*puArgErr*/)
{
	if (pDispParams->cArgs > 0 || !pVarResult)
		return DISP_E_BADPARAMCOUNT;

	switch (dispIdMember)
	{
	case DISPID_AUTO_CALC:
		*pVarResult = CVariant(CITNConverterApp::RegParam().AutoCalc()).variant();
		break;
	case DISPID_DIST_MILES:
		*pVarResult = CVariant(CITNConverterApp::RegParam().DistMiles()).variant();
		break;
	case DISPID_AVOID_HIGHWAY:
		*pVarResult = CVariant(CITNConverterApp::RegParam().AvoidHighway()).variant();
		break;
	case DISPID_AVOID_TOLLS:
		*pVarResult = CVariant(CITNConverterApp::RegParam().AvoidTolls()).variant();
		break;
	case DISPID_TRAVEL_MODE:
		*pVarResult = CVariant(CITNConverterApp::RegParam().VehicleType()).variant();
		break;
	case DISPID_ROUTE_PROVIDER:
		*pVarResult = CVariant(CITNConverterApp::RegParam().RouteProvider()).variant();
		break;
	case DISPID_FORCE_ROAD:
		*pVarResult = CVariant(CITNConverterApp::RegParam().ForceRoad()).variant();
		break;
	case DISPID_AUTO_ADD_STEP:
		*pVarResult = CVariant(CITNConverterApp::RegParam().AutoAddStep()).variant();
		break;
	case DISPID_COORDS_FORMAT:
		*pVarResult = CVariant(CITNConverterApp::RegParam().Coordinates()).variant();
		break;
	case DISPID_DISPLAY_NUMBER:
		*pVarResult = CVariant(CITNConverterApp::RegParam().DisplayNumber()).variant();
		break;

	default:
		return DISP_E_MEMBERNOTFOUND;
	}

	return S_OK;
}
