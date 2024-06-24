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
 * Purpose : Interface for the CNavigator class, CWebBrowser derived
 */

#ifndef AFX_NAVIGATOR_H_INCLUDED_
#define AFX_NAVIGATOR_H_INCLUDED_

#include <string>
#include <memory>
#include <mshtml.h>
#include <mshtmhst.h>
#include "webbrowser.h"
#include "WebExternal.h"
#include "ToolsLibrary/Raii.h"

class CJavaScript;
class CNavigator;

class CDocHostUIHandler : public IDocHostUIHandler
{
	friend class CNavigator;

public:
	CDocHostUIHandler(CNavigator& navigator);
	virtual ~CDocHostUIHandler();

	IDocHostUIHandler* SetDefaultClientSite(IOleClientSite* pClientSite);

	// IUnknown
	STDMETHOD(QueryInterface)(REFIID, void**) { return E_NOTIMPL; }
	ULONG STDMETHODCALLTYPE AddRef() { return 0; }
	ULONG STDMETHODCALLTYPE Release() { return 0; }

	// IDocHostUIHandler
	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(
		/* [in] */ DWORD /*dwID*/,
		/* [in] */ POINT* /*ppt*/,
		/* [in] */ IUnknown* /*pcmdtReserved*/,
		/* [in] */ IDispatch* /*pdispReserved*/)
	{
		return S_OK; // Disable context menu
		//		return m_pDefaultDocHostUIHandler->ShowContextMenu(dwID, ppt, pcmdtReserved, pdispReserved);
	}

	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(
		/* [out][in] */ DOCHOSTUIINFO* pInfo)
	{
		return m_pDefaultDocHostUIHandler->GetHostInfo(pInfo);
	}

	virtual HRESULT STDMETHODCALLTYPE ShowUI(
		/* [in] */ DWORD dwID,
		/* [in] */ IOleInPlaceActiveObject* pActiveObject,
		/* [in] */ IOleCommandTarget* pCommandTarget,
		/* [in] */ IOleInPlaceFrame* pFrame,
		/* [in] */ IOleInPlaceUIWindow* pDoc)
	{
		return m_pDefaultDocHostUIHandler->ShowUI(dwID, pActiveObject, pCommandTarget, pFrame, pDoc);
	}

	virtual HRESULT STDMETHODCALLTYPE HideUI()
	{
		return m_pDefaultDocHostUIHandler->HideUI();
	}

	virtual HRESULT STDMETHODCALLTYPE UpdateUI()
	{
		return m_pDefaultDocHostUIHandler->UpdateUI();
	}

	virtual HRESULT STDMETHODCALLTYPE EnableModeless(
		/* [in] */ BOOL fEnable)
	{
		return m_pDefaultDocHostUIHandler->EnableModeless(fEnable);
	}

	virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
		/* [in] */ BOOL fActivate)
	{
		return m_pDefaultDocHostUIHandler->OnDocWindowActivate(fActivate);
	}

	virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
		/* [in] */ BOOL fActivate)
	{
		return m_pDefaultDocHostUIHandler->OnFrameWindowActivate(fActivate);
	}

	virtual HRESULT STDMETHODCALLTYPE ResizeBorder(
		/* [in] */ LPCRECT prcBorder,
		/* [in] */ IOleInPlaceUIWindow* pUIWindow,
		/* [in] */ BOOL fRameWindow)
	{
		return m_pDefaultDocHostUIHandler->ResizeBorder(prcBorder, pUIWindow, fRameWindow);
	}

	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
		/* [in] */ LPMSG lpMsg,
		/* [in] */ const GUID* pguidCmdGroup,
		/* [in] */ DWORD nCmdID)
	{
		return m_pDefaultDocHostUIHandler->TranslateAccelerator(lpMsg, pguidCmdGroup, nCmdID);
	}

	virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
		/* [out] */
		__out  LPOLESTR* pchKey,
		/* [in] */ DWORD dw)
	{
		return m_pDefaultDocHostUIHandler->GetOptionKeyPath(pchKey, dw);
	}

	virtual HRESULT STDMETHODCALLTYPE GetDropTarget(
		/* [in] */ IDropTarget* pDropTarget,
		/* [out] */ IDropTarget** ppDropTarget)
	{
		return m_pDefaultDocHostUIHandler->GetDropTarget(pDropTarget, ppDropTarget);
	}

	virtual HRESULT STDMETHODCALLTYPE GetExternal(
		/* [out] */ IDispatch** ppDispatch);

	virtual HRESULT STDMETHODCALLTYPE TranslateUrl(
		/* [in] */ DWORD dwTranslate,
		/* [in] */
		__in __nullterminated  OLECHAR* pchURLIn,
		/* [out] */
		__out  OLECHAR** ppchURLOut)
	{
		return m_pDefaultDocHostUIHandler->TranslateUrl(dwTranslate, pchURLIn, ppchURLOut);
	}

	virtual HRESULT STDMETHODCALLTYPE FilterDataObject(
		/* [in] */ IDataObject* pDO,
		/* [out] */ IDataObject** ppDORet)
	{
		return m_pDefaultDocHostUIHandler->FilterDataObject(pDO, ppDORet);
	}

private:
	CWebExternal m_WebExternal;
	IDocHostUIHandler* m_pDefaultDocHostUIHandler;
};

class CNavigator : public CWebBrowser
{
public:
	CNavigator();
	virtual ~CNavigator();

	void InitInterface();
	void NewPinList(CNavPointView& cNavPointView, int nTabIndex);

	void JavaScript_ShowUpdateWindow();
	void JavaScript_CloseInfoWindow();
	void JavaScript_ViewAllIdem();
	void JavaScript_AddRoute(int nTabIndex, int nItemIndex);
	void JavaScript_ClearRoutes(int nTabIndex);
	void JavaScript_ClearPushPins(int nTabIndex);
	void JavaScript_RefreshPushPins(int nTabIndex, int nFirstItemIndex, int nLastItemIndex);
	void JavaScript_ViewPushPin(int nTabIndex, int nItemIndex);
	void JavaScript_DelPushPin(int nTabIndex, int nItemIndex);
	void JavaScript_InsertPushPin(int nTabIndex, int nItemIndex);
	void JavaScript_MapClickEnabled(bool bEnabled = true);
	void JavaScript_InitMap(const std::string& strParameters);
	std::string JavaScript_GetMaps();
	std::string JavaScript_GetProviders();

	void PostJavaScript(std::unique_ptr<CJavaScript>& pScript);

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	IDispatch* m_pScript;

	CDocHostUIHandler m_CurrentDocHostUIHandler;
	autoDispatch<IOleObject> m_pOleObject;
	IOleClientSite* m_pClientSite;
};

#endif // !AFX_NAVIGATOR_H_INCLUDED_
