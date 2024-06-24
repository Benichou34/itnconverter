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

#include "stdafx.h"
#include <mshtml.h>
#include "ITN Tools.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/HttpClient.h"
#include "Storage/Registry.h"
#include "GeoServices/GeoServices.h"
#include "jsonParser/JsonParser.h"
#include "stdx/uri_helper.h"
#include "sendtogps.h"
#include "AboutDlg.h"
#include "EditorDlg.h"
#include "ConfigDlg.h"
#include "Variant.h"

namespace
{
	CGpsPointView::RVCOLUMN sTabRoadColumn[] =
	{
		{ IDS_EMPTY    , LVCFMT_LEFT, 20, CGpsPointView::E_RVCOL_NUMBER  },
		{ IDS_ITINERARY, LVCFMT_LEFT, -1, CGpsPointView::E_RVCOL_ADDRESS },
		{ 0            , 0          ,  0, CGpsPointView::E_RVCOL_ADDRESS }
	};

	CGpsPointView::RVCOLUMN sTabSearchColumn[] =
	{
		{ IDS_SEARCH_RESULTS, LVCFMT_LEFT, -1, CGpsPointView::E_RVCOL_ADDRESS },
		{ 0                 , 0          ,  0, CGpsPointView::E_RVCOL_ADDRESS }
	};

	CGpsPointView::RVCOLUMN sTabFavoriteColumn[] =
	{
		{ IDS_FAVORITES, LVCFMT_LEFT, -1, CGpsPointView::E_RVCOL_ADDRESS },
		{ 0            , 0          ,  0, CGpsPointView::E_RVCOL_ADDRESS }
	};
}

#define URL_JS_MAPS     _T(BASE_URL) _T("gglplanner.html")
#define URL_JAVASCRIPT  _T("javascript:")
#define URL_SIGNAL      _T("signal:")
#define URL_ABOUT_BLANK _T("about:blank")

/* Context menu */
#define WM_IMPORT_FAV      WM_USER+2001
#define WM_EXPORT_FAV      WM_USER+2002
#define WM_CLEAR_FAV       WM_USER+2003

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg dialog

CEditorDlg::CEditorDlg(CGpsPointArray& cGpsPointArray, CWnd* pParent /*=nullptr*/)
	: CSizeableDialog(CEditorDlg::IDD, pParent), m_cGpsPointArray(cGpsPointArray)
{
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//{{AFX_DATA_INIT(CEditorDlg)
	m_csSearch = _T("");
	//}}AFX_DATA_INIT
}

void CEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CSizeableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditorDlg)
	DDX_Control(pDX, IDC_DISTANCE, m_LabelDistance);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_LabelStatus);
	DDX_Control(pDX, IDC_LIST_SEARCH, m_ListSearch);
	DDX_Control(pDX, IDC_LIST_PROXIMITY, m_ListProximity);
	DDX_Control(pDX, IDC_LIST_FAVORITES, m_ListFavorites);
	DDX_Control(pDX, IDC_POINT_NUMBER, m_LabelPointNumber);
	DDX_Control(pDX, IDC_LIST, m_ListPoint);
	DDX_Control(pDX, IDC_EDIT_NAME, m_EditName);
	DDX_Control(pDX, IDC_EXPLORER, m_Navigator);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_btSearch);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_EditSearch);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_csSearch);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditorDlg, CSizeableDialog)
	//{{AFX_MSG_MAP(CEditorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, OnItemchangedList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnKeydownList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SEARCH, OnDblclkListSearch)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SEARCH, OnItemchangedListSearch)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROXIMITY, OnDblclkListProximity)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FAVORITES, OnDblclkListFavorites)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FAVORITES, OnRclickListFavorites)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_FAVORITES, OnItemchangedListFavorites)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_INVERT, OnButtonInvert)
	ON_BN_CLICKED(IDC_BUTTON_VIEWALL, OnButtonViewall)
	ON_BN_CLICKED(IDC_BUTTON_CALCULATE, OnButtonCalculate)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, OnSearch)
	ON_BN_CLICKED(IDC_BUTTON_DELROUTE, OnButtonDelroute)
	ON_BN_CLICKED(IDC_BUTTON_REMOVEALL, OnButtonRemoveall)
	ON_BN_CLICKED(IDC_BUTTON_OPTIMISE, OnButtonOptimise)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, OnButtonConfig)
	ON_BN_CLICKED(IDC_SEARCH_LOCATION, OnSearchLocation)
	ON_BN_CLICKED(IDC_SEARCH_PROXIMITY, OnSearchProximity)
	ON_BN_CLICKED(IDC_DISPLAY_FAVORITE, OnDisplayFavorites)
#ifdef IDC_BUTTON_PRINT
	ON_BN_CLICKED(IDC_BUTTON_PRINT, OnButtonPrint)
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CEditorDlg, CSizeableDialog)
	//{{AFX_EVENTSINK_MAP(CEditorDlg)
	ON_EVENT(CEditorDlg, IDC_EXPLORER, 250 /* BeforeNavigate2 */, OnBeforeNavigate2Explorer, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CEditorDlg, IDC_EXPLORER, 271 /* NavigateError */, OnNavigateErrorExplorer, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CEditorDlg, IDC_EXPLORER, 108 /* ProgressChange */, OnProgressChangeExplorer, VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorDlg message handlers

BOOL CEditorDlg::OnInitDialog()
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

	// Change title
	SetWindowText(stdx::string_helper::widen(SOFT_FULL_NAME).c_str());

	// List of points
	m_ListPoint.Init(m_cGpsPointArray, sTabRoadColumn, true);
	m_ListPoint.SetControls(&m_LabelDistance, &m_LabelStatus);
	m_ptabList[TAB_LIST_WAYPOINT] = &m_ListPoint;

	// List of address points for search result
	m_ListSearch.Init(m_cLocationResult, sTabSearchColumn);
	m_ptabList[TAB_LIST_SEARCH] = &m_ListSearch;

	m_ListProximity.Init(m_cProximityResult, sTabSearchColumn);
	m_ptabList[TAB_LIST_PROXIMITY] = &m_ListProximity;

	m_ListFavorites.Init(m_cFavorites, sTabFavoriteColumn);
	m_ptabList[TAB_LIST_FAVORITE] = &m_ListFavorites;
	CITNConverterApp::RegParam().ReadFavorites(m_cFavorites);
	m_cFavorites.name(CToolsString::Load(IDS_FAVORITES));
	m_ListFavorites.Refresh();

	// Icons
#ifndef IDB_CALCULATE_E_BT_ON
	UpdateCalculateButton();
	((CButton*)GetDlgItem(IDC_BUTTON_CALCULATE))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstCalculate.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstCalculate.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 40, 0 });
	m_bstCalculate.DrawBorder(FALSE);
	m_bstCalculate.SetWindowText(CWToolsString::Load(IDS_BT_CALCULATE).c_str());
	m_bstCalculate.SetBitmaps(IDB_CALCULATE_E_BT_ON, 0, IDB_CALCULATE_E_BT_OFF);
	m_bstCalculate.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
	m_bstCalculate.SizeToContent();
#endif

#ifndef IDB_SEARCHLOCATION_E_BT_ON
	((CButton*)GetDlgItem(IDC_SEARCH_LOCATION))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_LOCATION)));
	((CButton*)GetDlgItem(IDC_SEARCH_LOCATION))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstSearchLocation.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstSearchLocation.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstSearchLocation.DrawBorder(FALSE);
	m_bstSearchLocation.SetWindowText(CWToolsString::Load(IDS_BT_SEARCHLOCATION).c_str());
	m_bstSearchLocation.SetBitmaps(IDB_SEARCHLOCATION_E_BT_ON, 0, IDB_SEARCHLOCATION_E_BT_OFF);
	m_bstSearchLocation.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
	m_bstSearchLocation.SizeToContent();
#endif

#ifndef IDB_SEARCHPROXIMITY_E_BT_ON
	((CButton*)GetDlgItem(IDC_SEARCH_PROXIMITY))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PROXIMITY)));
	((CButton*)GetDlgItem(IDC_SEARCH_PROXIMITY))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstSearchProximity.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstSearchProximity.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstSearchProximity.DrawBorder(FALSE);
	m_bstSearchProximity.SetWindowText(CWToolsString::Load(IDS_BT_SEARCHPROXIMITY).c_str());
	m_bstSearchProximity.SetBitmaps(IDB_SEARCHPROXIMITY_E_BT_ON, 0, IDB_SEARCHPROXIMITY_E_BT_OFF);
	m_bstSearchProximity.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
	m_bstSearchProximity.SizeToContent();
#endif

#ifndef IDB_FAVORITE_E_BT_ON
	((CButton*)GetDlgItem(IDC_DISPLAY_FAVORITE))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_FAVORITE)));
	((CButton*)GetDlgItem(IDC_DISPLAY_FAVORITE))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstDisplayFavorites.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstDisplayFavorites.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstDisplayFavorites.DrawBorder(FALSE);
	m_bstDisplayFavorites.SetWindowText(CWToolsString::Load(IDS_FAVORITES).c_str());
	m_bstDisplayFavorites.SetBitmaps(IDB_FAVORITE_E_BT_ON, 0, IDB_FAVORITE_E_BT_OFF);
	m_bstDisplayFavorites.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
	m_bstDisplayFavorites.SizeToContent();
#endif

#ifndef IDB_UP_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_UP))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_UP)));
	((CButton*)GetDlgItem(IDC_BUTTON_UP))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstUp.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstUp.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstUp.DrawBorder(FALSE);
	m_bstUp.SetWindowText(CWToolsString::Load(IDS_UP).c_str());
	m_bstUp.SetBitmaps(IDB_UP_E_BT_ON, 0, IDB_UP_E_BT_OFF);
	m_bstUp.SizeToContent();
#endif

#ifndef IDB_DOWN_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_DOWN))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_DOWN)));
	((CButton*)GetDlgItem(IDC_BUTTON_DOWN))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstDown.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstDown.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstDown.DrawBorder(FALSE);
	m_bstDown.SetWindowText(CWToolsString::Load(IDS_DOWN).c_str());
	m_bstDown.SetBitmaps(IDB_DOWN_E_BT_ON, 0, IDB_DOWN_E_BT_OFF);
	m_bstDown.SizeToContent();
#endif

#ifndef IDB_DEL_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_REMOVE))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_REMOVE)));
	((CButton*)GetDlgItem(IDC_BUTTON_REMOVE))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstDel.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstDel.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstDel.DrawBorder(FALSE);
	m_bstDel.SetWindowText(CWToolsString::Load(IDS_REMOVE).c_str());
	m_bstDel.SetBitmaps(IDB_DEL_E_BT_ON, 0, IDB_DEL_E_BT_OFF);
	m_bstDel.SizeToContent();
#endif

#ifndef IDB_INVERT_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_INVERT))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_INVERT)));
	((CButton*)GetDlgItem(IDC_BUTTON_INVERT))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstInvert.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstInvert.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstInvert.DrawBorder(FALSE);
	m_bstInvert.SetWindowText(CWToolsString::Load(IDS_INVERT).c_str());
	m_bstInvert.SetBitmaps(IDB_INVERT_E_BT_ON, 0, IDB_INVERT_E_BT_OFF);
	m_bstInvert.SizeToContent();
#endif

#ifndef IDB_CLEAR_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_REMOVEALL))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_REMOVEALL)));
	((CButton*)GetDlgItem(IDC_BUTTON_REMOVEALL))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstClearAll.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstClearAll.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstClearAll.DrawBorder(FALSE);
	m_bstClearAll.SetWindowText(CWToolsString::Load(IDS_BT_REMOVEALL).c_str());
	m_bstClearAll.SetBitmaps(IDB_CLEAR_E_BT_ON, 0, IDB_CLEAR_E_BT_OFF);
	m_bstClearAll.SizeToContent();
#endif

#ifndef IDB_CLEARROUTE_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_DELROUTE))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_CLEAR_CAR)));
	((CButton*)GetDlgItem(IDC_BUTTON_DELROUTE))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstClearRoute.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstClearRoute.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstClearRoute.DrawBorder(FALSE);
	m_bstClearRoute.SetWindowText(CWToolsString::Load(IDS_BT_CLEARROUTE).c_str());
	m_bstClearRoute.SetBitmaps(IDB_CLEARROUTE_E_BT_ON, 0, IDB_CLEARROUTE_E_BT_OFF);
	m_bstClearRoute.SizeToContent();
#endif

#ifndef IDB_VIEWALL_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_VIEWALL))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_EARTH)));
	((CButton*)GetDlgItem(IDC_BUTTON_VIEWALL))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstViewAll.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstViewAll.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstViewAll.DrawBorder(FALSE);
	m_bstViewAll.SetWindowText(CWToolsString::Load(IDS_BT_VIEWALL).c_str());
	m_bstViewAll.SetBitmaps(IDB_VIEWALL_E_BT_ON, 0, IDB_VIEWALL_E_BT_OFF);
	m_bstViewAll.SizeToContent();
#endif

#ifndef IDB_OPTIMISE_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_OPTIMISE))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_OPTIMISE)));
	((CButton*)GetDlgItem(IDC_BUTTON_OPTIMISE))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstOptim.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstOptim.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstOptim.DrawBorder(FALSE);
	m_bstOptim.SetWindowText(CWToolsString::Load(IDS_BT_OPTIMISE).c_str());
	m_bstOptim.SetBitmaps(IDB_OPTIMISE_E_BT_ON, 0, IDB_OPTIMISE_E_BT_OFF);
	m_bstOptim.SizeToContent();
#endif

#ifndef IDB_CONFIG_E_BT_ON
	((CButton*)GetDlgItem(IDC_BUTTON_CONFIG))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_CONFIG)));
	((CButton*)GetDlgItem(IDC_BUTTON_CONFIG))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
#else
	m_bstConfig.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstConfig.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 30, 0 });
	m_bstConfig.DrawBorder(FALSE);
	m_bstConfig.SetWindowText(CWToolsString::Load(IDS_BT_CONFIG).c_str());
	m_bstConfig.SetBitmaps(IDB_CONFIG_E_BT_ON, 0, IDB_CONFIG_E_BT_OFF);
	m_bstConfig.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
	m_bstConfig.SizeToContent();
#endif

#ifdef IDC_BUTTON_PRINT
	((CButton*)GetDlgItem(IDC_BUTTON_PRINT))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_PRINT)));
	((CButton*)GetDlgItem(IDC_BUTTON_PRINT))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_PRINT), IDS_TOOLTIP_PRINT);
#endif

#ifndef IDB_GOBACK_E_BT_ON
	((CButton*)GetDlgItem(IDOK))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_GOBACK)));
	((CButton*)GetDlgItem(IDOK))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
	AddSizeableControl(GetDlgItem(IDOK), E_RESIZE_PROP_LEFT | E_RESIZE_PROP_REDRAW);
#else
	m_bstGoBack.SetAlign(CButtonST::ST_ALIGN_OVERLAP);
	m_bstGoBack.SetTextPosition(CButtonST::ST_ALIGN_TEXT_LEFT, CButtonST::ST_ALIGN_TEXT_MIDDLE, { 35, 0 });
	m_bstGoBack.DrawBorder(FALSE);
	m_bstGoBack.SetWindowText(CWToolsString::Load(IDS_BT_GOBACK).c_str());
	m_bstGoBack.SetBitmaps(IDB_GOBACK_E_BT_ON, 0, IDB_GOBACK_E_BT_OFF);
	m_bstGoBack.SetColor(CButtonST::BTNST_COLOR_FG, RGB(0xFF, 0xFF, 0xFF));
	m_bstGoBack.SizeToContent();
#endif

#ifdef IDB_SEARCH_SEL
	m_btSearch.SetBitmaps(IDB_SEARCH, 0, 0, 0, IDB_SEARCH_DISABLED, 0, IDB_SEARCH_SEL, 0);
#else
	m_btSearch.DrawTransparent();
	m_btSearch.SetBitmaps(IDB_SEARCH, RGB(244, 244, 244), 0, 0, IDB_SEARCH_DISABLED, RGB(244, 244, 244));
#endif
	m_btSearch.DrawBorder(FALSE);
	m_btSearch.SizeToContent();

	((CButton*)GetDlgItem(IDC_SEARCH_LOCATION))->SetCheck(TRUE);

	// Add toolTips to buttons
	m_tooltip.AddTool(GetDlgItem(IDC_SEARCH_LOCATION), IDS_SEARCH_LOCATION);
	m_tooltip.AddTool(GetDlgItem(IDC_SEARCH_PROXIMITY), IDS_SEARCH_PROXIMITY);
	m_tooltip.AddTool(GetDlgItem(IDC_DISPLAY_FAVORITE), IDS_FAVORITES);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_UP), IDS_TOOLTIP_UP);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_DOWN), IDS_TOOLTIP_DOWN);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_REMOVE), IDS_TOOLTIP_REMOVE);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_INVERT), IDS_TOOLTIP_INVERT);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_REMOVEALL), IDS_TOOLTIP_REMOVEALL);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_VIEWALL), IDS_TOOLTIP_VIEWALL);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_CALCULATE), IDS_TOOLTIP_CALCULATE);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_DELROUTE), IDS_TOOLTIP_CLEARROUTE);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_OPTIMISE), IDS_TOOLTIP_OPTIMISE);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_CONFIG), IDS_TOOLTIP_CFG_SETTINGS);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SEARCH), IDS_TOOLTIP_SEARCH);
	m_tooltip.AddTool(GetDlgItem(IDOK), IDS_TOOLTIP_VALIDATE);

	// Labels
	GetDlgItem(IDC_NAME)->SetWindowText(CWToolsString::Load(IDS_NAME).c_str());
	m_EditName.SetLimitText(100);

	GetDlgItem(IDC_STATIC_SEARCH)->SetWindowText(CWToolsString::Load(IDS_SEARCH).c_str());
	GetDlgItem(IDC_STATIC_RESULTS)->SetWindowText(_T(""));
	GetDlgItem(IDC_STATIC_PROXIMITY)->SetWindowText(CWToolsString::Load(IDS_SEARCH_NEAR_FROM).c_str());

	AddSizeableControl(&m_Navigator, E_RESIZE_PROP_WIDTH | E_RESIZE_PROP_HEIGHT);
	AddSizeableControl(&m_ListPoint, E_RESIZE_PROP_HEIGHT);
	AddSizeableControl(&m_LabelStatus, E_RESIZE_PROP_WIDTH | E_RESIZE_PROP_REDRAW);

	m_LabelStatus.SetWindowText(_T(""));
	m_LabelPointNumber.SetWindowText(_T(""));
	m_LabelDistance.SetWindowText(_T(""));
	m_EditName.SetWindowText(stdx::wstring_helper::from_utf8(m_cGpsPointArray.name()).c_str());

	m_EditSearch.SetEnterButton(m_btSearch);
	UpdateSearchArea();

	// Create Favorites Popup context menu
	m_FavPopupMenu.CreatePopupMenu();
	m_FavPopupMenu.AppendMenu(MF_STRING, WM_CLEAR_FAV, CWToolsString::Load(IDS_CLEAR_FAV).c_str());
	m_FavPopupMenu.AppendMenu(MF_STRING, WM_IMPORT_FAV, CWToolsString::Load(IDS_IMPORT_FAV).c_str());
	m_FavPopupMenu.AppendMenu(MF_STRING, WM_EXPORT_FAV, CWToolsString::Load(IDS_EXPORT_FAV).c_str());

	if (IsVistaOrSuperior())
	{
		// Bitmap
		m_bmpMenuArray[BMP_MENU_CLEAR].Attach(LoadTransparentBitmap(IDB_REMOVE_POPUP));
		m_bmpMenuArray[BMP_MENU_IMPORT].Attach(LoadTransparentBitmap(IDB_OPEN_POPUP));
		m_bmpMenuArray[BMP_MENU_EXPORT].Attach(LoadTransparentBitmap(IDB_SAVE_POPUP));

		m_FavPopupMenu.SetMenuItemBitmaps(WM_CLEAR_FAV, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_CLEAR], &m_bmpMenuArray[BMP_MENU_CLEAR]);
		m_FavPopupMenu.SetMenuItemBitmaps(WM_IMPORT_FAV, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_IMPORT], &m_bmpMenuArray[BMP_MENU_IMPORT]);
		m_FavPopupMenu.SetMenuItemBitmaps(WM_EXPORT_FAV, MF_BYCOMMAND, &m_bmpMenuArray[BMP_MENU_EXPORT], &m_bmpMenuArray[BMP_MENU_EXPORT]);
	}

	// Restore current windows position
	WINDOWPLACEMENT& wdPlacement = CITNConverterApp::RegParam().EditorPlacement();
	if (wdPlacement.length)
	{
		SetWindowPlacement(&wdPlacement);
	}
	else
	{
		CenterWindow();
	}

	CallNavigate();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CEditorDlg::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (nCode == CN_COMMAND)
	{
		switch (nID)
		{
		case WM_CLEAR_FAV:
			OnButtonClearFavorites();
			return TRUE;

		case WM_IMPORT_FAV:
			OnButtonImportFavorites();
			return TRUE;

		case WM_EXPORT_FAV:
			OnButtonExportFavorites();
			return TRUE;

		default:
			break;
		}
	}

	return CSizeableDialog::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (!ShowAboutBox(nID))
	{
		CSizeableDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEditorDlg::OnPaint()
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
		CSizeableDialog::OnPaint();
	}
}

void CEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CSizeableDialog::OnSize(nType, cx, cy);
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEditorDlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void CEditorDlg::CallNavigate()
{
#ifdef _DEBUG
	std::wstring strUrl(CITNConverterApp::RegParam().getStorageFolder()[_T("DebugUrl")](URL_JS_MAPS));
	SetWindowText(strUrl.c_str()); // Change title
#else
	std::wstring strUrl(URL_JS_MAPS);
#endif

#if defined(_DEBUG) || defined(FORCE_LOG)
	CITNConverterApp::wlog() << clock() << L": CallNavigate(" << strUrl << L")" << std::endl;
#endif //_DEBUG

	VARIANT flags = CVariant(static_cast<long>(navNoHistory | navNoReadFromCache | navNoWriteToCache)).variant();
	m_Navigator.Navigate(strUrl.c_str(), &flags, nullptr, nullptr, nullptr);
}

void CEditorDlg::UpdateSearchArea()
{
	std::wstring strTmp;

	if (((CButton*)GetDlgItem(IDC_SEARCH_LOCATION))->GetCheck())
	{
		m_ListSearch.ShowWindow(TRUE);
		m_ListProximity.ShowWindow(FALSE);
		m_ListFavorites.ShowWindow(FALSE);

		GetDlgItem(IDC_STATIC_SEARCH)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_SEARCH)->ShowWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEARCH)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_RESULTS)->ShowWindow(TRUE);

		CWToolsString::Load(IDS_SEARCH, strTmp);
		GetDlgItem(IDC_STATIC_SEARCH)->SetWindowText(strTmp.c_str());
		GetDlgItem(IDC_STATIC_PROXIMITY)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SEARCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);
	}
	else if (((CButton*)GetDlgItem(IDC_SEARCH_PROXIMITY))->GetCheck())
	{
		m_ListSearch.ShowWindow(FALSE);
		m_ListProximity.ShowWindow(TRUE);
		m_ListFavorites.ShowWindow(FALSE);

		GetDlgItem(IDC_STATIC_SEARCH)->ShowWindow(TRUE);
		GetDlgItem(IDC_EDIT_SEARCH)->ShowWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEARCH)->ShowWindow(TRUE);
		GetDlgItem(IDC_STATIC_RESULTS)->ShowWindow(TRUE);

		if (!m_cSelectedLocation)
		{
			CWToolsString::Load(IDS_NO_PROXIMITY, strTmp);
			GetDlgItem(IDC_EDIT_SEARCH)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(FALSE);
		}
		else
		{
			strTmp = stdx::wstring_helper::from_utf8(m_cSelectedLocation.name());
			GetDlgItem(IDC_EDIT_SEARCH)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);
		}

		GetDlgItem(IDC_STATIC_SEARCH)->SetWindowText(strTmp.c_str());
		GetDlgItem(IDC_STATIC_PROXIMITY)->ShowWindow(TRUE);
	}
	else// if(((CButton *)GetDlgItem(IDC_DISPLAY_FAVORITE))->GetCheck())
	{
		m_ListSearch.ShowWindow(FALSE);
		m_ListProximity.ShowWindow(FALSE);
		m_ListFavorites.ShowWindow(TRUE);

		GetDlgItem(IDC_STATIC_SEARCH)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_SEARCH)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON_SEARCH)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_PROXIMITY)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_RESULTS)->ShowWindow(FALSE);
	}

	UpdateResults();
}

void CEditorDlg::UpdateCalculateButton()
{
	UINT uiRessource;
	if (CITNConverterApp::RegParam().RouteProvider() == geo::E_GEO_PROVIDER_INTERNAL)
		uiRessource = IDB_ITI_LINES;
	else if (CITNConverterApp::RegParam().VehicleType() == geo::GeoVehicleType::Pedestrian)
		uiRessource = IDB_ITI_RANDO;
	else if (CITNConverterApp::RegParam().VehicleType() == geo::GeoVehicleType::Bike)
		uiRessource = IDB_ITI_BIKE;
	else if (CITNConverterApp::RegParam().VehicleType() == geo::GeoVehicleType::Truck)
		uiRessource = IDB_ITI_TRUCK;
	else if (CITNConverterApp::RegParam().VehicleType() == geo::GeoVehicleType::Motorbike)
		uiRessource = IDB_ITI_MOTORBIKE;
	else
		uiRessource = IDB_ITI_CAR;

	((CButton*)GetDlgItem(IDC_BUTTON_CALCULATE))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(uiRessource)));
}

void CEditorDlg::OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CString str;

	if (m_ListPoint.GetSelectedCount() && static_cast<size_t>(pNMListView->iItem) < m_cGpsPointArray.size())
	{
		m_cSelectedLocation = m_cGpsPointArray[pNMListView->iItem];
		UpdateButtons(pNMListView->iItem);
	}
	else
	{
		m_cSelectedLocation.clear();
		UpdateButtons();
	}

	UpdateSearchArea();
	*pResult = 0;
}

void CEditorDlg::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	switch (pLVKeyDow->wVKey)
	{
	case VK_RETURN:
		break;

	case VK_INSERT:
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

void CEditorDlg::UpdateButtons(int nIndex)
{
	CString resStr;
	size_t ulWayPointNumber = m_cGpsPointArray.size();

	if (nIndex == -1)
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow((nIndex > 0 && m_ListPoint.GetSelectedCount() == 1) ? TRUE : FALSE);
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow((static_cast<size_t>(nIndex) < m_cGpsPointArray.upper_bound() && m_ListPoint.GetSelectedCount() == 1) ? TRUE : FALSE);
		GetDlgItem(IDC_BUTTON_REMOVE)->EnableWindow(TRUE);
	}

	GetDlgItem(IDC_BUTTON_REMOVEALL)->EnableWindow((ulWayPointNumber > 0) ? TRUE : FALSE);
	GetDlgItem(IDC_BUTTON_INVERT)->EnableWindow((ulWayPointNumber > 1) ? TRUE : FALSE);
	GetDlgItem(IDC_BUTTON_CALCULATE)->EnableWindow((ulWayPointNumber > 1) ? TRUE : FALSE);
	GetDlgItem(IDC_BUTTON_OPTIMISE)->EnableWindow((ulWayPointNumber > 3) ? TRUE : FALSE);
	GetDlgItem(IDC_BUTTON_DELROUTE)->EnableWindow((ulWayPointNumber > 1) ? TRUE : FALSE);
	GetDlgItem(IDC_BUTTON_VIEWALL)->EnableWindow((ulWayPointNumber > 0) ? TRUE : FALSE);

	/* Refresh point number label */
	resStr.Format(IDS_WAYPOINTS_NB, ulWayPointNumber);
	m_LabelPointNumber.ShowWindow(SW_HIDE);
	m_LabelPointNumber.SetWindowText(resStr);
	m_LabelPointNumber.ShowWindow(SW_SHOW);
}

void CEditorDlg::UpdateResults()
{
	CString resStr;

	/* Refresh result number label */
	if (((CButton*)GetDlgItem(IDC_SEARCH_LOCATION))->GetCheck())
		resStr.Format(IDS_RESULTS, m_cLocationResult.size());
	else
		resStr.Format(IDS_RESULTS, m_cProximityResult.size());

	GetDlgItem(IDC_STATIC_RESULTS)->SetWindowText(resStr);
}

void CEditorDlg::OnSignal(const std::string& strText)
{
	ScopedWaitCursor swc(*this);

#if defined(_DEBUG) || defined(FORCE_LOG)
	CITNConverterApp::log() << clock() << ": OnSignal(" << strText << ")" << std::endl;
#endif //_DEBUG

	switch (strText[0])
	{
	case 'L': // Loaded
	{
		double dJsVersion = 0;
		std::string strJsUrl;

		// Parse json argument
		CJsonParser jsParser;
		if (jsParser.parse(strText.substr(1)) == CJsonParser::JSON_SUCCESS)
		{
			dJsVersion = jsParser("software")("version");
			strJsUrl = jsParser("software")("url");
		}

		m_Navigator.InitInterface();
		if (m_MapsInfos.fromJs(m_Navigator.JavaScript_GetMaps())) // Lecture des cartes
		{
			// Set user selection, read it from registry
			std::map<std::string, CRegParam::S_MAP_OPTION> mapMapsOptions;
			CITNConverterApp::RegParam().ReadMapsOptions(mapMapsOptions); // Read map options from registry

			for (std::map<std::string, CRegParam::S_MAP_OPTION>::const_iterator cit = mapMapsOptions.begin(); cit != mapMapsOptions.end(); ++cit)
			{
				jsMapsInfos::iterator it = m_MapsInfos.find(cit->first);
				if (it != m_MapsInfos.end())
				{
					const CRegParam::S_MAP_OPTION& mapOption = cit->second;
					jsMapInfos& mapInfos = it->second;

					mapInfos[jsMapInfos::E_MAP].setSelected(mapOption.bMap);
					mapInfos[jsMapInfos::E_SAT].setSelected(mapOption.bSat);
					mapInfos[jsMapInfos::E_HYBRID].setSelected(mapOption.bHybrid);
					mapInfos[jsMapInfos::E_TOPO].setSelected(mapOption.bTopo);
				}
			}
		}

		// Lecture des providers
		m_ProvidersInfos.fromJs(m_Navigator.JavaScript_GetProviders());
		for (const auto& providerInfos : m_ProvidersInfos)
		{
			if (providerInfos.second.isAvailable())
			{
				geo::CGeoProvider& provider = geo::CGeoProviders::instance().get(providerInfos.first, std::nothrow);
				provider.setKey(providerInfos.second.getKey());
				provider.setLang(providerInfos.second.getLang());
				provider.setReferer(providerInfos.second.getReferer());
			}
			else
			{
				geo::CGeoProviders::instance().remove(providerInfos.first);
			}
		}

		// Check current route provider
		if (!geo::CGeoProviders::instance().exist(CITNConverterApp::RegParam().RouteProvider()))
		{
			CITNConverterApp::RegParam().RouteProvider() = geo::CGeoProviders::instance().getDefaultProvider();
			CITNConverterApp::RegParam().VehicleType() = geo::GeoVehicleType::Default;
		}

		// Preparation des parametres
		CJsonParser jsParam;
		CJsonObject& jsParamRoot = jsParam.setType(CJsonParser::JSON_TYPE_OBJECT);

		const std::pair<std::string, int>& curMap = CITNConverterApp::RegParam().getCurrentMap();
		if (!curMap.first.empty())
		{
			CJsonObject& jsCurrentMap = jsParamRoot.add("map").setType(CJsonValue::JSON_TYPE_OBJECT);
			jsCurrentMap.add("flag") = curMap.first;
			jsCurrentMap.add("type") = curMap.second;
		}

		CJsonArray& jsMaps = jsParamRoot.add("maps").setType(CJsonValue::JSON_TYPE_ARRAY);
		for (jsMapsInfos::const_iterator cit = m_MapsInfos.begin(); cit != m_MapsInfos.end(); ++cit)
		{
			const jsMapInfos& mapInfos = cit->second;

			CJsonObject& jsMap = jsMaps.add().setType(CJsonValue::JSON_TYPE_OBJECT);
			jsMap.add("flag") = mapInfos.flag();
			jsMap.add("map") = mapInfos[jsMapInfos::E_MAP].isSelected();
			jsMap.add("sat") = mapInfos[jsMapInfos::E_SAT].isSelected();
			jsMap.add("hybrid") = mapInfos[jsMapInfos::E_HYBRID].isSelected();
			jsMap.add("topo") = mapInfos[jsMapInfos::E_TOPO].isSelected();
		}

		jsParamRoot.add("number") = CITNConverterApp::RegParam().DisplayNumber();
		CJsonObject& jsLang = jsParamRoot.add("lang").setType(CJsonValue::JSON_TYPE_OBJECT);

		jsLang.add("hl") = CToolsString::Load(IDS_WEB_EDITOR_LANG);

		if (CSendToGps::IsAvailabled(CSendToGps::E_GPS_NAVIGON, CSendToGps::E_TYPE_POINT))
			jsLang.add("nvgfresh") = CToolsString::Load(IDS_SEND_TO_NVGFRESH);

		if (CSendToGps::IsAvailabled(CSendToGps::E_GPS_PIONEER, CSendToGps::E_TYPE_POINT))
			jsLang.add("navgate") = CToolsString::Load(IDS_SEND_TO_NAVGATE);

		if (CSendToGps::IsAvailabled(CSendToGps::E_GPS_MIOMORE, CSendToGps::E_TYPE_POINT))
			jsLang.add("miomore") = CToolsString::Load(IDS_SEND_TO_MIOMORE);

		if (CSendToGps::IsAvailabled(CSendToGps::E_GPS_GARMIN, CSendToGps::E_TYPE_POINT))
			jsLang.add("garmin") = CToolsString::Load(IDS_SEND_TO_GARMIN);

		m_Navigator.JavaScript_InitMap(jsParam.str());

		// Check version
		if (dJsVersion > stdx::string_helper::string_to<double>(SOFT_VERSION))
		{
			m_LabelStatus.SetWindowText(stdx::wformat(CWToolsString::Load(IDS_NEW_VERSION))(dJsVersion)(stdx::wstring_helper::from_utf8(strJsUrl)).str().c_str());
			m_Navigator.JavaScript_ShowUpdateWindow();
		}
		else
		{
			// Display advice
			m_LabelStatus.SetWindowText(CWToolsString::Load(IDS_ADVICE).c_str());
		}
	}
	break;

	case 'I': // Initialised
	{
		// Association du navigateur
		m_ListPoint.SetNavigator(m_Navigator, 0, CNavPointView::E_VIEW_TYPE_ROADBOOK);
		m_ListSearch.SetNavigator(m_Navigator, 1, CNavPointView::E_VIEW_TYPE_SEARCH);
		m_ListProximity.SetNavigator(m_Navigator, 2, CNavPointView::E_VIEW_TYPE_PROXIMITY);
		m_ListFavorites.SetNavigator(m_Navigator, 3, CNavPointView::E_VIEW_TYPE_FAVORITE);

		m_ListPoint.Refresh();
		m_ListSearch.Refresh();
		m_ListProximity.Refresh();
		m_ListFavorites.Refresh();

		UpdateButtons();
		UpdateResults();
		UpdatePois();

		m_Navigator.JavaScript_ViewAllIdem();
		m_Navigator.JavaScript_MapClickEnabled();

		GetDlgItem(IDC_BUTTON_SEARCH)->EnableWindow(TRUE);
	}
	break;

	default:
		break;
	}
}

void CEditorDlg::OnButtonDown()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Down();
}

void CEditorDlg::OnButtonUp()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Up();
}

void CEditorDlg::OnButtonRemove()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Remove();
}

void CEditorDlg::OnButtonRemoveall()
{
	ScopedWaitCursor swc(*this);

	std::wstring strTitle = CWToolsString::Load(IDS_TOOLTIP_REMOVEALL);
	std::wstring strMessage = CWToolsString::Load(IDS_CONFIRM_REMOVE);

	if (MessageBox(strMessage.c_str(), strTitle.c_str(), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		m_ListPoint.Clear();
		CallNavigate();
	}
}

void CEditorDlg::OnButtonInvert()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Invert();
}

void CEditorDlg::OnButtonViewall()
{
	ScopedWaitCursor swc(*this);
	m_Navigator.JavaScript_ViewAllIdem();
}

void CEditorDlg::OnButtonCalculate()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.CalculateAll();
}

void CEditorDlg::OnButtonClearFavorites()
{
	ScopedWaitCursor swc(*this);

	std::wstring strTitle = CWToolsString::Load(IDS_TOOLTIP_REMOVEALL);
	std::wstring strMessage = CWToolsString::Load(IDS_CONFIRM_CLEAR_FAV);

	if (MessageBox(strMessage.c_str(), strTitle.c_str(), MB_YESNO | MB_ICONQUESTION) == IDYES)
	{
		m_ListFavorites.Clear();
	}
}

void CEditorDlg::OnButtonImportFavorites()
{
	ScopedWaitCursor swc(*this);

	std::wstring strFilters = CWToolsString::Load(IDS_GPXFILTERS);

	// Create open file dialog
	CFileDialog cFileDialog(TRUE,
		TYPE_GPX,
		nullptr,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
		strFilters.c_str(),
		this);

	std::wstring strTitle = CWToolsString::Load(IDS_IMPORT_FAV);
	cFileDialog.m_ofn.lpstrTitle = strTitle.c_str();

	if (cFileDialog.DoModal() == IDOK)
	{
		std::vector<CGpsPointArray*> vecGpsArray;
		std::vector<CGpsPointArray*>::iterator it;

		HRESULT hr = ReadGPX((LPCTSTR)cFileDialog.GetPathName(), vecGpsArray, false);
		if (hr == S_OK)
		{
			for (it = vecGpsArray.begin(); it != vecGpsArray.end(); ++it)
				m_cFavorites += **it;

			m_ListFavorites.Refresh();
		}
		else
		{
			DisplayFileError(hr);
		}

		for (it = vecGpsArray.begin(); it != vecGpsArray.end(); ++it)
			delete* it;
	}
}

void CEditorDlg::OnButtonExportFavorites()
{
	ScopedWaitCursor swc(*this);

	std::wstring strFilters = CWToolsString::Load(IDS_GPXFILTERS);

	// Create save file dialog
	CFileDialog cFileDialog(FALSE,
		_T("gpx"),
		nullptr,
		OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
		strFilters.c_str(),
		this);

	std::wstring strTitle = CWToolsString::Load(IDS_EXPORT_FAV);
	cFileDialog.m_ofn.lpstrTitle = strTitle.c_str();

	if (cFileDialog.DoModal() == IDOK)
	{
		if (WriteGPX((LPCTSTR)cFileDialog.GetPathName(), m_cFavorites, 0, false) != S_OK)
			AfxMessageBox(IDS_ERRSAVE, MB_OK | MB_ICONSTOP);
	}
}

void CEditorDlg::OnSearch()
{
	ScopedWaitCursor swc(*this);

	int nRet = IDOK;
	CString resStr;

	GetDlgItem(IDC_STATIC_RESULTS)->SetWindowText(CWToolsString::Load(IDS_SEARCHING).c_str());

	UpdateData();

	std::string searchText = stdx::wstring_helper::to_utf8((LPCTSTR)m_csSearch);

	if (((CButton*)GetDlgItem(IDC_SEARCH_LOCATION))->GetCheck())
	{
		geo::CGeoLocalSearch gGoogleLocalSearch(geo::E_GEO_PROVIDER_GOOGLE_API);
		geo::CGeoGeocoder gGoogleGeocoder(geo::E_GEO_PROVIDER_GOOGLE_API);
		geo::CGeoGeocoder gHereGeocoder(geo::E_GEO_PROVIDER_HERE_API);

		do
		{
			try
			{
				gGoogleLocalSearch->Load(searchText);
				gGoogleGeocoder->Load(searchText);
				gHereGeocoder->Load(searchText);
				nRet = IDOK;
			}
			catch (CInternetException& e)
			{
				nRet = HTTPErrorBox(e.code());
			}
		} while (nRet == IDRETRY);

		m_cLocationResult.clear();
		if (nRet == IDOK)
		{
			if (gGoogleGeocoder->getStatus() == geo::E_GEO_OK)
				m_cLocationResult += gGoogleGeocoder->getResults();

			if (gHereGeocoder->getStatus() == geo::E_GEO_OK)
				m_cLocationResult += gHereGeocoder->getResults();

			if (gGoogleLocalSearch->getStatus() == geo::E_GEO_OK)
				m_cLocationResult += gGoogleLocalSearch->getLocations();

			m_cLocationResult.removeDuplicates();
		}

		m_ListSearch.Refresh();
		m_ListSearch.SelectItem(m_cLocationResult.size() ? 0 : -1);

		if (gGoogleGeocoder->getStatus() != geo::E_GEO_OK && gHereGeocoder->getStatus() != geo::E_GEO_OK && gGoogleLocalSearch->getStatus() != geo::E_GEO_OK)
			resStr.Format(GetStringStatus(gGoogleGeocoder->getStatus()).c_str(), (LPCTSTR)m_csSearch);
	}
	else
	{
		auto doLocalSearch = [&](geo::E_GEO_PROVIDER eGeoProvider) -> geo::E_GEO_STATUS_CODE
		{
			geo::CGeoLocalSearch gLocalSearch(eGeoProvider);

			do
			{
				try
				{
					m_cProximityResult.clear();
					gLocalSearch->Load(searchText, m_cSelectedLocation);

					if (gLocalSearch->getStatus() == geo::E_GEO_OK)
					{
						m_cProximityResult = gLocalSearch->getLocations();
						m_cProximityResult.removeDuplicates();
					}

					nRet = IDOK;
				}
				catch (CInternetException& e)
				{
					nRet = HTTPErrorBox(e.code());
				}
			} while (nRet == IDRETRY);

			return gLocalSearch->getStatus();
		};

		geo::E_GEO_STATUS_CODE sc = doLocalSearch(geo::E_GEO_PROVIDER_GOOGLE_API);
		if (m_cProximityResult.empty())
			sc = doLocalSearch(geo::E_GEO_PROVIDER_HERE_API);

		m_ListProximity.Refresh();
		m_ListProximity.SelectItem(m_cProximityResult.size() ? 0 : -1);
		resStr.Format(GetStringStatus(sc).c_str(), (LPCTSTR)m_csSearch);
	}

	m_LabelStatus.SetWindowText(resStr);
	UpdateResults();
}

void CEditorDlg::OnDblclkListSearch(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE lpNMItem = (LPNMITEMACTIVATE)pNMHDR;

	if (lpNMItem->iItem != MAXDWORD)
	{
		m_ptabList[TAB_LIST_WAYPOINT]->Add(m_cLocationResult[lpNMItem->iItem]);
		m_ptabList[TAB_LIST_SEARCH]->Remove(lpNMItem->iItem);
		UpdateResults();
	}

	*pResult = 0;
}

void CEditorDlg::OnItemchangedListSearch(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_ListSearch.GetSelectedCount() && static_cast<size_t>(pNMListView->iItem) < m_cLocationResult.size())
		m_cSelectedLocation = m_cLocationResult[pNMListView->iItem];
	else
		m_cSelectedLocation.clear();

	*pResult = 0;
}

void CEditorDlg::OnDblclkListProximity(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE lpNMItem = (LPNMITEMACTIVATE)pNMHDR;

	if (lpNMItem->iItem != MAXDWORD)
	{
		m_ptabList[TAB_LIST_WAYPOINT]->Add(m_cProximityResult[lpNMItem->iItem]);
		m_ptabList[TAB_LIST_PROXIMITY]->Remove(lpNMItem->iItem);
		UpdateResults();
	}

	*pResult = 0;
}

void CEditorDlg::OnDblclkListFavorites(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE lpNMItem = (LPNMITEMACTIVATE)pNMHDR;

	if (lpNMItem->iItem != MAXDWORD)
		m_ptabList[TAB_LIST_WAYPOINT]->Add(m_cFavorites[lpNMItem->iItem]);

	*pResult = 0;
}

void CEditorDlg::OnRclickListFavorites(NMHDR*, LRESULT* pResult)
{
	POINT pCursorPos;

	GetCursorPos(&pCursorPos);
	m_FavPopupMenu.EnableMenuItem(WM_CLEAR_FAV, MF_BYCOMMAND | (m_cFavorites.empty() ? (MF_DISABLED | MF_GRAYED) : MF_ENABLED));
	m_FavPopupMenu.EnableMenuItem(WM_EXPORT_FAV, MF_BYCOMMAND | (m_cFavorites.empty() ? (MF_DISABLED | MF_GRAYED) : MF_ENABLED));
	m_FavPopupMenu.TrackPopupMenu(TPM_RIGHTBUTTON, pCursorPos.x, pCursorPos.y, this);

	*pResult = 0;
}

void CEditorDlg::OnItemchangedListFavorites(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_ListFavorites.GetSelectedCount() && static_cast<size_t>(pNMListView->iItem) < m_cFavorites.size())
		m_cSelectedLocation = m_cFavorites[pNMListView->iItem];
	else
		m_cSelectedLocation.clear();

	*pResult = 0;
}

void CEditorDlg::OnBeforeNavigate2Explorer(LPDISPATCH, VARIANT FAR* URL, VARIANT FAR*, VARIANT FAR*, VARIANT FAR*, VARIANT FAR*, BOOL FAR* Cancel)
{
#ifdef _DEBUG
	CString cString;
	GetWindowText(cString);
	std::wstring strRefUrl((LPCTSTR)cString);
#else
	std::wstring strRefUrl(URL_JS_MAPS);
#endif
	std::wstring strUrl = URL->bstrVal;

	if (strUrl.find(URL_SIGNAL) != std::wstring::npos)
	{
		strUrl.erase(0, strUrl.find(':') + 1);
		OnSignal(stdx::uri_helper::unescape(stdx::wstring_helper::to_utf8(strUrl)));
		*Cancel = true;
	}
	else if (strUrl == URL_ABOUT_BLANK)
	{
		*Cancel = true;
	}
	else if (strUrl.find(URL_JAVASCRIPT) != 0 && strUrl != strRefUrl)
	{
		ShellExecute(nullptr, _T("open"), strUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
		*Cancel = true;
	}
}

BOOL CEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	// Let the ToolTip process this message.
	m_tooltip.RelayEvent(pMsg);

	return CSizeableDialog::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

void CEditorDlg::OnClose()
{
	WINDOWPLACEMENT& wdPlacement = CITNConverterApp::RegParam().EditorPlacement();
	GetWindowPlacement(&wdPlacement);
	wdPlacement.length = sizeof(wdPlacement);

	CString str;
	m_EditName.GetWindowText(str);
	m_cGpsPointArray.name(stdx::wstring_helper::to_utf8((LPCTSTR)str));

	CITNConverterApp::RegParam().WriteFavorites(m_cFavorites);
	CITNConverterApp::RegParam().Flush();

	CSizeableDialog::OnClose();
}

void CEditorDlg::OnOK()
{
	OnClose();
	CSizeableDialog::OnOK();
}

void CEditorDlg::OnNavigateErrorExplorer(LPDISPATCH, VARIANT FAR*, VARIANT FAR*, VARIANT FAR* StatusCode, BOOL FAR* Cancel)
{
	*Cancel = true;
	if (HTTPErrorBox(StatusCode->lVal) == IDRETRY)
		SetTimer(1, 100, nullptr);
}

void CEditorDlg::OnTimer(UINT nIDEvent)
{
	KillTimer(nIDEvent);
	CallNavigate();

	CSizeableDialog::OnTimer(nIDEvent);
}

void CEditorDlg::OnProgressChangeExplorer(long /*Progress*/, long /*ProgressMax*/)
{
	/* Do nothing now */
}

void CEditorDlg::OnButtonDelroute()
{
	ScopedWaitCursor swc(*this);

	m_ListPoint.DisableAutoCalc();
	m_ListPoint.ClearDriving();
}

void CEditorDlg::OnButtonOptimise()
{
	ScopedWaitCursor swc(*this);
	m_ListPoint.Optimize();
}

void CEditorDlg::OnButtonPrint()
{
	ScopedWaitCursor swc(*this);
}

void CEditorDlg::OnButtonPoi()
{
	ScopedWaitCursor swc(*this);
}

void CEditorDlg::UpdatePois()
{
	ScopedWaitCursor swc(*this);
}

void CEditorDlg::OnSearchLocation()
{
	UpdateSearchArea();
}

void CEditorDlg::OnSearchProximity()
{
	UpdateSearchArea();
}

void CEditorDlg::OnDisplayFavorites()
{
	UpdateSearchArea();
}

void CEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	m_ListPoint.MouseMove(nFlags, point);
	CSizeableDialog::OnMouseMove(nFlags, point);
}

void CEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_ListPoint.LButtonUp(nFlags, point);
	CSizeableDialog::OnLButtonUp(nFlags, point);
}

void CEditorDlg::OnButtonConfig()
{
	ScopedWaitCursor swc(*this);

	jsMapsInfos mapsInfos(m_MapsInfos);
	geo::GeoVehicleType::type_t vehicleType = CITNConverterApp::RegParam().VehicleType();
	geo::GeoItineraryType::type_t itineraryType = CITNConverterApp::RegParam().ItineraryType();
	geo::E_GEO_PROVIDER eRouteProvider = CITNConverterApp::RegParam().RouteProvider();
	bool bAvoidHighway = CITNConverterApp::RegParam().AvoidHighway();
	bool bAvoidTolls = CITNConverterApp::RegParam().AvoidTolls();
	bool bDisplayNumber = CITNConverterApp::RegParam().DisplayNumber();

	if (CConfigDlg(m_MapsInfos, m_ProvidersInfos, this).DoModal() == IDOK)
	{
		UpdateCalculateButton();

		if (vehicleType != CITNConverterApp::RegParam().VehicleType()
			|| itineraryType != CITNConverterApp::RegParam().ItineraryType()
			|| eRouteProvider != CITNConverterApp::RegParam().RouteProvider()
			|| bAvoidHighway != CITNConverterApp::RegParam().AvoidHighway()
			|| bAvoidTolls != CITNConverterApp::RegParam().AvoidTolls())
			m_ListPoint.ClearDriving();

		if (!CITNConverterApp::RegParam().AutoCalc())
			m_ListPoint.DisableAutoCalc();

		if (mapsInfos != m_MapsInfos || bDisplayNumber != CITNConverterApp::RegParam().DisplayNumber())
			CallNavigate();
		else
			m_ListPoint.CalculateAll();
	}
}
