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
 * Purpose : Advanced editor window
 */

 //{{AFX_INCLUDES()
#include "webbrowser.h"
//}}AFX_INCLUDES
#if !defined(AFX_EDITORDLG_H_INCLUDED_)
#define AFX_EDITORDLG_H_INCLUDED_

#include "ITN Tools.h"
#include "NavRouteView.h"
#include "SizeableDlg.h"
#include "EditCtrl.h"
#include "jsMapInfos.h"
#include "jsProviderInfos.h"
#include "BtnST.h"

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg dialog

class CEditorDlg : public CSizeableDialog
{
	// Construction
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CEditorDlg(CGpsPointArray& cGpsPointArray, CWnd* pParent = nullptr);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CEditorDlg)
	enum { IDD = IDD_DIALOG_EDITOR };
	CStatic	m_LabelDistance;
	CStatic	m_LabelStatus;
	CStatic	m_LabelPointNumber;
	CNavPointView	m_ListSearch;
	CNavPointView	m_ListProximity;
	CNavPointView   m_ListFavorites;
	CNavRouteView	m_ListPoint;
	CEditControl m_EditSearch;
	CEdit	m_EditName;
	CNavigator	m_Navigator;
	CString	m_csSearch;
	CMenu m_FavPopupMenu;
	CButtonST m_btSearch;
	//}}AFX_DATA

	enum
	{
		BMP_MENU_CLEAR,
		BMP_MENU_IMPORT,
		BMP_MENU_EXPORT,
		BMP_MENU_NUMBER
	};

	enum
	{
		TAB_LIST_WAYPOINT,
		TAB_LIST_SEARCH,
		TAB_LIST_PROXIMITY,
		TAB_LIST_FAVORITE,
		TAB_LIST_NUMBER
	};

	CGpsPointArray& m_cGpsPointArray;
	CGpsWaypointArray m_cLocationResult;
	CGpsWaypointArray m_cProximityResult;
	CGpsWaypointArray m_cFavorites;
	geo::CGeoLocation m_cSelectedLocation;
	CGpsPointView* m_ptabList[TAB_LIST_NUMBER];
	CBitmap m_bmpMenuArray[BMP_MENU_NUMBER]; // bmp 15x15 pixels

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorDlg)
protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	void UpdateButtons(int nIndex = -1);
	void UpdateResults();
	void UpdatePois();
	void UpdateSearchArea();
	void UpdateCalculateButton();
	void OnSignal(const std::string& strText);
	void CallNavigate();
	void OnButtonClearFavorites();
	void OnButtonImportFavorites();
	void OnButtonExportFavorites();

	// Implementation
protected:
	CToolTipCtrl m_tooltip;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CEditorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonInvert();
	afx_msg void OnButtonViewall();
	afx_msg void OnButtonCalculate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSearch();
	afx_msg void OnDblclkListSearch(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListSearch(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListProximity(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkListFavorites(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListFavorites(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListFavorites(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeforeNavigate2Explorer(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Flags, VARIANT FAR* TargetFrameName, VARIANT FAR* PostData, VARIANT FAR* Headers, BOOL FAR* Cancel);
	afx_msg void OnClose();
	afx_msg void OnNavigateErrorExplorer(LPDISPATCH pDisp, VARIANT FAR* URL, VARIANT FAR* Frame, VARIANT FAR* StatusCode, BOOL FAR* Cancel);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnProgressChangeExplorer(long Progress, long ProgressMax);
	afx_msg void OnButtonDelroute();
	afx_msg void OnButtonRemoveall();
	afx_msg void OnButtonOptimise();
	afx_msg void OnButtonPrint();
	afx_msg void OnButtonPoi();
	afx_msg void OnSearchLocation();
	afx_msg void OnSearchProximity();
	afx_msg void OnDisplayFavorites();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnButtonConfig();
	virtual void OnOK();
	DECLARE_EVENTSINK_MAP()
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

private:
	jsMapsInfos m_MapsInfos;
	jsProvidersInfos m_ProvidersInfos;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITORDLG_H_INCLUDED_)
