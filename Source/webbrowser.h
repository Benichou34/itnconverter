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

#if !defined(AFX_WEBBROWSER_H__FDC796C2_B56F_4182_B4B4_0B22F3AC5067__INCLUDED_)
#define AFX_WEBBROWSER_H__FDC796C2_B56F_4182_B4B4_0B22F3AC5067__INCLUDED_

 // Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

 // NOTE: Do not modify the contents of this file.  If this class is regenerated by
 //  Microsoft Visual C++, your modifications will be overwritten.

 /////////////////////////////////////////////////////////////////////////////
 // CWebBrowser wrapper class

class CWebBrowser : public CWnd
{
protected:
	DECLARE_DYNCREATE(CWebBrowser)
public:
	CLSID const& GetClsid()
	{
		static CLSID const clsid
			= { 0x8856f961, 0x340a, 0x11d0, { 0xa9, 0x6b, 0x0, 0xc0, 0x4f, 0xd7, 0x5, 0xa2 } };
		return clsid;
	}

	// Attributes
public:

	// Operations
public:
	void GoBack();
	void GoForward();
	void GoHome();
	void GoSearch();
	void Navigate(LPCTSTR URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers);
	void Refresh();
	void Refresh2(VARIANT* Level);
	void Stop();
	LPDISPATCH GetApplication();
	LPDISPATCH GetParent();
	LPDISPATCH GetContainer();
	LPDISPATCH GetDocument();
	BOOL GetTopLevelContainer();
	CString GetType();
	long GetLeft();
	void SetLeft(long nNewValue);
	long GetTop();
	void SetTop(long nNewValue);
	long GetWidth();
	void SetWidth(long nNewValue);
	long GetHeight();
	void SetHeight(long nNewValue);
	CString GetLocationName();
	CString GetLocationURL();
	BOOL GetBusy();
	void Quit();
	void ClientToWindow(long* pcx, long* pcy);
	void PutProperty(LPCTSTR Property_, const VARIANT& vtValue);
	VARIANT GetProperty_(LPCTSTR Property_);
	CString GetName();
	long GetHwnd();
	CString GetFullName();
	CString GetPath();
	BOOL GetVisible();
	void SetVisible(BOOL bNewValue);
	BOOL GetStatusBar();
	void SetStatusBar(BOOL bNewValue);
	CString GetStatusText();
	void SetStatusText(LPCTSTR lpszNewValue);
	long GetToolBar();
	void SetToolBar(long nNewValue);
	BOOL GetMenuBar();
	void SetMenuBar(BOOL bNewValue);
	BOOL GetFullScreen();
	void SetFullScreen(BOOL bNewValue);
	void Navigate2(VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers);
	long QueryStatusWB(long cmdID);
	void ExecWB(long cmdID, long cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut);
	void ShowBrowserBar(VARIANT* pvaClsid, VARIANT* pvarShow, VARIANT* pvarSize);
	long GetReadyState();
	BOOL GetOffline();
	void SetOffline(BOOL bNewValue);
	BOOL GetSilent();
	void SetSilent(BOOL bNewValue);
	BOOL GetRegisterAsBrowser();
	void SetRegisterAsBrowser(BOOL bNewValue);
	BOOL GetRegisterAsDropTarget();
	void SetRegisterAsDropTarget(BOOL bNewValue);
	BOOL GetTheaterMode();
	void SetTheaterMode(BOOL bNewValue);
	BOOL GetAddressBar();
	void SetAddressBar(BOOL bNewValue);
	BOOL GetResizable();
	void SetResizable(BOOL bNewValue);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WEBBROWSER_H__FDC796C2_B56F_4182_B4B4_0B22F3AC5067__INCLUDED_)
