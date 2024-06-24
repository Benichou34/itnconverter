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
 * Purpose : Main dialog Windows implementation
 */

#include "stdafx.h"
#include <algorithm>
#include "AboutDlg.h"
#include "ITN ConverterDlg.h"
#include "ITN FileDialog.h"
#include "ToolsLibrary/Internet.h"
#include "EditorDlg.h"
#include "InsertModify.h"
#include "ChooseArrayDlg.h"
#include "stdx/guard.h"
#include "stdx/uri_helper.h"

static CGpsPointView::RVCOLUMN sTabColumn[] =
{
	{ IDS_ADDRESS, LVCFMT_LEFT, 198, CGpsPointView::E_RVCOL_ADDRESS },
	{ IDS_LONGITUDE, LVCFMT_RIGHT, 63, CGpsPointView::E_RVCOL_LONGITUDE },
	{ IDS_LATITUDE, LVCFMT_RIGHT, 63, CGpsPointView::E_RVCOL_LATITUDE },
	{ 0, 0, 0, CGpsPointView::E_RVCOL_ADDRESS }
};

#define DEFAULT_EXT  0

static const DWORD sTabCutItn[][2] =
{
	{ IDS_NONE, 0 },
	{ IDS_CUT_ITN_TT3, 16 },
	{ IDS_CUT_ITN_TT5, 48 },
	{ IDS_CUT_ITN_RIDER, 100 },
	{ IDS_CUT_ITN_CUSTOM, MAXDWORD },
	{ 0, 0 }
};

/* Context menu */
#define WM_MODIFY             WM_USER+1001
#define WM_REMOVE             WM_USER+1002
#define WM_GOOGLE_MAP_VIEW    WM_USER+1003
#define WM_VIRTUAL_EARTH_VIEW WM_USER+1004
#define WM_TOMTOM_MAP_VIEW    WM_USER+1005
#define WM_HERE_MAP_VIEW      WM_USER+1006
#define WM_SEND_TO_NVGFRESH   WM_USER+1007
#define WM_SEND_TO_PIONEER    WM_USER+1008
#define WM_SEND_TO_MIOMORE    WM_USER+1009
#define WM_SEND_TO_GARMIN     WM_USER+1010

/////////////////////////////////////////////////////////////////////////////
// CITNConverterDlg dialog

CITNConverterDlg::CITNConverterDlg(CWnd* pParent /*=nullptr*/)
	: CSizeableDialog(CITNConverterDlg::IDD, pParent)
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	Clear();
}

void CITNConverterDlg::DoDataExchange(CDataExchange* pDX)
{
	CSizeableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CITNConverterDlg)
#ifdef IDC_EDIT_CUT_ITN
	DDX_Control(pDX, IDC_EDIT_CUT_ITN, m_EditCutItn);
#endif
	DDX_Control(pDX, IDC_LIST, m_ListPoint);
	DDX_Control(pDX, IDC_EDIT_NAME, m_EditName);
	DDX_Control(pDX, IDC_POINT_NUMBER, m_LabelPointNumber);
	DDX_Control(pDX, IDC_FILENAME, m_LabelFileName);
	DDX_Control(pDX, IDC_COMBO_FILEEXPORT, m_cbExportExt);
#ifdef IDC_COMBO_CUT_ITN
	DDX_Control(pDX, IDC_COMBO_CUT_ITN, m_cbCutItn);
#endif
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CITNConverterDlg, CSizeableDialog)
	//{{AFX_MSG_MAP(CITNConverterDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOPEN, OnOpen)
	ON_BN_CLICKED(IDEXPORT, OnExport)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_EDIT_NAME, OnChangeEditName)
	ON_CBN_SELCHANGE(IDC_COMBO_FILEEXPORT, OnSelchangeComboFileexport)
#ifdef IDC_COMBO_CUT_ITN
	ON_CBN_SELCHANGE(IDC_COMBO_CUT_ITN, OnSelchangeComboCutItn)
#endif
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_INVERT, OnButtonInvert)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnKeydownList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, OnRclickList)
	ON_BN_CLICKED(IDC_BUTTON_EDITOR, OnButtonEditor)
	ON_MESSAGE(WM_DROPFILES, OnDropFiles)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CITNConverterDlg message handlers

BOOL CITNConverterDlg::OnInitDialog()
{
	CSizeableDialog::OnInitDialog();

	// Add "About..." menu item to system menu.
	AddAboutBox(this);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);       // Set big icon
	SetIcon(m_hIcon, FALSE);      // Set small icon

	// Create the ToolTip control.
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);

	m_FileFormats = getFileFormats();

	CString resStr;

	// Change title
	SetWindowText(stdx::string_helper::widen(SOFT_FULL_NAME).c_str());

#ifdef IDC_STATIC_BACK_BT
	WINDOWPLACEMENT wp;
	GetDlgItem(IDC_STATIC_BACK_BT)->GetWindowPlacement(&wp);
	AddBackgroundImage(IDB_BACK_BT_1, wp.rcNormalPosition.top, wp.rcNormalPosition.left, E_RESIZE_PROP_LEFT);
#endif

	// Icons
	CWnd* pWnd = GetDlgItem(IDC_BUTTON_UP);
#ifndef IDB_UP_BT_ON
	static_cast<CButton*>(pWnd)->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_UP)));
	pWnd->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstUp.DrawTransparent(TRUE);
	m_bstUp.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 10, 0 });
	m_bstUp.DrawBorder(FALSE);
	m_bstUp.SetWindowText(CWToolsString::Load(IDS_UP).c_str());
	m_bstUp.SetBitmaps(IDB_UP_BT_ON, 0, IDB_UP_BT_OFF);
#endif
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_UP);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);

	pWnd = GetDlgItem(IDC_BUTTON_DOWN);
#ifndef IDB_DOWN_BT_ON
	static_cast<CButton*>(pWnd)->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_DOWN)));
	pWnd->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstDown.DrawTransparent(TRUE);
	m_bstDown.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 10, 0 });
	m_bstDown.DrawBorder(FALSE);
	m_bstDown.SetWindowText(CWToolsString::Load(IDS_DOWN).c_str());
	m_bstDown.SetBitmaps(IDB_DOWN_BT_ON, 0, IDB_DOWN_BT_OFF);
#endif
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_DOWN);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);

	pWnd = GetDlgItem(IDC_BUTTON_ADD);
#ifndef IDB_ADD_BT_ON
	static_cast<CButton*>(pWnd)->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_ADD)));
	pWnd->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstAdd.DrawTransparent(TRUE);
	m_bstAdd.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 10, 0 });
	m_bstAdd.DrawBorder(FALSE);
	m_bstAdd.SetWindowText(CWToolsString::Load(IDS_ADD).c_str());
	m_bstAdd.SetBitmaps(IDB_ADD_BT_ON, 0, IDB_ADD_BT_OFF);
#endif
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_ADD);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);

	pWnd = GetDlgItem(IDC_BUTTON_REMOVE);
#ifndef IDB_DEL_BT_ON
	static_cast<CButton*>(pWnd)->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_REMOVE)));
	pWnd->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstDel.DrawTransparent(TRUE);
	m_bstDel.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 10, 0 });
	m_bstDel.DrawBorder(FALSE);
	m_bstDel.SetWindowText(CWToolsString::Load(IDS_REMOVE).c_str());
	m_bstDel.SetBitmaps(IDB_DEL_BT_ON, 0, IDB_DEL_BT_OFF);
#endif
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_REMOVE);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);

	pWnd = GetDlgItem(IDC_BUTTON_INVERT);
#ifndef IDB_INVERT_BT_ON
	static_cast<CButton*>(pWnd)->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_INVERT)));
	pWnd->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstInvert.DrawTransparent(TRUE);
	m_bstInvert.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 10, 0 });
	m_bstInvert.DrawBorder(FALSE);
	m_bstInvert.SetWindowText(CWToolsString::Load(IDS_INVERT).c_str());
	m_bstInvert.SetBitmaps(IDB_INVERT_BT_ON, 0, IDB_INVERT_BT_OFF);
#endif
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_INVERT);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);

	pWnd = GetDlgItem(IDC_BUTTON_EDITOR);
#ifndef IDB_EDITOR_BT
	static_cast<CButton*>(pWnd)->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_COMPASS)));
	pWnd->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstEditor.DrawTransparent(TRUE);
	m_bstEditor.DrawBorder(FALSE);
	m_bstEditor.SetAlign(CButtonST::ST_ALIGN_VERT);
	m_bstEditor.SetWindowText(CWToolsString::Load(IDS_EDITOR).c_str());
	m_bstEditor.SetBitmaps(IDB_EDITOR_BT, 0);
#endif
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_EDITOR);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);

	// Buttons
	pWnd = GetDlgItem(IDEXPORT);
	pWnd->SetWindowText(CWToolsString::Load(IDS_BT_EXPORT).c_str());
	pWnd->EnableWindow(FALSE);
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_BT_EXPORT);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_TOP);

	pWnd = GetDlgItem(IDOK);
	pWnd->SetWindowText(CWToolsString::Load(IDS_BT_EXIT).c_str());
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_BT_EXIT);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#ifdef IDB_EXIT_BT
	m_bstExit.DrawTransparent(TRUE);
	m_bstExit.DrawBorder(FALSE);
	m_bstExit.SetAlign(CButtonST::ST_ALIGN_HORIZ_RIGHT);
	m_bstExit.SetBitmaps(IDB_EXIT_BT, 0);
	m_bstExit.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
#endif

	pWnd = GetDlgItem(IDOPEN);
	pWnd->SetWindowText(CWToolsString::Load(IDS_BT_OPEN).c_str());
	m_tooltip.AddTool(pWnd, IDS_TOOLTIP_BT_OPEN);
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#ifdef IDB_EJECT_BT
	m_bstOpen.DrawTransparent(TRUE);
	m_bstOpen.DrawBorder(FALSE);
	m_bstOpen.SetBitmaps(IDB_EJECT_BT, 0);
	//	m_bstOpen.SetBitmaps(IDB_EJECT_BT_ON, RGB(0xFF, 0, 0), IDB_EJECT_BT_OFF, RGB(0xFF, 0, 0));
	m_bstOpen.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
#endif

	// Labels
	pWnd = GetDlgItem(IDC_NAME);
	pWnd->SetWindowText(CWToolsString::Load(IDS_NAME).c_str());

	pWnd = GetDlgItem(IDC_LABEL_EXPORT);
	pWnd->SetWindowText(CWToolsString::Load(IDS_LABEL_EXPORT).c_str());
	AddSizeableControl(pWnd, E_RESIZE_PROP_TOP);
#ifdef IDB_EXPORT_BT_ON
	m_bstExport.DrawTransparent(TRUE);
	m_bstExport.DrawBorder(FALSE);
	m_bstExport.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstExport.SetBitmaps(IDB_EXPORT_BT_ON, 0, IDB_EXPORT_BT_OFF, 0);
	m_bstExport.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
#endif

#ifdef IDC_STATIC_UP
	pWnd = GetDlgItem(IDC_STATIC_UP);
	pWnd->SetWindowText(CWToolsString::Load(IDS_UP).c_str());
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#endif

#ifdef IDC_STATIC_DOWN
	pWnd = GetDlgItem(IDC_STATIC_DOWN);
	pWnd->SetWindowText(CWToolsString::Load(IDS_DOWN).c_str());
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#endif

#ifdef IDC_STATIC_ADD
	pWnd = GetDlgItem(IDC_STATIC_ADD);
	pWnd->SetWindowText(CWToolsString::Load(IDS_ADD).c_str());
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#endif

#ifdef IDC_STATIC_REMOVE
	pWnd = GetDlgItem(IDC_STATIC_REMOVE);
	pWnd->SetWindowText(CWToolsString::Load(IDS_REMOVE).c_str());
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#endif

#ifdef IDC_STATIC_INVERT
	pWnd = GetDlgItem(IDC_STATIC_INVERT);
	pWnd->SetWindowText(CWToolsString::Load(IDS_INVERT).c_str());
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#endif

#ifdef IDC_STATIC_EDITOR
	pWnd = GetDlgItem(IDC_STATIC_EDITOR);
	pWnd->SetWindowText(CWToolsString::Load(IDS_EDITOR).c_str());
	AddSizeableControl(pWnd, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#endif

	// List of points
	m_ListPoint.Init(m_cGpsRoute, sTabColumn, true, true);
	AddSizeableControl(&m_ListPoint, E_RESIZE_PROP_WIDTH | E_RESIZE_PROP_HEIGHT);

	m_LabelFileName.SetWindowText(_T(""));
	AddSizeableControl(&m_LabelFileName, E_RESIZE_PROP_WIDTH);
	m_LabelPointNumber.SetWindowText(_T(""));
	AddSizeableControl(&m_LabelPointNumber, E_RESIZE_PROP_WIDTH);

	m_EditName.SetWindowText(_T(""));
	AddSizeableControl(&m_EditName, E_RESIZE_PROP_WIDTH);

#ifdef IDC_EDIT_CUT_ITN
	m_EditCutItn.SetLimitText(4);
	AddSizeableControl(&m_cbCutItn, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_TOP | E_RESIZE_PROP_REDRAW);
	AddSizeableControl(&m_EditCutItn, E_RESIZE_PROP_LEFT | E_RESIZE_PROP_TOP | E_RESIZE_PROP_REDRAW);
	AddSizeableControl(GetDlgItem(IDC_CHECK_OPTION), E_RESIZE_PROP_LEFT | E_RESIZE_PROP_TOP | E_RESIZE_PROP_REDRAW);
	AddSizeableControl(GetDlgItem(IDC_STATIC_OPTIONS), E_RESIZE_PROP_LEFT | E_RESIZE_PROP_TOP | E_RESIZE_PROP_REDRAW);

	// Construct ITN cut combo box
	{
		int i = 0;
		while (sTabCutItn[i][0])
		{
			resStr.LoadString(sTabCutItn[i][0]);
			int nIndex = m_cbCutItn.AddString(resStr);
			m_cbCutItn.SetItemData(nIndex, sTabCutItn[i][1]);
			i++;
		}
		m_cbCutItn.SetCurSel(0);
	}
#endif

	// Exported file type selection
	UpdateFileExportCombo();
	AddSizeableControl(&m_cbExportExt, E_RESIZE_PROP_TOP | E_RESIZE_PROP_WIDTH);

	// Create Popup context menu
	m_PopupMenu.CreatePopupMenu();

	resStr.LoadString(IDS_MODIFY);
	m_PopupMenu.AppendMenu(MF_STRING, WM_MODIFY, resStr);

	resStr.LoadString(IDS_REMOVE);
	m_PopupMenu.AppendMenu(MF_STRING, WM_REMOVE, resStr);

	m_PopupMenu.AppendMenu(MF_SEPARATOR);
	//	resStr.LoadString(IDS_TOMTOM_MAP_VIEW);
	//	m_PopupMenu.AppendMenu(MF_STRING, WM_TOMTOM_MAP_VIEW, resStr); // TODO

	resStr.LoadString(IDS_GOOGLE_MAP_VIEW);
	m_PopupMenu.AppendMenu(MF_STRING, WM_GOOGLE_MAP_VIEW, resStr);

	resStr.LoadString(IDS_VIRTUAL_EARTH_VIEW);
	m_PopupMenu.AppendMenu(MF_STRING, WM_VIRTUAL_EARTH_VIEW, resStr);

	resStr.LoadString(IDS_HERE_MAPS_VIEW);
	m_PopupMenu.AppendMenu(MF_STRING, WM_HERE_MAP_VIEW, resStr);

	if (IsVistaOrSuperior())
	{
		m_bmpMenuArray[BMP_MENU_MODIFY].Attach(LoadTransparentBitmap(IDB_MODIFY_POPUP));
		m_bmpMenuArray[BMP_MENU_REMOVE].Attach(LoadTransparentBitmap(IDB_REMOVE_POPUP));
		m_bmpMenuArray[BMP_MENU_TOMTOM_MAP_VIEW].Attach(LoadTransparentBitmap(IDB_TOMTOM_MAP));
		m_bmpMenuArray[BMP_MENU_GOOGLE_MAP_VIEW].Attach(LoadTransparentBitmap(IDB_GOOGLE_MAP));
		m_bmpMenuArray[BMP_MENU_VIRTUAL_EARTH_VIEW].Attach(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_VIRTUAL_EARTH)));
		m_bmpMenuArray[BMP_MENU_HERE_MAP_VIEW].Attach(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_HERE_MAP)));
		m_bmpMenuArray[BMP_MENU_ADD_TO_TOMTOM].Attach(LoadTransparentBitmap(IDB_TOMTOM_HOME));
		m_bmpMenuArray[BMP_MENU_SEND_TO_NVGFRESH].Attach(LoadTransparentBitmap(IDB_NVG_FRESH));
		m_bmpMenuArray[BMP_MENU_SEND_TO_NAVGATE].Attach(LoadTransparentBitmap(IDB_PIONEER_NAVGATE));
		m_bmpMenuArray[BMP_MENU_SEND_TO_MIOMORE].Attach(LoadTransparentBitmap(IDB_MIOMORE_DESKTOP));
		m_bmpMenuArray[BMP_MENU_SEND_TO_GARMIN].Attach(LoadTransparentBitmap(IDB_GARMIN_COMMUNICATOR));

		m_PopupMenu.SetMenuItemBitmaps(WM_MODIFY, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_MODIFY], &m_bmpMenuArray[BMP_MENU_MODIFY]);
		m_PopupMenu.SetMenuItemBitmaps(WM_REMOVE, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_REMOVE], &m_bmpMenuArray[BMP_MENU_REMOVE]);
		m_PopupMenu.SetMenuItemBitmaps(WM_VIRTUAL_EARTH_VIEW, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_VIRTUAL_EARTH_VIEW], &m_bmpMenuArray[BMP_MENU_VIRTUAL_EARTH_VIEW]);
		m_PopupMenu.SetMenuItemBitmaps(WM_TOMTOM_MAP_VIEW, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_TOMTOM_MAP_VIEW], &m_bmpMenuArray[BMP_MENU_TOMTOM_MAP_VIEW]);
		m_PopupMenu.SetMenuItemBitmaps(WM_GOOGLE_MAP_VIEW, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_GOOGLE_MAP_VIEW], &m_bmpMenuArray[BMP_MENU_GOOGLE_MAP_VIEW]);
		m_PopupMenu.SetMenuItemBitmaps(WM_HERE_MAP_VIEW, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_HERE_MAP_VIEW], &m_bmpMenuArray[BMP_MENU_HERE_MAP_VIEW]);
	}

	bool bSeparator = false;
	if (CSendToGps::IsAvailabled(CSendToGps::E_GPS_NAVIGON, CSendToGps::E_TYPE_POINT))
	{
		if (!bSeparator)
		{
			m_PopupMenu.AppendMenu(MF_SEPARATOR);
			bSeparator = true;
		}

		resStr.LoadString(IDS_SEND_TO_NVGFRESH);
		m_PopupMenu.AppendMenu(MF_STRING, WM_SEND_TO_NVGFRESH, resStr);
		if (IsVistaOrSuperior())
			m_PopupMenu.SetMenuItemBitmaps(WM_SEND_TO_NVGFRESH, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_SEND_TO_NVGFRESH], &m_bmpMenuArray[BMP_MENU_SEND_TO_NVGFRESH]);
	}

	if (CSendToGps::IsAvailabled(CSendToGps::E_GPS_MIOMORE, CSendToGps::E_TYPE_POINT))
	{
		if (!bSeparator)
		{
			m_PopupMenu.AppendMenu(MF_SEPARATOR);
			bSeparator = true;
		}

		resStr.LoadString(IDS_SEND_TO_MIOMORE);
		m_PopupMenu.AppendMenu(MF_STRING, WM_SEND_TO_MIOMORE, resStr);
		if (IsVistaOrSuperior())
			m_PopupMenu.SetMenuItemBitmaps(WM_SEND_TO_MIOMORE, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_SEND_TO_MIOMORE], &m_bmpMenuArray[BMP_MENU_SEND_TO_MIOMORE]);
	}

	if (CSendToGps::IsAvailabled(CSendToGps::E_GPS_PIONEER, CSendToGps::E_TYPE_POINT))
	{
		if (!bSeparator)
		{
			m_PopupMenu.AppendMenu(MF_SEPARATOR);
			bSeparator = true;
		}

		resStr.LoadString(IDS_SEND_TO_NAVGATE);
		m_PopupMenu.AppendMenu(MF_STRING, WM_SEND_TO_PIONEER, resStr);
		if (IsVistaOrSuperior())
			m_PopupMenu.SetMenuItemBitmaps(WM_SEND_TO_PIONEER, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_SEND_TO_NAVGATE], &m_bmpMenuArray[BMP_MENU_SEND_TO_NAVGATE]);
	}

	if (CSendToGps::IsAvailabled(CSendToGps::E_GPS_GARMIN, CSendToGps::E_TYPE_POINT))
	{
		if (!bSeparator)
		{
			m_PopupMenu.AppendMenu(MF_SEPARATOR);
			bSeparator = true;
		}

		resStr.LoadString(IDS_SEND_TO_GARMIN);
		m_PopupMenu.AppendMenu(MF_STRING, WM_SEND_TO_GARMIN, resStr);
		if (IsVistaOrSuperior())
			m_PopupMenu.SetMenuItemBitmaps(WM_SEND_TO_GARMIN, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_SEND_TO_GARMIN], &m_bmpMenuArray[BMP_MENU_SEND_TO_GARMIN]);
	}

	// Restore current windows position
	WINDOWPLACEMENT& wdPlacement = CITNConverterApp::RegParam().ConverterPlacement();
	if (wdPlacement.length)
		SetWindowPlacement(&wdPlacement);

	// Verifier la ligne de commande
	if (!CheckCommandLine())
		EndDialog(0);

	// Autorisation Drag Drop de fichiers
	DragAcceptFiles();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BOOL CITNConverterDlg::PreTranslateMessage(MSG* pMsg)
{
	// Let the ToolTip process this message.
	m_tooltip.RelayEvent(pMsg);

	return CSizeableDialog::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}
void CITNConverterDlg::OnClose()
{
	OnOK();
}

void CITNConverterDlg::OnOK()
{
	if (CheckModified())
	{
		Clear();

		WINDOWPLACEMENT& wdPlacement = CITNConverterApp::RegParam().ConverterPlacement();

		GetWindowPlacement(&wdPlacement);
		wdPlacement.length = sizeof(wdPlacement);

		CITNConverterApp::RegParam().Flush();

		CSizeableDialog::OnOK();
	}
}

bool CITNConverterDlg::CheckCommandLine()
{
	stdx::wstring_helper::vector vecArglist;
	CITNConverterApp::GetCommandLineArgv(vecArglist);

	stdx::wstring_helper::vector::iterator it = vecArglist.begin();
	while (it != vecArglist.end())
	{
		if (it->find(_T("LANG_")) != std::wstring::npos)
			it = vecArglist.erase(it);
		else
			++it;
	}

	if (vecArglist.size() == 1)
	{
		// Seulement un fichier, on l'ouvre juste
		// On verifie que le fichier existe
		if (GetFileAttributes(vecArglist[0].c_str()) != INVALID_FILE_ATTRIBUTES)
			OpenFile(vecArglist[0].c_str(), false, true);
	}
	else if (vecArglist.size() > 1)
	{
		std::wstring strLower(vecArglist[1]);
		stdx::wstring_helper::tolower(strLower);

		// Vérifie le deuxième argument
		auto itFileFormat = m_FileFormats.begin();
		while (itFileFormat != m_FileFormats.end())
		{
			if (!strLower.compare(itFileFormat->szFileExt) && itFileFormat->pWriteFile)
				break;

			++itFileFormat;
		}

		if (itFileFormat != m_FileFormats.end())
		{
			DWORD dwFileAttributes = GetFileAttributes(vecArglist[0].c_str());
			if (dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				WIN32_FIND_DATA fdFindFileData;
				HANDLE hFile;

				CString szFilePath = CWToolsString::FileDirectory(vecArglist[0]).c_str();
				hFile = FindFirstFile(vecArglist[0].c_str(), &fdFindFileData);

				do
				{
					CString szOldPathName = szFilePath + fdFindFileData.cFileName;
					if (OpenFile(szOldPathName, false, true) == S_OK)
					{
						CString szNewPathName;
						ReplaceFileExt(szOldPathName, itFileFormat->szFileExt, szNewPathName);
						ExportFile(szNewPathName, *itFileFormat, true);
					}
				} while (FindNextFile(hFile, &fdFindFileData));

				FindClose(hFile);
			}
			else if (dwFileAttributes != INVALID_FILE_ATTRIBUTES)
			{
				if (OpenFile(vecArglist[0].c_str(), false, true) == S_OK)
				{
					CString szNewPathName;
					ReplaceFileExt(vecArglist[0].c_str(), itFileFormat->szFileExt, szNewPathName);
					ExportFile(szNewPathName, *itFileFormat, true);
				}
			}
		}

		return false;
	}

	return true;
}

void CITNConverterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (!ShowAboutBox(nID))
		CSizeableDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CITNConverterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//      GetDlgItem(IDC_BACKGROUND)->SetWindowPos(&CWnd::wndBottom, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		CSizeableDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CITNConverterDlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

LRESULT CITNConverterDlg::OnDropFiles(WPARAM wParam, LPARAM /*lParam*/)
{
	HDROP hDropInfo = (HDROP)wParam;
	TCHAR szFileName[MAX_PATH];

	if (DragQueryFile(hDropInfo, 0, szFileName, MAX_PATH))
		OpenFile(szFileName, false, false);

	DragFinish(hDropInfo);
	return 0;
}

void CITNConverterDlg::OnOpen()
{
	if (!CheckModified())
		return;

	CString resStr;
	CString tmpStr;

	// Construct first open filter
	for (auto it = m_FileFormats.begin(); it != m_FileFormats.end(); ++it)
	{
		if (it->pReadFile)
		{
			resStr.LoadString(it->nFilter);
			resStr.Delete(0, resStr.Find('|') + 1);
			if (tmpStr.Find(resStr) == -1)
				tmpStr += resStr;
		}
	}

	while (tmpStr.Replace('|', ';'));

	// Construct full open filter
	resStr.Format(IDS_OPENFILTERS, (LPCTSTR)tmpStr.Left(tmpStr.ReverseFind(';')));

	for (auto it = m_FileFormats.begin(); it != m_FileFormats.end(); ++it)
	{
		if (it->pReadFile)
		{
			tmpStr.LoadString(it->nFilter);
			resStr += tmpStr;
		}
	}

	tmpStr.LoadString(IDS_ALLFILTERS);
	resStr += tmpStr;

	// Create open file dialog
	CITNFileDialog cFileDialog(
		nullptr,
		nullptr,
		resStr,
		this,
		!m_cGpsRoute.empty());

	std::wstring strOpen = CWToolsString::Load(IDS_BT_OPEN);
	stdx::wstring_helper::remove(strOpen, '&');
	cFileDialog.m_ofn.lpstrTitle = strOpen.c_str();

	if (cFileDialog.DoModal() == IDOK)
		OpenFile(cFileDialog.GetPathName(), cFileDialog.GetAppendState(), false);
}

void CITNConverterDlg::OnExport()
{
	const FileFormatDesc* pfileFormatDesc = reinterpret_cast<const FileFormatDesc*>(m_cbExportExt.GetItemData(m_cbExportExt.GetCurSel()));
	if (!pfileFormatDesc)
		return;

	CString tmpStr;
	CString resStr;

	resStr.LoadString(pfileFormatDesc->nFilter);
	tmpStr.LoadString(IDS_ALLFILTERS);
	resStr += tmpStr;

	// Update route name
	m_EditName.GetWindowText(tmpStr);
	m_cGpsRoute.name(stdx::wstring_helper::to_utf8((LPCTSTR)tmpStr.Left(250)));
	if (m_cGpsRoute.name().empty())
		m_cGpsRoute.name(stdx::wstring_helper::to_utf8(CWToolsString::Load(IDS_ITINERARY)));

	// File name
	std::wstring fileName = CWToolsString::FileTitle(m_strPathName);
	if (fileName.empty())
		fileName = stdx::wstring_helper::from_utf8(CToolsString::FileTitle(m_cGpsRoute.name()));

	// Don't open the file dialog for Google Maps, Here Maps or TomTom Home export
	if (wcscmp(pfileFormatDesc->szFileExt, TYPE_GGM)
		&& wcscmp(pfileFormatDesc->szFileExt, TYPE_HERE)
		&& wcscmp(pfileFormatDesc->szFileExt, TYPE_VMM)
		&& wcscmp(pfileFormatDesc->szFileExt, TYPE_TTH)
		&& wcscmp(pfileFormatDesc->szFileExt, TYPE_TTD))
	{
		const std::vector<std::wstring>& targetFolders = getTargetFolders(pfileFormatDesc->szFileExt);
		if (!targetFolders.empty())
		{
			std::vector<wchar_t> vecTmpBuffer(1024); // 1 Ko
			DWORD dwSize = 0;

			do
			{
				dwSize = GetLogicalDriveStringsW(vecTmpBuffer.size(), vecTmpBuffer.data());
				if (dwSize == vecTmpBuffer.size() - 1)
					vecTmpBuffer.resize(vecTmpBuffer.size() * 2);
			} while (dwSize == vecTmpBuffer.size() - 1);

			DWORD index = 0;
			bool bFinded = false;

			while (index < dwSize && !bFinded)
			{
				std::wstring driveName(&vecTmpBuffer[index]);
				index += driveName.size() + 1;

				if (!driveName.empty() && GetDriveTypeW(driveName.c_str()) == DRIVE_REMOVABLE)
				{
					// Looking for target folder
					for (const std::wstring& targetFolder : targetFolders)
					{
						std::wstring targetFolderPath(driveName + targetFolder);

						DWORD dwAttributes = GetFileAttributesW(targetFolderPath.c_str());
						if (dwAttributes != INVALID_FILE_ATTRIBUTES && (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
						{
							fileName = targetFolderPath + L"\\" + fileName;
							bFinded = true; // Exit from parent while loop
							break;
						}
					}
				}
			}
		}

		CFileDialog cFileDialog(
			FALSE,
			pfileFormatDesc->szFileExt,
			fileName.c_str(),
			OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
			resStr,
			this);

		std::wstring strExport = CWToolsString::Load(IDS_BT_EXPORT);
		stdx::wstring_helper::remove(strExport, '&');
		cFileDialog.m_ofn.lpstrTitle = strExport.c_str();

		if (cFileDialog.DoModal() == IDOK)
			ExportFile(cFileDialog.GetPathName(), *pfileFormatDesc, false);
	}
	else
	{
		ExportFile(fileName.c_str(), *pfileFormatDesc, false);
	}
}

bool CITNConverterDlg::CheckModified()
{
	int iRet = IDYES;

	if (m_cGpsRoute.IsModified())
		iRet = AfxMessageBox(IDS_CONFIRM_CLOSE, MB_YESNO | MB_ICONEXCLAMATION);

	return (iRet == IDYES);
}

void CITNConverterDlg::Clear()
{
	m_cGpsRoute.clear();
	m_strPathName.clear();
}

int CITNConverterDlg::OpenFile(const CString& sFileName, bool bAppend, bool bCmdLine)
{
	ScopedWaitCursor swc(*this);

	HRESULT hr = ERROR_BAD_FORMAT;

	// Find file extension
	CString resStr = CWToolsString::FileExt((LPCTSTR)sFileName).c_str();

	// Find file function
	for (auto it = m_FileFormats.begin(); it != m_FileFormats.end(); ++it)
	{
		if (!resStr.CompareNoCase(it->szFileExt) && it->pReadFile)
		{
			hr = ReadFile(it->pReadFile, sFileName, bAppend, bCmdLine);
			if (hr == S_OK || hr == ERROR_CANCELLED || hr == ERROR_SHARING_VIOLATION || hr == STG_E_SHAREVIOLATION)
				break;
		}
	}

	return CheckReadFile(sFileName, bAppend, hr);
}

int CITNConverterDlg::ReadFile(_ReadFile* pReadFile, const CString& sFileName, bool bAppend, bool bCmdLine)
{
	HRESULT hr = ERROR_BAD_FORMAT;
	std::vector<CGpsPointArray*> vecGpsArray;
	std::vector<CGpsPointArray*>::iterator it;

	stdx::function_guard release([&]()
		{
			// Delete Array list
			for (it = vecGpsArray.begin(); it != vecGpsArray.end(); ++it)
				delete* it;
		});

	hr = pReadFile((LPCTSTR)sFileName, vecGpsArray, bCmdLine);
	if (hr != S_OK)
		return hr;

	// Remove empties
	for (it = vecGpsArray.begin(); it != vecGpsArray.end(); ++it)
	{
		if ((*it)->empty())
		{
			delete* it;
			*it = nullptr;
		}
	}
	vecGpsArray.resize(std::remove(vecGpsArray.begin(), vecGpsArray.end(), static_cast<CGpsPointArray*>(nullptr)) - vecGpsArray.begin());

	if (!vecGpsArray.empty())
	{
		CChooseArrayDlg dlg(CWToolsString::FileName((LPCTSTR)sFileName), vecGpsArray);
		if (dlg.DoModal() == IDOK)
		{
			if (!bAppend)
				Clear();
			m_ListPoint.DeleteAllItems();
			m_EditName.SetWindowText(_T(""));

			m_LabelFileName.ShowWindow(SW_HIDE);
			m_LabelFileName.SetWindowText(_T(""));
			m_LabelFileName.ShowWindow(SW_SHOW);

			m_LabelPointNumber.ShowWindow(SW_HIDE);
			m_LabelPointNumber.SetWindowText(_T(""));
			m_LabelPointNumber.ShowWindow(SW_SHOW);

			GetDlgItem(IDEXPORT)->EnableWindow(FALSE);

			std::vector<CGpsPointArray*>::const_iterator cit;
			const std::vector<CGpsPointArray*>& vecSelectedArray = dlg.GetSelectedArray();

			for (cit = vecSelectedArray.begin(); cit != vecSelectedArray.end(); ++cit)
			{
				m_cGpsRoute += **cit;
				if (m_cGpsRoute.name().empty() && !(*cit)->name().empty())
					m_cGpsRoute.name((*cit)->name());
			}
		}
		else
		{
			hr = IDABORT;
		}
	}
	else
	{
		hr = ERROR_BAD_FORMAT;
	}

	return hr;
}

int CITNConverterDlg::CheckReadFile(const CString& sFileName, bool bAppend, HRESULT hr)
{
	if (hr == S_OK)
	{
		if (m_cGpsRoute.size())
		{
			if (!bAppend)
			{
				m_strPathName = sFileName;

				if (m_cGpsRoute.name().empty())
					m_cGpsRoute.name(stdx::wstring_helper::to_utf8(CWToolsString::FileTitle(m_strPathName)));
			}

			CString resStr;
			resStr.Format(IDS_FILENAME, m_strPathName.c_str());
			m_LabelFileName.SetWindowText(resStr);

			m_EditName.SetWindowText(stdx::wstring_helper::from_utf8(m_cGpsRoute.name()).c_str());

			m_cGpsRoute.ClearModified();
			m_ListPoint.Refresh();
			UpdateButtons();
		}
		else
		{
			// Pas de routes
			AfxMessageBox(IDS_ERRITN, MB_OK | MB_ICONEXCLAMATION);
			hr = S_FALSE;
		}
	}
	else if (hr != IDABORT)
	{
		// Mauvais fichier
		DisplayFileError(hr);
	}

	return hr;
}

int CITNConverterDlg::ExportFile(const CString& sFileName, const FileFormatDesc& fileFormat, bool bCmdLine)
{
	ScopedWaitCursor swc(*this);

	DWORD dwFlag = 0;

#ifdef IDC_EDIT_CUT_ITN
	if (!wcscmp(fileFormat.szFileExt, TYPE_ITN))
	{
		CString str;

		m_EditCutItn.GetWindowText(str);
		dwFlag = stdx::wstring_helper::string_to<DWORD>((LPCTSTR)str);
	}
	else
	{
		dwFlag = ((CButton*)GetDlgItem(IDC_CHECK_OPTION))->GetCheck();
	}
#endif

	HRESULT hr = (fileFormat.pWriteFile)((LPCTSTR)sFileName, m_cGpsRoute, dwFlag, bCmdLine);
	if (hr == S_OK)
		m_cGpsRoute.ClearModified();
	else
		AfxMessageBox(IDS_ERRSAVE, MB_OK | MB_ICONSTOP);

	return hr;
}

void CITNConverterDlg::OnChangeEditName()
{
	CString cstrName;
	m_EditName.GetWindowText(cstrName);

	if (isGoogleURL((LPCTSTR)cstrName) || isHereURL((LPCTSTR)cstrName) || isViaMichelinURL((LPCTSTR)cstrName) || isTomtomURL((LPCTSTR)cstrName))
	{
		if (AfxMessageBox(IDS_CONFIRM_READ_URL, MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;

		if (!CheckModified())
			return;

		ScopedWaitCursor swc(*this);
		HRESULT ret = S_FALSE;

		if (isGoogleURL((LPCTSTR)cstrName))
			ret = CheckReadFile(cstrName, false, ReadFile(ReadGoogleURL, cstrName, false, false));

		if (isHereURL((LPCTSTR)cstrName))
			ret = CheckReadFile(cstrName, false, ReadFile(ReadHereURL, cstrName, false, false));

		if (isViaMichelinURL((LPCTSTR)cstrName))
			ret = CheckReadFile(cstrName, false, ReadFile(ReadViaMichelinURL, cstrName, false, false));

		if (isTomtomURL((LPCTSTR)cstrName))
			ret = CheckReadFile(cstrName, false, ReadFile(ReadTomtomURL, cstrName, false, false));

		if (ret == S_OK)
		{
			m_strPathName.clear();

			m_EditName.SetWindowText(stdx::wstring_helper::from_utf8(m_cGpsRoute.name()).c_str());
			m_LabelFileName.SetWindowText(_T(""));
		}
	}
}

void CITNConverterDlg::OnSelchangeComboFileexport()
{
	const FileFormatDesc* pfileFormatDesc = reinterpret_cast<const FileFormatDesc*>(m_cbExportExt.GetItemData(m_cbExportExt.GetCurSel()));
	if (!pfileFormatDesc)
		return;

#ifdef IDC_STATIC_OPTIONS
	CString str;
	CString title;
	m_cbCutItn.ShowWindow(SW_HIDE);
	m_EditCutItn.ShowWindow(SW_HIDE);

	if (!wcscmp(pfileFormatDesc->szFileExt, TYPE_ITN))
	{
		m_cbCutItn.ShowWindow(SW_SHOW);
		m_EditCutItn.ShowWindow(SW_SHOW);

		title.LoadString(IDS_CUT_ITN);
		OnSelchangeComboCutItn();
	}
	else if (pfileFormatDesc->nOption)
	{
		title.LoadString(IDS_OPTIONS);
		str.LoadString(pfileFormatDesc->nOption);
	}

	GetDlgItem(IDC_CHECK_OPTION)->SetWindowText(str);
	GetDlgItem(IDC_CHECK_OPTION)->ShowWindow(!str.IsEmpty());

	GetDlgItem(IDC_STATIC_OPTIONS)->SetWindowText(title);
	GetDlgItem(IDC_STATIC_OPTIONS)->ShowWindow(!title.IsEmpty());
#endif

	/* Save selection into the registry */
	CITNConverterApp::RegParam().ExportId() = m_cbExportExt.GetCurSel();
}

void CITNConverterDlg::OnSelchangeComboCutItn()
{
	if (m_cbCutItn.GetCurSel() < 4)
	{
		m_EditCutItn.SetWindowText(stdx::wformat(_T("%d"))(m_cbCutItn.GetItemData(m_cbCutItn.GetCurSel())).str().c_str());
		m_EditCutItn.EnableWindow(FALSE);
	}
	else
	{
		m_EditCutItn.EnableWindow(TRUE);
	}
}

void CITNConverterDlg::UpdateFileExportCombo()
{
	std::vector<std::vector<FileFormatDesc>::const_iterator> vecTabFileIndex;
	for (std::vector<FileFormatDesc>::const_iterator it = m_FileFormats.begin(); it != m_FileFormats.end(); ++it)
	{
		if (it->nFilter && it->pWriteFile)
			vecTabFileIndex.push_back(it);
	}

	if (alphabeticalSortFileFormats())
	{
		// Alphabetical sort
		std::sort(vecTabFileIndex.begin(), vecTabFileIndex.end(), [&](
			const std::vector<FileFormatDesc>::const_iterator& it1,
			const std::vector<FileFormatDesc>::const_iterator& it2) -> bool
			{
				std::wstring str1;
				std::wstring str2;

				CWToolsString::Load(it1->nFilter, str1);
				CWToolsString::Load(it2->nFilter, str2);

				stdx::wstring_helper::tolower(str1);
				stdx::wstring_helper::tolower(str2);

				return str1 < str2;
			});
	}

	/* Clear combo box */
	m_cbExportExt.ResetContent();

	// Exported file type selection
	CString resStr;
	for (auto it = vecTabFileIndex.begin(); it != vecTabFileIndex.end(); ++it)
	{
		const FileFormatDesc& fileFormatDesc = **it;
		if (fileFormatDesc.szFileExt && fileFormatDesc.nFilter)
		{
			resStr.LoadString(fileFormatDesc.nFilter);
			resStr = resStr.Left(resStr.Find('|'));
			m_cbExportExt.SetItemData(m_cbExportExt.AddString(resStr), reinterpret_cast<DWORD>(&fileFormatDesc)); // Only works in 32 bits !!
		}
	}

	int nCurSel = m_cbExportExt.GetCurSel();
	if (nCurSel == CB_ERR)
	{
		/* Read selection from registry */
		nCurSel = CITNConverterApp::RegParam().ExportId();
		if (nCurSel < 0 || nCurSel >= m_cbExportExt.GetCount())
			nCurSel = DEFAULT_EXT;
	}

	m_cbExportExt.SetCurSel(nCurSel);
	OnSelchangeComboFileexport();
}

void CITNConverterDlg::UpdateButtons(int nIndex)
{
	CString resStr;
	size_t ulWayPointNumber = m_cGpsRoute.size();

	/* Update buttons states */
	if (nIndex == -1)
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow((nIndex > 0 && m_ListPoint.GetSelectedCount() == 1) ? TRUE : FALSE);
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow((static_cast<size_t>(nIndex) < m_cGpsRoute.upper_bound() && m_ListPoint.GetSelectedCount() == 1) ? TRUE : FALSE);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(TRUE);
	}

	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(m_ListPoint.GetSelectedCount() < 2 ? TRUE : FALSE);
	GetDlgItem(IDC_BUTTON_INVERT)->EnableWindow((ulWayPointNumber > 1) ? TRUE : FALSE);
	GetDlgItem(IDEXPORT)->EnableWindow(ulWayPointNumber ? TRUE : FALSE);

	/* Refresh point number label */
	resStr.Format(IDS_WAYPOINTS_NB, ulWayPointNumber);

	m_LabelPointNumber.SetWindowText(resStr);

	/* Refresh area */
	RECT rect;
	m_LabelPointNumber.GetWindowRect(&rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect);
}

void CITNConverterDlg::OnButtonUp()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Up();
}

void CITNConverterDlg::OnButtonDown()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Down();
}

void CITNConverterDlg::OnButtonAdd()
{
	ScopedWaitCursor swc(*this);

	CGpsPoint cGpsPoint;
	CInsertModify dlgInsert;

	if (dlgInsert.DoModal(cGpsPoint) == IDOK && cGpsPoint)
	{
		POSITION pos = m_ListPoint.GetFirstSelectedItemPosition();
		if (pos)
			m_ListPoint.Insert(m_ListPoint.GetNextSelectedItem(pos), cGpsPoint);
		else
			m_ListPoint.Add(cGpsPoint);
	}
}

void CITNConverterDlg::OnButtonRemove()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Remove();
}

void CITNConverterDlg::OnButtonInvert()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Invert();
}

void CITNConverterDlg::OnButtonModify()
{
	ScopedWaitCursor swc(*this);

	POSITION pos = m_ListPoint.GetFirstSelectedItemPosition();
	while (pos)
	{
		if (CInsertModify().DoModal(m_cGpsRoute[m_ListPoint.GetNextSelectedItem(pos)]) == IDOK)
			m_ListPoint.Modify();
	}
}

void CITNConverterDlg::OnDblclkList(NMHDR*, LRESULT* pResult)
{
	OnButtonModify();

	*pResult = 0;
}

void CITNConverterDlg::OnRclickList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE lpNMItem = (LPNMITEMACTIVATE)pNMHDR;

	if (lpNMItem->iItem != MAXDWORD)
	{
		POINT pCursorPos;
		GetCursorPos(&pCursorPos);
		m_PopupMenu.TrackPopupMenu(TPM_RIGHTBUTTON, pCursorPos.x, pCursorPos.y, this);
	}

	*pResult = 0;
}

void CITNConverterDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_ListPoint.GetSelectedCount())
		UpdateButtons(pNMListView->iItem);
	else
		UpdateButtons();

	*pResult = 0;
}

void CITNConverterDlg::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	switch (pLVKeyDow->wVKey)
	{
	case VK_RETURN:
		break;

	case VK_INSERT:
		OnButtonAdd();
		break;

	case VK_SUBTRACT:
		OnButtonUp();
		break;

	case VK_ADD:
		OnButtonDown();
		break;

	case VK_DELETE:
		OnButtonRemove();
		break;

	default:
		break;
	}

	*pResult = 0;
}

void CITNConverterDlg::OnButtonVirtualEarth()
{
	ScopedWaitCursor swc(*this);

	POSITION pos = m_ListPoint.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = m_ListPoint.GetNextSelectedItem(pos);
		const CGpsPoint& cGpsPoint = m_cGpsRoute[nIndex];

		std::string strUrl = stdx::format(CToolsString::Load(IDS_MSN_VE_LINK))
			(cGpsPoint.lat())
			(cGpsPoint.lng())
			(cGpsPoint.lat())
			(cGpsPoint.lng())
			(nIndex + 1)
			(stdx::uri_helper::escape(cGpsPoint.name()));

		ShellExecuteA(nullptr, "open", strUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	}
}

void CITNConverterDlg::OnButtonTomtomMaps()
{
	ScopedWaitCursor swc(*this);

	POSITION pos = m_ListPoint.GetFirstSelectedItemPosition();
	while (pos)
	{
		const CGpsPoint& cGpsPoint = m_cGpsRoute[m_ListPoint.GetNextSelectedItem(pos)];

		std::ostringstream ossLatLng;
		ossLatLng << cGpsPoint.lat() << ',' << cGpsPoint.lng();

		std::ostringstream ossLocation;
		if (!cGpsPoint.name().empty())
			ossLocation << cGpsPoint.name() << "@";
		ossLocation << ossLatLng.str() << "@-1";

		std::string strUrl = stdx::format(CToolsString::Load(IDS_TOMTOM_MAPS_LINK))
			(stdx::uri_helper::escape(ossLocation.str()))
			(stdx::uri_helper::escape(ossLatLng.str()));

		ShellExecuteA(nullptr, "open", strUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	}
}

void CITNConverterDlg::OnButtonGoogleMaps()
{
	ScopedWaitCursor swc(*this);

	POSITION pos = m_ListPoint.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = m_ListPoint.GetNextSelectedItem(pos);
		const CGpsPoint& cGpsPoint = m_cGpsRoute[nIndex];

		std::string strUrl = stdx::format(CToolsString::Load(IDS_GOOGLE_MAPS_LINK))
			(nIndex + 1)
			(cGpsPoint.lat())
			(cGpsPoint.lng())
			(stdx::uri_helper::escape(cGpsPoint.name()))
			(stdx::uri_helper::escape(cGpsPoint.comment()));

		ShellExecuteA(nullptr, "open", strUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	}
}

void CITNConverterDlg::OnButtonHereMaps()
{
	ScopedWaitCursor swc(*this);

	POSITION pos = m_ListPoint.GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = m_ListPoint.GetNextSelectedItem(pos);
		const CGpsPoint& cGpsPoint = m_cGpsRoute[nIndex];

		std::string strUrl = stdx::format(CToolsString::Load(IDS_HERE_MAPS_LINK))
			(cGpsPoint.lat())
			(cGpsPoint.lng())
			(stdx::uri_helper::escape(cGpsPoint.name()));

		ShellExecuteA(nullptr, "open", strUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
	}
}

void CITNConverterDlg::OnButtonSendToGps(CSendToGps::E_GPS eGps)
{
	ScopedWaitCursor swc(*this);

	POSITION pos = m_ListPoint.GetFirstSelectedItemPosition();
	while (pos)
		CSendToGps::SendTo(eGps, m_cGpsRoute[m_ListPoint.GetNextSelectedItem(pos)]);
}

BOOL CITNConverterDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (nCode == CN_COMMAND)
	{
		switch (nID)
		{
		case WM_MODIFY:
			OnButtonModify();
			return TRUE;

		case WM_REMOVE:
			OnButtonRemove();
			return TRUE;

		case WM_TOMTOM_MAP_VIEW:
			OnButtonTomtomMaps();
			return TRUE;

		case WM_GOOGLE_MAP_VIEW:
			OnButtonGoogleMaps();
			return TRUE;

		case WM_VIRTUAL_EARTH_VIEW:
			OnButtonVirtualEarth();
			return TRUE;

		case WM_HERE_MAP_VIEW:
			OnButtonHereMaps();
			return TRUE;

		case WM_SEND_TO_NVGFRESH:
			OnButtonSendToGps(CSendToGps::E_GPS_NAVIGON);
			return TRUE;

		case WM_SEND_TO_PIONEER:
			OnButtonSendToGps(CSendToGps::E_GPS_PIONEER);
			return TRUE;

		case WM_SEND_TO_MIOMORE:
			OnButtonSendToGps(CSendToGps::E_GPS_MIOMORE);
			return TRUE;

		case WM_SEND_TO_GARMIN:
			OnButtonSendToGps(CSendToGps::E_GPS_GARMIN);
			return TRUE;

		default:
			break;
		}
	}

	return CSizeableDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CITNConverterDlg::OnButtonEditor()
{
	bool bEditor = true;

	if (m_cGpsRoute.size() > 300)
		bEditor = (AfxMessageBox(IDS_TOOMANYPOINTS, MB_YESNO | MB_ICONEXCLAMATION) == IDYES);

	if (bEditor)
	{
		ScopedWaitCursor swc(*this);

		CEditorDlg(m_cGpsRoute).DoModal();

		/* Update route name */
		m_EditName.SetWindowText(stdx::wstring_helper::from_utf8(m_cGpsRoute.name()).c_str());

		/* Update list */
		m_ListPoint.Refresh();
		UpdateButtons();
	}
}

void CITNConverterDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_ListPoint.MouseMove(nFlags, point);
	CSizeableDialog::OnMouseMove(nFlags, point);
}

void CITNConverterDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_ListPoint.LButtonUp(nFlags, point);
	CSizeableDialog::OnLButtonUp(nFlags, point);
}
