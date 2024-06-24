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
 * Purpose : Implementation of the CNavigator class
 */

#include "stdafx.h"
#include "Itn Tools.h"
#include "Navigator.h"
#include "Javascript.h"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////
CNavigator::CNavigator() :
	m_pScript(nullptr),
	m_CurrentDocHostUIHandler(*this),
	m_pClientSite(nullptr)
{
}

CNavigator::~CNavigator()
{
	try
	{
		if (m_pOleObject)
			m_pOleObject->SetClientSite(m_pClientSite);

		if (m_pScript)
			m_pScript->Release();
	}
	catch (...)
	{
	}
}

void CNavigator::InitInterface()
{
	if (m_pScript)
		m_pScript->Release();

	autoDispatch<IDispatch> pDisp(GetDocument(), false);
	autoDispatch<IHTMLDocument2> pHTMLDoc(pDisp.get(), IID_IHTMLDocument2);

	pHTMLDoc->get_Script(&m_pScript);

	// Request default handler from MSHTML client site
	m_pOleObject.QueryInterfaceFrom(pDisp.get(), IID_IOleObject);
	if (m_pOleObject)
	{
		if (SUCCEEDED(m_pOleObject->GetClientSite(&m_pClientSite)) && m_pClientSite)
			m_CurrentDocHostUIHandler.SetDefaultClientSite(m_pClientSite);
	}

	// Set this class to be the IDocHostUIHandler
	ICustomDoc* pCustomDoc;
	pHTMLDoc->QueryInterface(IID_ICustomDoc, reinterpret_cast<PVOID*>(&pCustomDoc));
	pCustomDoc->SetUIHandler(&m_CurrentDocHostUIHandler);
}

void CNavigator::NewPinList(CNavPointView& cNavPointView, int nTabIndex)
{
	m_CurrentDocHostUIHandler.m_WebExternal.AddView(cNavPointView, nTabIndex);
	NO_THROW(CJavaScript(m_pScript).method(L"NewPinList").arg(nTabIndex).execute());
}

void CNavigator::JavaScript_ShowUpdateWindow()
{
	NO_THROW(CJavaScript(m_pScript).method(L"ShowUpdateWindow").execute());
}

void CNavigator::JavaScript_CloseInfoWindow()
{
	NO_THROW(CJavaScript(m_pScript).method(L"CloseInfoWindow").execute());
}

void CNavigator::JavaScript_ViewAllIdem()
{
	NO_THROW(CJavaScript(m_pScript).method(L"AutoViewMap").execute());
}

void CNavigator::JavaScript_AddRoute(int nTabIndex, int nItemIndex)
{
	NO_THROW(CJavaScript(m_pScript).method(L"AddRoute").arg(nTabIndex).arg(nItemIndex).execute());
}

void CNavigator::JavaScript_ClearRoutes(int nTabIndex)
{
	NO_THROW(CJavaScript(m_pScript).method(L"ClearRoutes").arg(nTabIndex).execute());
}

void CNavigator::JavaScript_ClearPushPins(int nTabIndex)
{
	NO_THROW(CJavaScript(m_pScript).method(L"ClearPushPins").arg(nTabIndex).execute());
}

void CNavigator::JavaScript_RefreshPushPins(int nTabIndex, int nFirstItemIndex, int nLastItemIndex)
{
	NO_THROW(CJavaScript(m_pScript).method(L"RefreshPushPins").arg(nTabIndex).arg(nFirstItemIndex).arg(nLastItemIndex).execute());
}

void CNavigator::JavaScript_ViewPushPin(int nTabIndex, int nItemIndex)
{
	NO_THROW(CJavaScript(m_pScript).method(L"ViewPushPin").arg(nTabIndex).arg(nItemIndex).execute());
}

void CNavigator::JavaScript_DelPushPin(int nTabIndex, int nItemIndex)
{
	NO_THROW(CJavaScript(m_pScript).method(L"DelPushPin").arg(nTabIndex).arg(nItemIndex).execute());
}

void CNavigator::JavaScript_InsertPushPin(int nTabIndex, int nItemIndex)
{
	NO_THROW(CJavaScript(m_pScript).method(L"InsertPushPin").arg(nTabIndex).arg(nItemIndex).execute());
}

void CNavigator::JavaScript_MapClickEnabled(bool bEnabled)
{
	NO_THROW(CJavaScript(m_pScript).method(L"MapClickEnabled").arg(bEnabled).execute());
}

void CNavigator::JavaScript_InitMap(const std::string& strParameters)
{
	NO_THROW(CJavaScript(m_pScript).method(L"InitMap").arg(strParameters).execute());
}

std::string CNavigator::JavaScript_GetMaps()
{
	CAutoVariant vResult;

	NO_THROW(CJavaScript(m_pScript).method(L"GetMaps").execute(vResult));
	if (vResult.type() == VT_BSTR)
		return vResult.asString();

	return std::string();
}

std::string CNavigator::JavaScript_GetProviders()
{
	CAutoVariant vResult;

	NO_THROW(CJavaScript(m_pScript).method(L"GetProviders").execute(vResult));
	if (vResult.type() == VT_BSTR)
		return vResult.asString();

	return std::string();
}

#define WM_JS_CALLBACK (WM_USER + 200)

void CNavigator::PostJavaScript(std::unique_ptr<CJavaScript>& pScript)
{
	if (IsWindow(m_hWnd))
		this->PostMessage(WM_JS_CALLBACK, reinterpret_cast<WPARAM>(pScript.release()), NULL);
}

BOOL CNavigator::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg && pMsg->message == WM_JS_CALLBACK)
	{
		std::unique_ptr<CJavaScript> pJavascript(reinterpret_cast<CJavaScript*>(pMsg->wParam));
		if (pJavascript)
		{
			NO_THROW(pJavascript->execute());
			pMsg->wParam = NULL;
		}
	}

	return CWebBrowser::PreTranslateMessage(pMsg);
}

CDocHostUIHandler::CDocHostUIHandler(CNavigator& navigator) :
	m_WebExternal(navigator),
	m_pDefaultDocHostUIHandler(nullptr)
{
}

CDocHostUIHandler::~CDocHostUIHandler()
{
	SetDefaultClientSite(nullptr);
}

IDocHostUIHandler* CDocHostUIHandler::SetDefaultClientSite(IOleClientSite* pClientSite)
{
	if (pClientSite)
	{
		pClientSite->AddRef();
		pClientSite->QueryInterface(IID_IDocHostUIHandler, (PVOID*)&m_pDefaultDocHostUIHandler);
	}
	else if (m_pDefaultDocHostUIHandler)
	{
		m_pDefaultDocHostUIHandler->Release();
		m_pDefaultDocHostUIHandler = nullptr;
	}

	return m_pDefaultDocHostUIHandler;
}

HRESULT STDMETHODCALLTYPE CDocHostUIHandler::GetExternal(IDispatch** ppDispatch)
{
	*ppDispatch = &m_WebExternal;
	(*ppDispatch)->AddRef();

	return S_OK;
}
