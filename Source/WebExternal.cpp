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
 * Purpose : Main Dispatch class for javascript
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "ToolsLibrary/Raii.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "GeoServices/GoogleStreetView.h"
#include "GeoServices/GoogleApiElevation.h"
#include "sendtogps.h"
#include "WebExternal.h"
#include "Javascript.h"
#include "Navigator.h"
#include "HttpRequest.h"
#include "NavRouteView.h"
#include "PointDispatch.h"
#include "SettingsDispatch.h"
#include "Base64Dispatch.h"
#include "Utf8Dispatch.h"
#include "RouteDispatch.h"
#include "AsyncRouteCalculation.h"

IMPLEMENT_OLETYPELIB(CWebExternal, GUID_NULL, 1, 0);

namespace
{
	enum
	{
		DISPID_TRACE = 1,
		DISPID_MESSAGE_BOX,
		DISPID_HTTP_REQUEST,
		DISPID_HTTP_DOWNLOAD,
		DISPID_GET_POINT,
		DISPID_SELECT_POINT,
		DISPID_DELETE_POINT,
		DISPID_RENAME_POINT,
		DISPID_MOVE_POINT,
		DISPID_INSERT_POINT,
		DISPID_ADD_POINT,
		DISPID_ADD_FAVORITE,
		DISPID_CALCULATE,
		DISPID_SEND_TO_GPS,
		DISPID_GET_LOCATION,
		DISPID_GET_ROAD_LOCATION,
		DISPID_GET_STREETVIEW_LOCATION,
		DISPID_RVS_GEOCODING,
		DISPID_GET_ROUTE_PREVIEW,
		DISPID_GET_ELEVATION,
		DISPID_GET_SETTINGS,
		DISPID_BASE64,
		DISPID_UTF8,
		DISPID_GET_STR_COORDS,
		DISPID_SET_CURRENT_MAP
	};

	const std::wstring c_strTrace(L"Trace");
	const std::wstring c_strMessageBox(L"MessageBox");
	const std::wstring c_strHttpRequest(L"HttpRequest");
	const std::wstring c_strHttpDownload(L"HttpDownload");
	const std::wstring c_strGetPoint(L"GetPoint");
	const std::wstring c_strSelectPoint(L"SelectPoint");
	const std::wstring c_strDeletePoint(L"DeletePoint");
	const std::wstring c_strRenamePoint(L"RenamePoint");
	const std::wstring c_strMovePoint(L"MovePoint");
	const std::wstring c_strInsertPoint(L"InsertPoint");
	const std::wstring c_strAddPoint(L"AddPoint");
	const std::wstring c_strAddFavorite(L"AddFavorite");
	const std::wstring c_strCalculate(L"Calculate");
	const std::wstring c_strSendToGps(L"SendToGps");
	const std::wstring c_strGetLocation(L"GetLocation");
	const std::wstring c_strGetRoadLocation(L"GetRoadLocation");
	const std::wstring c_strGetStreetViewLocation(L"GetStreetViewLocation");
	const std::wstring c_strReverseGeocoding(L"ReverseGeocoding");
	const std::wstring c_strGetRoutePreview(L"GetRoutePreview");
	const std::wstring c_strGetElevation(L"GetElevation");
	const std::wstring c_strSettings(L"Settings");
	const std::wstring c_strBase64(L"Base64");
	const std::wstring c_strUtf8(L"Utf8");
	const std::wstring c_strGetStrCoords(L"GetStrCoords");
	const std::wstring c_strSetCurrentMap(L"SetCurrentMap");
}

/////////////////////////////////////////////////////////////////////////////
// CWebExternal

CWebExternal::CWebExternal(CNavigator& navigator) :
	m_Navigator(navigator),
	m_AsyncRouteCalculation(new CAsyncRouteCalculation)
{
	EnableTypeLib();
}

CWebExternal::~CWebExternal()
{
}

void CWebExternal::AddView(CNavPointView& cNavPointView, int nTabIndex)
{
	m_mapNavPointView[nTabIndex] = &cNavPointView;
}

HRESULT STDMETHODCALLTYPE CWebExternal::GetIDsOfNames(
	/* [in] */ __RPC__in REFIID /*riid*/,
	/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
	/* [range][in] */ UINT cNames,
	/* [in] */ LCID /*lcid*/,
	/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
{
	if (!cNames)
		return DISP_E_UNKNOWNNAME;

	std::wstring strName(rgszNames[0]);

	if (strName == c_strTrace)
		*rgDispId = DISPID_TRACE;
	else if (strName == c_strMessageBox)
		*rgDispId = DISPID_MESSAGE_BOX;
	else if (strName == c_strHttpRequest)
		*rgDispId = DISPID_HTTP_REQUEST;
	else if (strName == c_strHttpDownload)
		*rgDispId = DISPID_HTTP_DOWNLOAD;
	else if (strName == c_strGetPoint)
		*rgDispId = DISPID_GET_POINT;
	else if (strName == c_strSelectPoint)
		*rgDispId = DISPID_SELECT_POINT;
	else if (strName == c_strDeletePoint)
		*rgDispId = DISPID_DELETE_POINT;
	else if (strName == c_strRenamePoint)
		*rgDispId = DISPID_RENAME_POINT;
	else if (strName == c_strMovePoint)
		*rgDispId = DISPID_MOVE_POINT;
	else if (strName == c_strInsertPoint)
		*rgDispId = DISPID_INSERT_POINT;
	else if (strName == c_strAddPoint)
		*rgDispId = DISPID_ADD_POINT;
	else if (strName == c_strAddFavorite)
		*rgDispId = DISPID_ADD_FAVORITE;
	else if (strName == c_strCalculate)
		*rgDispId = DISPID_CALCULATE;
	else if (strName == c_strSendToGps)
		*rgDispId = DISPID_SEND_TO_GPS;
	else if (strName == c_strGetLocation)
		*rgDispId = DISPID_GET_LOCATION;
	else if (strName == c_strGetRoadLocation)
		*rgDispId = DISPID_GET_ROAD_LOCATION;
	else if (strName == c_strGetStreetViewLocation)
		*rgDispId = DISPID_GET_STREETVIEW_LOCATION;
	else if (strName == c_strReverseGeocoding)
		*rgDispId = DISPID_RVS_GEOCODING;
	else if (strName == c_strGetRoutePreview)
		*rgDispId = DISPID_GET_ROUTE_PREVIEW;
	else if (strName == c_strGetElevation)
		*rgDispId = DISPID_GET_ELEVATION;
	else if (strName == c_strSettings)
		*rgDispId = DISPID_GET_SETTINGS;
	else if (strName == c_strBase64)
		*rgDispId = DISPID_BASE64;
	else if (strName == c_strUtf8)
		*rgDispId = DISPID_UTF8;
	else if (strName == c_strGetStrCoords)
		*rgDispId = DISPID_GET_STR_COORDS;
	else if (strName == c_strSetCurrentMap)
		*rgDispId = DISPID_SET_CURRENT_MAP;
	else
		return DISP_E_UNKNOWNNAME;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CWebExternal::Invoke(
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID /*riid*/,
	/* [in] */ LCID /*lcid*/,
	/* [in] */ WORD /*wFlags*/,
	/* [out][in] */ DISPPARAMS* pDispParams,
	/* [out] */ VARIANT* pVarResult,
	/* [out] */ EXCEPINFO* /*pExcepInfo*/,
	/* [out] */ UINT* /*puArgErr*/)
{
	try
	{
		switch (dispIdMember)
		{
		case DISPID_TRACE:
			Trace(pDispParams->rgvarg[0], pVarResult);
			break;
		case DISPID_MESSAGE_BOX:
			MessageBox(pDispParams->rgvarg[0], pVarResult);
			break;
		case DISPID_HTTP_REQUEST:
			HttpRequest(pDispParams, pVarResult);
			break;
		case DISPID_HTTP_DOWNLOAD:
			HttpDownload(pDispParams, pVarResult);
			break;
		case DISPID_GET_POINT:
			GetPoint(pDispParams, pVarResult);
			break;
		case DISPID_SELECT_POINT:
			SelectPoint(pDispParams, pVarResult);
			break;
		case DISPID_DELETE_POINT:
			DeletePoint(pDispParams, pVarResult);
			break;
		case DISPID_RENAME_POINT:
			RenamePoint(pDispParams, pVarResult);
			break;
		case DISPID_MOVE_POINT:
			MovePoint(pDispParams, pVarResult);
			break;
		case DISPID_INSERT_POINT:
			InsertPoint(pDispParams, pVarResult);
			break;
		case DISPID_ADD_POINT:
			AddPoint(pDispParams, pVarResult);
			break;
		case DISPID_ADD_FAVORITE:
			AddFavorite(pDispParams, pVarResult);
			break;
		case DISPID_CALCULATE:
			Calculate(pDispParams, pVarResult);
			break;
		case DISPID_SEND_TO_GPS:
			SendToGps(pDispParams, pVarResult);
			break;
		case DISPID_GET_LOCATION:
			GetLocation(pDispParams, pVarResult);
			break;
		case DISPID_GET_ROAD_LOCATION:
			GetRoadLocation(pDispParams, pVarResult);
			break;
		case DISPID_GET_STREETVIEW_LOCATION:
			GetStreetViewLocation(pDispParams, pVarResult);
			break;
		case DISPID_RVS_GEOCODING:
			ReverseGeocoding(pDispParams, pVarResult);
			break;
		case DISPID_GET_ROUTE_PREVIEW:
			GetRoutePreview(pDispParams, pVarResult);
			break;
		case DISPID_GET_ELEVATION:
			GetElevation(pDispParams, pVarResult);
			break;
		case DISPID_GET_SETTINGS:
			GetSettings(pDispParams, pVarResult);
			break;
		case DISPID_BASE64:
			UseBase64(pDispParams, pVarResult);
			break;
		case DISPID_UTF8:
			UseUtf8(pDispParams, pVarResult);
			break;
		case DISPID_GET_STR_COORDS:
			GetStrCoords(pDispParams, pVarResult);
			break;
		case DISPID_SET_CURRENT_MAP:
			SetCurrentMap(pDispParams, pVarResult);
			break;
		default:
			throw CWinApiException(DISP_E_MEMBERNOTFOUND);
		}
	}
	catch (CWinApiException& winEx)
	{
		return winEx.code();
	}
	catch (DWORD& dw)
	{
		return HRESULT_FROM_WIN32(dw);
	}

	return S_OK;
}

void CWebExternal::Trace(VARIANT& LOG, VARIANT* /*retval*/)
{
	CVariant vLog(LOG);
	if (vLog.type() == VT_BSTR)
		CITNConverterApp::wlog() << clock() << L": " << vLog.asWString() << std::endl;
}

void CWebExternal::MessageBox(VARIANT& MSG, VARIANT* /*retval*/)
{
	CVariant vMsg(MSG);
	if (vMsg.type() == VT_BSTR && !vMsg.asWString().empty())
		AfxMessageBox(vMsg.asWString().c_str());
}

void CWebExternal::HttpRequest(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (pDispParams->cArgs < 1)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	CVariantList args(*pDispParams);

	if (args.front().type() != VT_BSTR)
		throw CWinApiException(DISP_E_BADVARTYPE);

	std::string strRequest = args.front().asString();
	std::string strReferrer;
	std::string strHost;
	size_t queue = 0;
	CAutoVariant vCallBack;

	if (args.size() > 1 && args.back().type() == VT_DISPATCH)
	{
		autoDispatch<IDispatch> dispatch(args.back().asDispatch());
		CJavaScript jsDispatch(dispatch.get());

		if (jsDispatch.property(L"referrer"))
			NO_THROW(strReferrer = jsDispatch.execute().asString());

		if (jsDispatch.property(L"host"))
			NO_THROW(strHost = jsDispatch.execute().asString());

		if (jsDispatch.property(L"callback"))
			NO_THROW(vCallBack = jsDispatch.execute());

		if (jsDispatch.property(L"queue"))
			NO_THROW(queue = jsDispatch.execute().asUInteger());
	}

	try
	{
		if (vCallBack.type() == VT_DISPATCH)
		{
			auto it = m_AsyncHttpRequests.find(queue);
			if (it == m_AsyncHttpRequests.end())
				it = m_AsyncHttpRequests.insert(std::make_pair(queue, std::make_unique<CAsyncHttpRequest>(m_Navigator))).first;

			it->second->PostRequest(strRequest, strReferrer, strHost, vCallBack.asDispatch());
		}
		else if (retval)
		{
			*retval = CVariant(CInternet::AjaxHttpRequest(strRequest, strReferrer, strHost)).variant();
		}
	}
	catch (CInternetException&)
	{
		throw CWinApiException(DISP_E_EXCEPTION);
	}
}

void CWebExternal::HttpDownload(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (pDispParams->cArgs < 1 || !retval)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	CVariantList args(*pDispParams);

	if (args.front().type() != VT_BSTR)
		throw CWinApiException(DISP_E_BADVARTYPE);

	std::string strRequest = args.front().asString();

	std::string strReferrer;
	if (args.size() > 2 && args[1].type() == VT_BSTR)
		strReferrer = args[1].asString();

	try
	{
		*retval = CVariant(CInternet::HttpDownload(strRequest, strReferrer)).variant();
	}
	catch (CInternetException&)
	{
		throw CWinApiException(DISP_E_EXCEPTION);
	}
}

geo::CGeoLocation CWebExternal::GetLocation(const geo::CGeoLatLng& cgLatLng)
{
	geo::E_GEO_PROVIDER eGeoProvider = CITNConverterApp::RegParam().RouteProvider();
	if (geo::E_GEO_PROVIDER_INTERNAL)
		eGeoProvider = geo::CGeoProviders::instance().getDefaultProvider();

	try
	{
		geo::CGeoRvsGeocoder gGeocoder(eGeoProvider);
		gGeocoder->Load(cgLatLng);
		if (gGeocoder->getStatus() == geo::E_GEO_OK && gGeocoder->getResults().size())
			return gGeocoder->getResults().front();
	}
	catch (std::invalid_argument&)
	{
	}

	return geo::CGeoLocation();
}

geo::CGeoLocation CWebExternal::GetRoadLocation(const geo::CGeoLatLng& cgLatLng)
{
	geo::E_GEO_PROVIDER eGeoProvider = CITNConverterApp::RegParam().RouteProvider();
	if (geo::E_GEO_PROVIDER_INTERNAL)
		eGeoProvider = geo::CGeoProviders::instance().getDefaultProvider();

	try
	{
		geo::CGeoDirections gDirections(eGeoProvider);
		geo::CGeoLatLng cgDestLatLng(cgLatLng.lat() + 0.01, cgLatLng.lng() + 0.01);

		gDirections->Load(cgLatLng, cgDestLatLng);
		if (gDirections->getStatus() == geo::E_GEO_OK && gDirections->getRoutes().front().locations().front())
			return gDirections->getRoutes().front().locations().front();
	}
	catch (std::invalid_argument&)
	{
	}

	return geo::CGeoLocation();
}

geo::CGeoLocation CWebExternal::GetStreetViewLocation(const geo::CGeoLatLng& cgLatLng)
{
	geo::CGoogleStreetView geoStreetView(cgLatLng);

	if (geoStreetView.getStatus() == geo::E_GEO_OK)
		return geoStreetView.getResults().front();
	else
		return geo::CGeoLocation();
}

geo::CGeoLatLng CWebExternal::GetElevation(const geo::CGeoLatLng& cgLatLng)
{
	geo::CGoogleApiElevation geoGoogleApiElevation(cgLatLng);

	if (geoGoogleApiElevation.getStatus() == geo::E_GEO_OK)
		return geoGoogleApiElevation.getResults().front();
	else
		return cgLatLng;
}

CNavPointView* CWebExternal::RetrieveView(DISPPARAMS* pDispParams, int& nIndex)
{
	if (pDispParams->cArgs < 2)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	if (pDispParams->rgvarg[pDispParams->cArgs - 1].vt != VT_I4 || pDispParams->rgvarg[pDispParams->cArgs - 2].vt != VT_I4)
		throw CWinApiException(DISP_E_BADVARTYPE);

	int nTab = pDispParams->rgvarg[pDispParams->cArgs - 1].intVal;
	nIndex = pDispParams->rgvarg[pDispParams->cArgs - 2].intVal;

	if (nTab == -1 || nIndex == -1)
		throw CWinApiException(DISP_E_BADINDEX);

	if (!m_mapNavPointView.count(nTab))
		throw CWinApiException(DISP_E_BADINDEX);

	return m_mapNavPointView[nTab];
}

double CWebExternal::GetDoubleFromVariant(const CVariant& variant)
{
	double dResult = 0;

	switch (variant.type())
	{
	case VT_R4:
		dResult = variant.asFloat();
		break;
	case VT_R8:
		dResult = variant.asDouble();
		break;
	case VT_I2:
		dResult = variant.asShort();
		break;
	case VT_I4:
		dResult = variant.asLong();
		break;
	case VT_INT:
		dResult = variant.asInteger();
		break;

	default:
		throw CWinApiException(DISP_E_BADVARTYPE);
	}

	return dResult;
}

geo::CGeoLatLng CWebExternal::GetLatLngFromVariant(const CVariant& variant)
{
	if (variant.type() != VT_DISPATCH)
		throw CWinApiException(DISP_E_BADVARTYPE);

	autoDispatch<IDispatch> dispatch(variant.asDispatch());

	return geo::CGeoLatLng(
		GetDoubleFromVariant(CJavaScript(dispatch.get()).method(L"lat").execute()),
		GetDoubleFromVariant(CJavaScript(dispatch.get()).method(L"lng").execute()));
}

geo::CGeoLocation CWebExternal::GetLocationFromVariant(const CVariant& varLatLng, const CVariant& varAddress)
{
	if (varLatLng.type() != VT_DISPATCH || varAddress.type() != VT_BSTR)
		throw CWinApiException(DISP_E_BADVARTYPE);

	return geo::CGeoLocation(GetLatLngFromVariant(varLatLng), varAddress.asString());
}

void CWebExternal::GetPoint(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (!retval)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	int nIndex;
	CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);
	*retval = CVariant(new CPointDispatch(pNavPointView->GpsPointArray().at(nIndex), pNavPointView->GetType(nIndex))).variant();
}

void CWebExternal::SelectPoint(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	int nIndex;
	CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);
	pNavPointView->SelectItem(nIndex);
}

void CWebExternal::DeletePoint(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	int nIndex;
	CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);
	pNavPointView->Remove(nIndex);
}

void CWebExternal::RenamePoint(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	if (pDispParams->cArgs < 3)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	if (pDispParams->rgvarg[0].vt != VT_BSTR)
		throw CWinApiException(DISP_E_BADVARTYPE);

	int nIndex;
	CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);
	pNavPointView->Rename(nIndex, CVariant(pDispParams->rgvarg[0]).asString());
}

void CWebExternal::MovePoint(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	if (pDispParams->cArgs < 3)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	geo::CGeoLatLng gLatLng(GetLatLngFromVariant(pDispParams->rgvarg[0]));

	int nIndex;
	CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);
	pNavPointView->Deplace(nIndex, gLatLng);
}

void CWebExternal::InsertPoint(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	if (pDispParams->cArgs < 4)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	int nIndex;
	CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);
	pNavPointView->Insert(nIndex, GetLocationFromVariant(pDispParams->rgvarg[1], pDispParams->rgvarg[0]));
}

void CWebExternal::AddPointToView(int nViewIndex, DISPPARAMS* pDispParams)
{
	if (pDispParams->cArgs < 2)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	if (pDispParams->rgvarg[1].vt == VT_DISPATCH)
	{
		m_mapNavPointView[nViewIndex]->Add(GetLocationFromVariant(pDispParams->rgvarg[1], pDispParams->rgvarg[0]));
	}
	else
	{
		int nIndex;
		CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);

		m_mapNavPointView[nViewIndex]->Add(pNavPointView->GpsPointArray().at(nIndex));

		if (pNavPointView->GetType(-1) != CNavPointView::E_VIEW_TYPE_ROADBOOK && pNavPointView->GetType(-1) != CNavPointView::E_VIEW_TYPE_FAVORITE)
			pNavPointView->Remove(nIndex);
	}
}

void CWebExternal::AddPoint(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	AddPointToView(0, pDispParams);
}

void CWebExternal::AddFavorite(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	std::map<int, CNavPointView*>::iterator it;

	for (it = m_mapNavPointView.begin(); it != m_mapNavPointView.end(); ++it)
	{
		if (it->second->GetType(-1) == CNavPointView::E_VIEW_TYPE_FAVORITE)
			break;
	}

	if (it == m_mapNavPointView.end())
		throw CWinApiException(DISP_E_EXCEPTION);

	AddPointToView(it->first, pDispParams);
}

void CWebExternal::Calculate(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	int nIndex;
	CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);
	if (pNavPointView->GetType(-1) == CNavPointView::E_VIEW_TYPE_ROADBOOK)
		static_cast<CNavRouteView*>(pNavPointView)->CalculateSegment(nIndex);
}

void CWebExternal::SendToGps(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	if (pDispParams->cArgs < 3)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	try
	{
		int nIndex;
		CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);

		if (pDispParams->rgvarg[0].vt != VT_I4)
			throw CWinApiException(DISP_E_BADVARTYPE);

		CSendToGps::SendTo(static_cast<CSendToGps::E_GPS>(pDispParams->rgvarg[0].intVal), pNavPointView->GpsPointArray().at(nIndex));
	}
	catch (std::exception&)
	{
		if (pDispParams->rgvarg[2].vt != VT_I4)
			throw CWinApiException(DISP_E_BADVARTYPE);

		CSendToGps::SendTo(static_cast<CSendToGps::E_GPS>(pDispParams->rgvarg[2].intVal), GetLocationFromVariant(pDispParams->rgvarg[1], pDispParams->rgvarg[0]));
	}
}

void CWebExternal::GetLocation(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (!retval || pDispParams->cArgs < 1)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	*retval = CVariant(new CPointDispatch(new CGpsPoint(GetLocation(GetLatLngFromVariant(pDispParams->rgvarg[0]))), CNavPointView::E_VIEW_TYPE_MARKER)).variant();
}

void CWebExternal::GetRoadLocation(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (!retval || pDispParams->cArgs < 1)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	*retval = CVariant(new CPointDispatch(new CGpsPoint(GetRoadLocation(GetLatLngFromVariant(pDispParams->rgvarg[0]))), CNavPointView::E_VIEW_TYPE_MARKER)).variant();
}

void CWebExternal::GetStreetViewLocation(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (!retval || pDispParams->cArgs < 1)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	*retval = CVariant(new CPointDispatch(new CGpsPoint(GetStreetViewLocation(GetLatLngFromVariant(pDispParams->rgvarg[0]))), CNavPointView::E_VIEW_TYPE_MARKER)).variant();
}

void CWebExternal::ReverseGeocoding(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (!retval || pDispParams->cArgs < 1)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	geo::CGeoLatLng cgLatLng(GetLatLngFromVariant(pDispParams->rgvarg[0]));
	geo::CGeoLocation cgLocation;

	if (CITNConverterApp::RegParam().RouteProvider() != geo::E_GEO_PROVIDER_INTERNAL && CITNConverterApp::RegParam().ForceRoad())
		cgLocation = GetRoadLocation(cgLatLng);

	if (!cgLocation)
		cgLocation = cgLatLng;

	if (cgLocation.name().empty())
		cgLocation.name(GetLocation(cgLocation).name());

	*retval = CVariant(new CPointDispatch(new CGpsPoint(cgLocation), CNavPointView::E_VIEW_TYPE_MARKER)).variant();
}

void CWebExternal::GetRoutePreview(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (pDispParams->cArgs < 4)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	CVariantList args(*pDispParams);
	CAutoVariant vCallBack;

	if (args[3].type() != VT_BOOL)
		throw CWinApiException(DISP_E_BADVARTYPE);

	if (args.size() > 4 && args.back().type() == VT_DISPATCH)
		vCallBack = args.back();

	int nIndex;
	CNavPointView* pNavPointView = RetrieveView(pDispParams, nIndex);
	if (pNavPointView->GetType(-1) == CNavPointView::E_VIEW_TYPE_ROADBOOK)
	{
		std::unique_ptr<geo::CGeoRouteOptions> cgOptions;
		geo::GeoVehicleType::type_t vehicleType = FillRouteOptionsWithSettings(cgOptions, true);
		geo::E_GEO_PROVIDER eGeoProvider = CITNConverterApp::RegParam().RouteProvider();

		geo::CGeoDirections gDirections(eGeoProvider);
		eGeoProvider = gDirections->getProvider();
		if (!gDirections->getSupportedTravelOptions()[vehicleType]->preview())
			return;

		geo::CGeoLatLngs cgLatLngs = static_cast<CNavRouteView*>(pNavPointView)->GetRoutePreview(
			nIndex,
			GetLatLngFromVariant(args[2]),
			args[3].asBool());

		if (vCallBack.type() == VT_DISPATCH)
		{
			m_AsyncRouteCalculation->setProvider(eGeoProvider);
			m_AsyncRouteCalculation->PostRequest(cgLatLngs, vehicleType, *cgOptions, [vCallBack, this](geo::E_GEO_STATUS_CODE eCode, const geo::GeoRoutes& gRoutes)
				{
					if (eCode != geo::E_GEO_OK)
						return;

					CRouteInfo* pRouteInfo = new CRouteInfo(gRoutes.front());
					pRouteInfo->polyline().setColor(0xFF0000);

					IDispatch* lpCallback = vCallBack.asDispatch();
					lpCallback->AddRef();

					std::unique_ptr<CJavaScript> pScript = std::make_unique<CJavaScript>(lpCallback);
					pScript->arg(CVariant(new CRouteDispatch(pRouteInfo, true)).variant());
					m_Navigator.PostJavaScript(pScript);
				});
		}
		else if (retval)
		{
			gDirections->Load(cgLatLngs, vehicleType, *cgOptions);
			if (gDirections->getStatus() == geo::E_GEO_OK)
			{
				CRouteInfo* pRouteInfo = new CRouteInfo(gDirections->getRoutes().front());
				pRouteInfo->polyline().setColor(0xFF0000);
				*retval = CVariant(new CRouteDispatch(pRouteInfo, true)).variant();
			}
		}
	}
}

void CWebExternal::GetElevation(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (!retval || pDispParams->cArgs < 1)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	*retval = CVariant(new CPointDispatch(new CGpsPoint(GetElevation(GetLatLngFromVariant(pDispParams->rgvarg[0]))), CNavPointView::E_VIEW_TYPE_MARKER)).variant();
}

void CWebExternal::GetSettings(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (pDispParams->cArgs > 0 || !retval)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	*retval = CVariant(new CSettingsDispatch).variant();
}

void CWebExternal::UseBase64(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (pDispParams->cArgs > 0 || !retval)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	*retval = CVariant(new CBase64Dispatch).variant();
}

void CWebExternal::UseUtf8(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (pDispParams->cArgs > 0 || !retval)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	*retval = CVariant(new CUtf8Dispatch).variant();
}

void CWebExternal::GetStrCoords(DISPPARAMS* pDispParams, VARIANT* retval)
{
	if (!retval || pDispParams->cArgs < 1)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	int iOption = 0;
	if (pDispParams->cArgs > 1 && pDispParams->rgvarg[pDispParams->cArgs - 2].vt == VT_I4)
		iOption = pDispParams->rgvarg[pDispParams->cArgs - 2].intVal;

	std::wstring strLat, strLng;
	GetStringCoordinates(GetLatLngFromVariant(pDispParams->rgvarg[pDispParams->cArgs - 1]), strLat, strLng);

	switch (iOption)
	{
	case 1: // Latitude
		*retval = CVariant(strLat).variant();
		break;
	case 2: // Longitude
		*retval = CVariant(strLng).variant();
		break;
	case 0: // Latitude,Longitude
	default:
		*retval = CVariant(strLat + _T(",") + strLng).variant();
		break;
	}
}

void CWebExternal::SetCurrentMap(DISPPARAMS* pDispParams, VARIANT* /*retval*/)
{
	if (pDispParams->cArgs < 2)
		throw CWinApiException(DISP_E_BADPARAMCOUNT);

	CVariantList args(*pDispParams);
	CITNConverterApp::RegParam().setCurrentMap(std::make_pair(args[0].asString(), args[1].asInteger()));
}
