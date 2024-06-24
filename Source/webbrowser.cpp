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
#include "webbrowser.h"

 /////////////////////////////////////////////////////////////////////////////
 // CWebBrowser

IMPLEMENT_DYNCREATE(CWebBrowser, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser properties

/////////////////////////////////////////////////////////////////////////////
// CWebBrowser operations

void CWebBrowser::GoBack()
{
	InvokeHelper(0x64, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
}

void CWebBrowser::GoForward()
{
	InvokeHelper(0x65, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
}

void CWebBrowser::GoHome()
{
	InvokeHelper(0x66, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
}

void CWebBrowser::GoSearch()
{
	InvokeHelper(0x67, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
}

void CWebBrowser::Navigate(LPCTSTR URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers)
{
	static BYTE parms[] =
		VTS_BSTR VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
	InvokeHelper(0x68, DISPATCH_METHOD, VT_EMPTY, nullptr, parms,
		URL, Flags, TargetFrameName, PostData, Headers);
}

void CWebBrowser::Refresh()
{
	InvokeHelper(DISPID_REFRESH, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
}

void CWebBrowser::Refresh2(VARIANT* Level)
{
	static BYTE parms[] =
		VTS_PVARIANT;
	InvokeHelper(0x69, DISPATCH_METHOD, VT_EMPTY, nullptr, parms,
		Level);
}

void CWebBrowser::Stop()
{
	InvokeHelper(0x6a, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
}

LPDISPATCH CWebBrowser::GetApplication()
{
	LPDISPATCH result;
	InvokeHelper(0xc8, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
	return result;
}

LPDISPATCH CWebBrowser::GetParent()
{
	LPDISPATCH result;
	InvokeHelper(0xc9, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
	return result;
}

LPDISPATCH CWebBrowser::GetContainer()
{
	LPDISPATCH result;
	InvokeHelper(0xca, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
	return result;
}

LPDISPATCH CWebBrowser::GetDocument()
{
	LPDISPATCH result;
	InvokeHelper(0xcb, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&result, nullptr);
	return result;
}

BOOL CWebBrowser::GetTopLevelContainer()
{
	BOOL result;
	InvokeHelper(0xcc, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

CString CWebBrowser::GetType()
{
	CString result;
	InvokeHelper(0xcd, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
	return result;
}

long CWebBrowser::GetLeft()
{
	long result;
	InvokeHelper(0xce, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetLeft(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xce, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		nNewValue);
}

long CWebBrowser::GetTop()
{
	long result;
	InvokeHelper(0xcf, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetTop(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xcf, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		nNewValue);
}

long CWebBrowser::GetWidth()
{
	long result;
	InvokeHelper(0xd0, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetWidth(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xd0, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		nNewValue);
}

long CWebBrowser::GetHeight()
{
	long result;
	InvokeHelper(0xd1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetHeight(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xd1, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		nNewValue);
}

CString CWebBrowser::GetLocationName()
{
	CString result;
	InvokeHelper(0xd2, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
	return result;
}

CString CWebBrowser::GetLocationURL()
{
	CString result;
	InvokeHelper(0xd3, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
	return result;
}

BOOL CWebBrowser::GetBusy()
{
	BOOL result;
	InvokeHelper(0xd4, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::Quit()
{
	InvokeHelper(0x12c, DISPATCH_METHOD, VT_EMPTY, nullptr, nullptr);
}

void CWebBrowser::ClientToWindow(long* pcx, long* pcy)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4;
	InvokeHelper(0x12d, DISPATCH_METHOD, VT_EMPTY, nullptr, parms,
		pcx, pcy);
}

void CWebBrowser::PutProperty(LPCTSTR Property_, const VARIANT& vtValue)
{
	static BYTE parms[] =
		VTS_BSTR VTS_VARIANT;
	InvokeHelper(0x12e, DISPATCH_METHOD, VT_EMPTY, nullptr, parms,
		Property_, &vtValue);
}

VARIANT CWebBrowser::GetProperty_(LPCTSTR Property_)
{
	VARIANT result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x12f, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms,
		Property_);
	return result;
}

CString CWebBrowser::GetName()
{
	CString result;
	InvokeHelper(0x0, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
	return result;
}

long CWebBrowser::GetHwnd()
{
	long result;
	InvokeHelper(DISPID_HWND, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
	return result;
}

CString CWebBrowser::GetFullName()
{
	CString result;
	InvokeHelper(0x190, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
	return result;
}

CString CWebBrowser::GetPath()
{
	CString result;
	InvokeHelper(0x191, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
	return result;
}

BOOL CWebBrowser::GetVisible()
{
	BOOL result;
	InvokeHelper(0x192, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetVisible(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x192, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

BOOL CWebBrowser::GetStatusBar()
{
	BOOL result;
	InvokeHelper(0x193, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetStatusBar(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x193, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

CString CWebBrowser::GetStatusText()
{
	CString result;
	InvokeHelper(0x194, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetStatusText(LPCTSTR lpszNewValue)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x194, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		lpszNewValue);
}

long CWebBrowser::GetToolBar()
{
	long result;
	InvokeHelper(0x195, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetToolBar(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x195, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		nNewValue);
}

BOOL CWebBrowser::GetMenuBar()
{
	BOOL result;
	InvokeHelper(0x196, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetMenuBar(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x196, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

BOOL CWebBrowser::GetFullScreen()
{
	BOOL result;
	InvokeHelper(0x197, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetFullScreen(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x197, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

void CWebBrowser::Navigate2(VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers)
{
	static BYTE parms[] =
		VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
	InvokeHelper(0x1f4, DISPATCH_METHOD, VT_EMPTY, nullptr, parms,
		URL, Flags, TargetFrameName, PostData, Headers);
}

long CWebBrowser::QueryStatusWB(long cmdID)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1f5, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		cmdID);
	return result;
}

void CWebBrowser::ExecWB(long cmdID, long cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT;
	InvokeHelper(0x1f6, DISPATCH_METHOD, VT_EMPTY, nullptr, parms,
		cmdID, cmdexecopt, pvaIn, pvaOut);
}

void CWebBrowser::ShowBrowserBar(VARIANT* pvaClsid, VARIANT* pvarShow, VARIANT* pvarSize)
{
	static BYTE parms[] =
		VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
	InvokeHelper(0x1f7, DISPATCH_METHOD, VT_EMPTY, nullptr, parms,
		pvaClsid, pvarShow, pvarSize);
}

long CWebBrowser::GetReadyState()
{
	long result;
	InvokeHelper(DISPID_READYSTATE, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, nullptr);
	return result;
}

BOOL CWebBrowser::GetOffline()
{
	BOOL result;
	InvokeHelper(0x226, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetOffline(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x226, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

BOOL CWebBrowser::GetSilent()
{
	BOOL result;
	InvokeHelper(0x227, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetSilent(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x227, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

BOOL CWebBrowser::GetRegisterAsBrowser()
{
	BOOL result;
	InvokeHelper(0x228, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetRegisterAsBrowser(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x228, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

BOOL CWebBrowser::GetRegisterAsDropTarget()
{
	BOOL result;
	InvokeHelper(0x229, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetRegisterAsDropTarget(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x229, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

BOOL CWebBrowser::GetTheaterMode()
{
	BOOL result;
	InvokeHelper(0x22a, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetTheaterMode(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x22a, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

BOOL CWebBrowser::GetAddressBar()
{
	BOOL result;
	InvokeHelper(0x22b, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetAddressBar(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x22b, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}

BOOL CWebBrowser::GetResizable()
{
	BOOL result;
	InvokeHelper(0x22c, DISPATCH_PROPERTYGET, VT_BOOL, (void*)&result, nullptr);
	return result;
}

void CWebBrowser::SetResizable(BOOL bNewValue)
{
	static BYTE parms[] =
		VTS_BOOL;
	InvokeHelper(0x22c, DISPATCH_PROPERTYPUT, VT_EMPTY, nullptr, parms,
		bNewValue);
}
