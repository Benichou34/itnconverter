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
 * Purpose : Config dialog Windows implementation
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "itn converter.h"
#include "ConfigDlg.h"
#include "jsMapInfos.h"
#include "jsProviderInfos.h"

namespace
{
	template <class T>
	struct comboBoxPair
	{
		T identifier;
		UINT uiRessource;
	};

	const std::vector<comboBoxPair<geo::E_GEO_PROVIDER>> cs_RouteProvider =
	{
		{ geo::E_GEO_PROVIDER_INTERNAL, IDS_CFG_STRAIGHT_LINES },
		{ geo::E_GEO_PROVIDER_GOOGLE_API, IDS_PROVIDER_GOOGLE_MAP },
		{ geo::E_GEO_PROVIDER_BING_API, IDS_PROVIDER_BING_API },
		{ geo::E_GEO_PROVIDER_VIAMICHELIN_API, IDS_PROVIDER_VIAMICHLIN },
		{ geo::E_GEO_PROVIDER_TOMTOM_API, IDS_PROVIDER_TOMTOM },
		{ geo::E_GEO_PROVIDER_OSRM_API, IDS_PROVIDER_OSM },
		{ geo::E_GEO_PROVIDER_WAZE_MAP, IDS_PROVIDER_WAZE_USA },
		{ geo::E_GEO_PROVIDER_WAZE_WORLD_MAP, IDS_PROVIDER_WAZE_WORLD },
		{ geo::E_GEO_PROVIDER_HERE_API, IDS_PROVIDER_HERE_API },
		{ geo::E_GEO_PROVIDER_GEOPORTAL_API, IDS_PROVIDER_GEOPORTAL_API }
	};

	std::set<geo::E_GEO_PROVIDER> cs_AcceptedRouteProvider =
	{
		geo::E_GEO_PROVIDER_INTERNAL,
		geo::E_GEO_PROVIDER_GOOGLE_API,
		geo::E_GEO_PROVIDER_BING_API,
		geo::E_GEO_PROVIDER_VIAMICHELIN_API,
		geo::E_GEO_PROVIDER_TOMTOM_API,
		geo::E_GEO_PROVIDER_OSRM_API,
		geo::E_GEO_PROVIDER_WAZE_MAP,
		geo::E_GEO_PROVIDER_WAZE_WORLD_MAP,
		geo::E_GEO_PROVIDER_HERE_API,
		geo::E_GEO_PROVIDER_GEOPORTAL_API
	};

	const std::vector<comboBoxPair<geo::GeoVehicleType::type_t>> cs_VehiculeType =
	{
		{ geo::GeoVehicleType::Default, IDS_VEHICLE_CAR },
		{ geo::GeoVehicleType::Car, IDS_VEHICLE_CAR },
		{ geo::GeoVehicleType::Motorbike, IDS_VEHICLE_MOTORBIKE },
		{ geo::GeoVehicleType::Truck, IDS_VEHICLE_TRUCK },
		{ geo::GeoVehicleType::Campervan, IDS_VEHICLE_CAMPERVAN },
		{ geo::GeoVehicleType::Bus, IDS_VEHICLE_BUS },
		{ geo::GeoVehicleType::Bike, IDS_VEHICLE_BIKE },
		{ geo::GeoVehicleType::Pedestrian, IDS_VEHICLE_FOOT }
	};

	const std::vector<comboBoxPair<geo::GeoItineraryType::type_t>> cs_ItineraryType =
	{
		{ geo::GeoItineraryType::Recommended, IDS_ITINERARY_DEFAULT },
		{ geo::GeoItineraryType::Quickest, IDS_ITINERARY_QUICKEST },
		{ geo::GeoItineraryType::Shortest, IDS_ITINERARY_SHORTEST },
		{ geo::GeoItineraryType::Sightseeing, IDS_ITINERARY_SIGHTSEEING },
		{ geo::GeoItineraryType::Economical, IDS_ITINERARY_ECONOMICAL },
		{ geo::GeoItineraryType::Thrilling, IDS_ITINERARY_THRILLING }
	};

#ifdef IDC_COMBO_TRUCK_CATEGORY
	const std::vector<comboBoxPair<geo::GeoTruckCategoryType::type_t>> cs_TruckCategoryType =
	{
		{ geo::GeoTruckCategoryType::no_catory, IDS_TRUCK_NO_CATEGORY },
		{ geo::GeoTruckCategoryType::public_transport, IDS_TRUCK_PUBLIC_TRANSPORT },
		{ geo::GeoTruckCategoryType::commun_goods, IDS_TRUCK_COMMON_GOODS },
		{ geo::GeoTruckCategoryType::hazardous_explosive, IDS_TRUCK_HAZARDOUS_EXPLOSIVE },
		{ geo::GeoTruckCategoryType::hazardous_gas, IDS_TRUCK_HAZARDOUS_GAS },
		{ geo::GeoTruckCategoryType::hazardous_flammable, IDS_TRUCK_HAZARDOUS_FLAMMABLE },
		{ geo::GeoTruckCategoryType::hazardous_combustible, IDS_TRUCK_HAZARDOUS_COMBUSTIBLE },
		{ geo::GeoTruckCategoryType::hazardous_organic, IDS_TRUCK_HAZARDOUS_ORGANIC },
		{ geo::GeoTruckCategoryType::hazardous_poison, IDS_TRUCK_HAZARDOUS_POISON },
		{ geo::GeoTruckCategoryType::hazardous_radioActive, IDS_TRUCK_HAZARDOUS_RADIOACTIVE },
		{ geo::GeoTruckCategoryType::hazardous_corrosive, IDS_TRUCK_HAZARDOUS_CORROSIVE },
		{ geo::GeoTruckCategoryType::hazardous_poisonousInhalation, IDS_TRUCK_HAZARDOUS_POISONOUS },
		{ geo::GeoTruckCategoryType::hazardous_harmfulToWater, IDS_TRUCK_HAZARDOUS_HARMFUL},
		{ geo::GeoTruckCategoryType::hazardous_other, IDS_TRUCK_HAZARDOUS_OTHER },
		{ geo::GeoTruckCategoryType::hazardous_all, IDS_TRUCK_HAZARDOUS_ALL }
	};
#endif

	const std::vector<comboBoxPair<CRegParam::E_COORDINATES>> cs_CoordinatesFormatList =
	{
		{ CRegParam::E_COORDINATES_DD, IDS_DECIMAL_DEGREES },
		{ CRegParam::E_COORDINATES_DM, IDS_DEGREES_MINUTES },
		{ CRegParam::E_COORDINATES_DMS, IDS_DEGREES_MINUTES_SECONDS }
	};

	template <class T>
	struct AcceptAll
	{
		size_t count(const T&) const { return 1; }
	};

	enum
	{
		E_MAPS_PROVIDER_GENERIC,
		E_MAPS_PROVIDER_ROAD,
		E_MAPS_PROVIDER_SAT,
		E_MAPS_PROVIDER_HYBRID,
		E_MAPS_PROVIDER_TOPO
	};

	void addMapToTreeMaps(CTreeCtrl& treeCtrl, size_t maxIcon, HTREEITEM parentItem, jsMapInfos& mapInfos, jsMapInfos::E_MAP_TYPE mapType)
	{
		if (mapInfos[mapType].isAvailable())
		{
			size_t icon = (mapInfos.icon() < maxIcon) ? mapInfos.icon() : 0;
			HTREEITEM hItem = treeCtrl.InsertItem(stdx::wstring_helper::from_utf8(mapInfos.name()).c_str(), icon, icon, parentItem);
			treeCtrl.SetItemData(hItem, reinterpret_cast<DWORD>(&mapInfos[mapType]));
			treeCtrl.SetCheck(hItem, mapInfos[mapType].isSelected() ? TRUE : FALSE);
		}
	}

	void TreeCtrlCheckItem(CTreeCtrl& treeCtrl, HTREEITEM hItem)
	{
		BOOL bChecked = TRUE;

		HTREEITEM hChildItem = treeCtrl.GetChildItem(hItem);
		while (hChildItem && bChecked == TRUE)
		{
			if (treeCtrl.GetCheck(hChildItem) == FALSE)
				bChecked = FALSE;

			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
		}

		treeCtrl.SetCheck(hItem, bChecked);
	}

	void TreeCtrlCheckParent(CTreeCtrl& treeCtrl, HTREEITEM hItem)
	{
		HTREEITEM hNextItem = treeCtrl.GetParentItem(hItem);
		while (hNextItem)
		{
			TreeCtrlCheckItem(treeCtrl, hNextItem);
			hNextItem = treeCtrl.GetNextItem(hNextItem, TVGN_PARENT);
		}
	}

	void TreeCtrlCheckChildren(CTreeCtrl& treeCtrl, HTREEITEM hItem)
	{
		if (!treeCtrl.ItemHasChildren(hItem))
			return;

		BOOL bChecked = treeCtrl.GetCheck(hItem);

		HTREEITEM hNextItem = treeCtrl.GetChildItem(hItem);
		while (hNextItem)
		{
			treeCtrl.SetCheck(hNextItem, bChecked);
			TreeCtrlCheckChildren(treeCtrl, hNextItem);
			hNextItem = treeCtrl.GetNextItem(hNextItem, TVGN_NEXT);
		}
	}

	void TreeCtrlCheckAll(CTreeCtrl& treeCtrl, BOOL bCheck)
	{
		HTREEITEM hItem = treeCtrl.GetRootItem();
		while (hItem)
		{
			treeCtrl.SetCheck(hItem, bCheck);
			TreeCtrlCheckChildren(treeCtrl, hItem);
			hItem = treeCtrl.GetNextItem(hItem, TVGN_NEXT);
		}
	}

	template <class T, class R>
	void FillComboBox(
		CWnd* pCwdComboxBox,
		const std::vector<comboBoxPair<T>>& srcList,
		const R& acceptedIdentifier,
		const T& defaultIdentifier)
	{
		if (!pCwdComboxBox)
			return;

		CComboBox* pComboBox = static_cast<CComboBox*>(pCwdComboxBox);
		while (pComboBox->GetCount() > 0) // Clear content
			pComboBox->DeleteString(0);

		for (auto it = std::begin(srcList); it != std::end(srcList); ++it)
		{
			if (acceptedIdentifier.count(it->identifier) > 0)
			{
				std::wstring str;
				if (!it->uiRessource)
					str = stdx::wstring_helper::to_string(it->identifier);
				else
					str = CWToolsString::Load(it->uiRessource);

				int nIndex = pComboBox->AddString(str.c_str());
				pComboBox->SetItemData(nIndex, static_cast<DWORD>(it->identifier));
				if (it->identifier == defaultIdentifier)
					pComboBox->SetCurSel(nIndex);
			}
		}

		if (pComboBox->GetCount() > 0 && pComboBox->GetCurSel() == CB_ERR)
			pComboBox->SetCurSel(0);

		pComboBox->EnableWindow((pComboBox->GetCount() > 1) ? TRUE : FALSE);
	}

	template <class T>
	void getComboBoxValue(const CWnd* pCwdComboxBox, T& value)
	{
		const CComboBox* pComboBox = static_cast<const CComboBox*>(pCwdComboxBox);
		if (pComboBox && pComboBox->GetCurSel() != CB_ERR)
			value = static_cast<T>(pComboBox->GetItemData(pComboBox->GetCurSel()));
	}

	void enableDlgItemWindow(CWnd* pCwd, bool bValue)
	{
		if (pCwd)
			pCwd->EnableWindow(bValue ? TRUE : FALSE);
	}

	template <class T>
	T getEditValue(CWnd* pCwd)
	{
		CString str;

		if (!pCwd)
			static_cast<CEdit*>(pCwd)->GetWindowText(str);

		return stdx::wstring_helper::string_to<T>((LPCTSTR)str);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg dialog

CConfigDlg::CConfigDlg(jsMapsInfos& mapsInfos, const jsProvidersInfos& providersInfos, CWnd* pParent) : CDialog(CConfigDlg::IDD, pParent),
m_MapsInfos(mapsInfos),
m_pAcceptedRouteOptions(nullptr)
{
	geo::CGeoDirections geoDirections(CITNConverterApp::RegParam().RouteProvider());
	m_CurrentTravelOptions = geoDirections->getSupportedTravelOptions();
	auto it = m_CurrentTravelOptions.find(CITNConverterApp::RegParam().VehicleType());
	if (it != m_CurrentTravelOptions.end())
		m_pAcceptedRouteOptions = it->second.get();
	else if (!m_CurrentTravelOptions.empty())
		m_pAcceptedRouteOptions = m_CurrentTravelOptions.begin()->second.get();

	for (const auto& providerInfos : providersInfos)
	{
		if (!providerInfos.second.isAvailable())
			cs_AcceptedRouteProvider.erase(providerInfos.first);
	}

	for (auto itP = cs_AcceptedRouteProvider.begin(); itP != cs_AcceptedRouteProvider.end(); ++itP)
	{
		if (*itP != geo::E_GEO_PROVIDER_INTERNAL && !geo::CGeoProviders::instance().exist(*itP))
			itP = cs_AcceptedRouteProvider.erase(itP);
	}

	//{{AFX_DATA_INIT(CConfigDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
#ifdef IDC_TREE_MAPS
	DDX_Control(pDX, IDC_TREE_MAPS, m_TreeMaps);
#endif
#ifdef IDC_EDIT_TRUCK_HEIGHT
	DDX_Text(pDX, IDC_EDIT_TRUCK_HEIGHT, m_dTruckHeight);
#endif
#ifdef IDC_EDIT_TRUCK_WIDTH
	DDX_Text(pDX, IDC_EDIT_TRUCK_WIDTH, m_dTruckWidth);
#endif
#ifdef IDC_EDIT_TRUCK_LENGTH
	DDX_Text(pDX, IDC_EDIT_TRUCK_LENGTH, m_dTruckLength);
#endif
#ifdef IDC_EDIT_TRUCK_LIMITED_WEIGHT
	DDX_Text(pDX, IDC_EDIT_TRUCK_LIMITED_WEIGHT, m_dTruckLimitedWeight);
#endif
#ifdef IDC_EDIT_TRUCK_WEIGHT_AXLE
	DDX_Text(pDX, IDC_EDIT_TRUCK_WEIGHT_AXLE, m_dTruckWeightAxle);
#endif
#ifdef IDC_EDIT_USER_ID
	DDX_Text(pDX, IDC_EDIT_USER_ID, m_csUserId);
#endif
	//}}AFX_DATA_MAP
}

#define TREE_MAPS_CHECK_STATE_CHANGE (WM_USER + 100)

BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigDlg)
	ON_WM_CTLCOLOR()
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST, OnKeydownList)
#ifdef IDC_COMBO_ROUTE_PROVIDER
	ON_CBN_SELCHANGE(IDC_COMBO_ROUTE_PROVIDER, OnSelchangeComboRouteProvider)
#endif
#ifdef IDC_COMBO_VEHICLE_TYPE
	ON_CBN_SELCHANGE(IDC_COMBO_VEHICLE_TYPE, OnSelchangeComboVehicleType)
#endif
#ifdef IDC_TREE_MAPS
	ON_BN_CLICKED(IDC_BUTTON_SELECT_ALL, OnSelectAllMaps)
	ON_BN_CLICKED(IDC_BUTTON_UNSELECT_ALL, OnUnselectAllMaps)
	ON_NOTIFY(NM_CLICK, IDC_TREE_MAPS, OnClickTreeMaps)
	ON_MESSAGE(TREE_MAPS_CHECK_STATE_CHANGE, OnTreeMapsCheckStateChange)
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigDlg message handlers

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(CWToolsString::Load(IDS_CFG_SETTINGS).c_str());

	// Create the ToolTip control.
	m_tooltip.Create(this);
	m_tooltip.Activate(TRUE);

	SetDlgItemWindowText<CStatic>(IDC_STATIC_OPT_DISPLAY, IDS_CFG_OPT_DISPLAY);
	SetDlgItemWindowText<CStatic>(IDC_STATIC_OPT_ITN, IDS_CFG_OPT_ITN);
	SetDlgItemWindowText<CStatic>(IDC_STATIC_ROUTE_PROVIDER, IDS_CFG_ROUTE_PROVIDER);
	SetDlgItemWindowText<CStatic>(IDC_STATIC_VEHICLE_TYPE, IDS_VEHICLE_TYPE);
	SetDlgItemWindowText<CStatic>(IDC_STATIC_ITINERARY_TYPE, IDS_ITINERARY_TYPE);
	SetDlgItemWindowText<CStatic>(IDC_STATIC_COORDS_FORMAT, IDS_COORD_FORMAT);
	SetDlgItemWindowText<CStatic>(IDC_STATIC_BACKGROUND, IDS_CFG_BACKGROUND);
#ifdef IDC_STATIC_OPT_TRUCK
	SetDlgItemWindowText<CStatic>(IDC_STATIC_OPT_TRUCK, IDS_VEHICLE_TRUCK);
#endif
#ifdef IDC_STATIC_TRUCK_HEIGHT
	SetDlgItemWindowText<CStatic>(IDC_STATIC_TRUCK_HEIGHT, IDS_CFG_TRUCK_HEIGHT);
#endif
#ifdef IDC_STATIC_TRUCK_WIDTH
	SetDlgItemWindowText<CStatic>(IDC_STATIC_TRUCK_WIDTH, IDS_CFG_TRUCK_WIDTH);
#endif
#ifdef IDC_STATIC_TRUCK_LENGTH
	SetDlgItemWindowText<CStatic>(IDC_STATIC_TRUCK_LENGTH, IDS_CFG_TRUCK_LENGTH);
#endif
#ifdef IDC_STATIC_TRUCK_TRAILERS
	SetDlgItemWindowText<CStatic>(IDC_STATIC_TRUCK_TRAILERS, IDS_CFG_TRUCK_TRAILERS);
#endif
#ifdef IDC_STATIC_TRUCK_LIMITED_WEIGHT
	SetDlgItemWindowText<CStatic>(IDC_STATIC_TRUCK_LIMITED_WEIGHT, IDS_CFG_TRUCK_LIMITED_WEIGHT);
#endif
#ifdef IDC_STATIC_TRUCK_WEIGHT_AXLE
	SetDlgItemWindowText<CStatic>(IDC_STATIC_TRUCK_WEIGHT_AXLE, IDS_CFG_TRUCK_WEIGHT_AXLE);
#endif
#ifdef IDC_STATIC_TRUCK_CATEGORY
	SetDlgItemWindowText<CStatic>(IDC_STATIC_TRUCK_CATEGORY, IDS_CFG_TRUCK_CATEGORY);
#endif

#ifdef IDC_CHECK_AUTO_CALC
	SetDlgItemWindowText<CButton>(IDC_CHECK_AUTO_CALC, IDS_CFG_AUTO_CALC);
	SetDlgItemCheck<CButton>(IDC_CHECK_AUTO_CALC, CITNConverterApp::RegParam().AutoCalc());
#endif

#ifdef IDC_CHECK_DIST_MILES
	SetDlgItemWindowText<CButton>(IDC_CHECK_DIST_MILES, IDS_CFG_DIST_MILES);
	SetDlgItemCheck<CButton>(IDC_CHECK_DIST_MILES, CITNConverterApp::RegParam().DistMiles());
#endif

#ifdef IDC_CHECK_AVOID_HIGHWAY
	SetDlgItemWindowText<CButton>(IDC_CHECK_AVOID_HIGHWAY, IDS_CFG_AVOID_HIGHWAY);
	SetDlgItemCheck<CButton>(IDC_CHECK_AVOID_HIGHWAY, CITNConverterApp::RegParam().AvoidHighway());
#endif

#ifdef IDC_CHECK_AVOID_TOLLS
	SetDlgItemWindowText<CButton>(IDC_CHECK_AVOID_TOLLS, IDS_CFG_AVOID_TOLLS);
	SetDlgItemCheck<CButton>(IDC_CHECK_AVOID_TOLLS, CITNConverterApp::RegParam().AvoidTolls());
#endif

#ifdef IDC_CHECK_FORCE_ROAD
	SetDlgItemWindowText<CButton>(IDC_CHECK_FORCE_ROAD, IDS_TOOLTIP_FORCE_ROAD);
	SetDlgItemCheck<CButton>(IDC_CHECK_FORCE_ROAD, CITNConverterApp::RegParam().ForceRoad());
#endif

#ifdef IDC_CHECK_DISPLAY_NUMBER
	SetDlgItemWindowText<CButton>(IDC_CHECK_DISPLAY_NUMBER, IDS_CFG_DISPLAY_NUMBER);
	SetDlgItemCheck<CButton>(IDC_CHECK_DISPLAY_NUMBER, CITNConverterApp::RegParam().DisplayNumber());
#endif

#ifdef IDC_CHECK_AUTO_ADD_STEP
	SetDlgItemWindowText<CButton>(IDC_CHECK_AUTO_ADD_STEP, IDS_CFG_AUTO_ADD_STEP);
	SetDlgItemCheck<CButton>(IDC_CHECK_AUTO_ADD_STEP, CITNConverterApp::RegParam().AutoAddStep());
#endif

#ifdef IDC_BUTTON_SELECT_ALL
	static_cast<CButton*>(GetDlgItem(IDC_BUTTON_SELECT_ALL))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_SELECT_ALL)));
	static_cast<CButton*>(GetDlgItem(IDC_BUTTON_SELECT_ALL))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_SELECT_ALL), IDS_TOOLTIP_SELECT_ALL); // Add toolTips to buttons
#endif

#ifdef IDC_BUTTON_UNSELECT_ALL
	static_cast<CButton*>(GetDlgItem(IDC_BUTTON_UNSELECT_ALL))->SetBitmap(LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_UNSELECT_ALL)));
	static_cast<CButton*>(GetDlgItem(IDC_BUTTON_UNSELECT_ALL))->SetWindowPos(nullptr, 0, 0, 31, 31, SWP_NOMOVE | SWP_NOZORDER);
	m_tooltip.AddTool(GetDlgItem(IDC_BUTTON_UNSELECT_ALL), IDS_TOOLTIP_UNSELECT_ALL); // Add toolTips to buttons
#endif

#ifdef IDC_TREE_MAPS
	m_IconBitmap.LoadBitmap(IDB_MAPS_ICON);
	m_ImageList.Create(16, 16, ILC_COLOR32, 0, 1);
	m_ImageList.Add(&m_IconBitmap, RGB(0, 0, 0));

	m_TreeMaps.SetImageList(&m_ImageList, TVSIL_NORMAL);

	HTREEITEM hMap = m_TreeMaps.InsertItem(CWToolsString::Load(IDS_CFG_BACK_MAP).c_str(), E_MAPS_PROVIDER_ROAD, E_MAPS_PROVIDER_ROAD);
	HTREEITEM hsat = m_TreeMaps.InsertItem(CWToolsString::Load(IDS_CFG_BACK_SAT).c_str(), E_MAPS_PROVIDER_SAT, E_MAPS_PROVIDER_SAT);
	HTREEITEM hHybrid = m_TreeMaps.InsertItem(CWToolsString::Load(IDS_CFG_BACK_HYBRID).c_str(), E_MAPS_PROVIDER_HYBRID, E_MAPS_PROVIDER_HYBRID);
	HTREEITEM hTopo = m_TreeMaps.InsertItem(CWToolsString::Load(IDS_CFG_BACK_TOPO).c_str(), E_MAPS_PROVIDER_TOPO, E_MAPS_PROVIDER_TOPO);

	for (jsMapsInfos::iterator it = m_MapsInfos.begin(); it != m_MapsInfos.end(); ++it)
	{
		jsMapInfos& mapInfos = it->second;

		addMapToTreeMaps(m_TreeMaps, m_ImageList.GetImageCount(), hMap, mapInfos, jsMapInfos::E_MAP);
		addMapToTreeMaps(m_TreeMaps, m_ImageList.GetImageCount(), hsat, mapInfos, jsMapInfos::E_SAT);
		addMapToTreeMaps(m_TreeMaps, m_ImageList.GetImageCount(), hHybrid, mapInfos, jsMapInfos::E_HYBRID);
		addMapToTreeMaps(m_TreeMaps, m_ImageList.GetImageCount(), hTopo, mapInfos, jsMapInfos::E_TOPO);
	}

	TreeCtrlCheckItem(m_TreeMaps, hMap);
	TreeCtrlCheckItem(m_TreeMaps, hsat);
	TreeCtrlCheckItem(m_TreeMaps, hHybrid);
	TreeCtrlCheckItem(m_TreeMaps, hTopo);
#endif // IDC_TREE_MAPS

#ifdef IDC_COMBO_ROUTE_PROVIDER
	FillComboBox(GetDlgItem(IDC_COMBO_ROUTE_PROVIDER), cs_RouteProvider, cs_AcceptedRouteProvider, CITNConverterApp::RegParam().RouteProvider());
	OnSelchangeComboRouteProvider();
#endif

#ifdef IDC_COMBO_COORDS_FORMAT
	FillComboBox(GetDlgItem(IDC_COMBO_COORDS_FORMAT), cs_CoordinatesFormatList, AcceptAll<CRegParam::E_COORDINATES>(), CITNConverterApp::RegParam().Coordinates());
#endif

#ifdef IDC_EDIT_TRUCK_HEIGHT
	m_dTruckHeight = static_cast<double>(CITNConverterApp::RegParam().TruckHeight()) / 10;
#endif
#ifdef IDC_EDIT_TRUCK_WIDTH
	m_dTruckWidth = static_cast<double>(CITNConverterApp::RegParam().TruckWidth()) / 10;
#endif
#ifdef IDC_EDIT_TRUCK_LENGTH
	m_dTruckLength = static_cast<double>(CITNConverterApp::RegParam().TruckLength()) / 10;
#endif
#ifdef IDC_EDIT_TRUCK_LIMITED_WEIGHT
	m_dTruckLimitedWeight = static_cast<double>(CITNConverterApp::RegParam().TruckLimitedWeight()) / 10;
#endif
#ifdef IDC_EDIT_TRUCK_WEIGHT_AXLE
	m_dTruckWeightAxle = static_cast<double>(CITNConverterApp::RegParam().TruckWeightAxle()) / 10;
#endif

#ifdef IDC_EDIT_USER_ID
	m_csUserId = CITNConverterApp::RegParam().UserId().c_str();
#endif

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConfigDlg::PreTranslateMessage(MSG* pMsg)
{
	// Let the ToolTip process this message.
	m_tooltip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

void CConfigDlg::OnOK()
{
	bool bOk = UpdateData() == TRUE;

#ifdef IDC_TREE_MAPS
	HTREEITEM hItem = m_TreeMaps.GetRootItem();
	while (hItem)
	{
		HTREEITEM hChildItem = m_TreeMaps.GetChildItem(hItem);
		while (hChildItem)
		{
			jsMapStatus* pMapStatus = reinterpret_cast<jsMapStatus*>(m_TreeMaps.GetItemData(hChildItem));
			if (pMapStatus)
				pMapStatus->setSelected(m_TreeMaps.GetCheck(hChildItem) == TRUE);

			hChildItem = m_TreeMaps.GetNextItem(hChildItem, TVGN_NEXT);
		}

		hItem = m_TreeMaps.GetNextItem(hItem, TVGN_NEXT);
	}

	std::map<std::string, CRegParam::S_MAP_OPTION> mapMapsOptions;
	for (jsMapsInfos::const_iterator cit = m_MapsInfos.begin(); cit != m_MapsInfos.end(); ++cit)
	{
		const jsMapInfos& mapInfos = cit->second;
		CRegParam::S_MAP_OPTION mapOption;

		mapOption.bMap = mapInfos[jsMapInfos::E_MAP].isSelected();
		mapOption.bSat = mapInfos[jsMapInfos::E_SAT].isSelected();
		mapOption.bHybrid = mapInfos[jsMapInfos::E_HYBRID].isSelected();
		mapOption.bTopo = mapInfos[jsMapInfos::E_TOPO].isSelected();

		mapMapsOptions[mapInfos.flag()] = mapOption;
	}

	CITNConverterApp::RegParam().WriteMapsOptions(mapMapsOptions); // Write map options to registry
#endif

#ifdef IDC_CHECK_AUTO_CALC
	CITNConverterApp::RegParam().AutoCalc() = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_AUTO_CALC))->GetCheck() == BST_CHECKED);
#endif

#ifdef IDC_CHECK_DIST_MILES
	CITNConverterApp::RegParam().DistMiles() = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_DIST_MILES))->GetCheck() == BST_CHECKED);
#endif

#ifdef IDC_CHECK_FORCE_ROAD
	CITNConverterApp::RegParam().ForceRoad() = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_FORCE_ROAD))->GetCheck() == BST_CHECKED);
#endif

#ifdef IDC_CHECK_DISPLAY_NUMBER
	CITNConverterApp::RegParam().DisplayNumber() = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_DISPLAY_NUMBER))->GetCheck() == BST_CHECKED);
#endif

#ifdef IDC_CHECK_AUTO_ADD_STEP
	CITNConverterApp::RegParam().AutoAddStep() = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_AUTO_ADD_STEP))->GetCheck() == BST_CHECKED);
#endif

#ifdef IDC_CHECK_AVOID_HIGHWAY
	CITNConverterApp::RegParam().AvoidHighway() = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_AVOID_HIGHWAY))->GetCheck() == BST_CHECKED) && (((CButton*)GetDlgItem(IDC_CHECK_AVOID_HIGHWAY))->IsWindowEnabled() == TRUE);
#endif

#ifdef IDC_CHECK_AVOID_TOLLS
	CITNConverterApp::RegParam().AvoidTolls() = (static_cast<CButton*>(GetDlgItem(IDC_CHECK_AVOID_TOLLS))->GetCheck() == BST_CHECKED) && (((CButton*)GetDlgItem(IDC_CHECK_AVOID_TOLLS))->IsWindowEnabled() == TRUE);
#endif

#ifdef IDC_EDIT_USER_ID
	CITNConverterApp::RegParam().UserId() = (LPCTSTR)m_csUserId;
#endif

#ifdef IDC_COMBO_ROUTE_PROVIDER
	getComboBoxValue(GetDlgItem(IDC_COMBO_ROUTE_PROVIDER), CITNConverterApp::RegParam().RouteProvider());
#endif

#ifdef IDC_COMBO_VEHICLE_TYPE
	getComboBoxValue(GetDlgItem(IDC_COMBO_VEHICLE_TYPE), CITNConverterApp::RegParam().VehicleType());
#endif

#ifdef IDC_COMBO_ITINERARY_TYPE
	getComboBoxValue(GetDlgItem(IDC_COMBO_ITINERARY_TYPE), CITNConverterApp::RegParam().ItineraryType());
#endif

#ifdef IDC_COMBO_COORDS_FORMAT
	getComboBoxValue(GetDlgItem(IDC_COMBO_COORDS_FORMAT), CITNConverterApp::RegParam().Coordinates());
#endif

	if (m_pAcceptedRouteOptions->typeIs(geo::GeoRouteOptionsType::Truck))
	{
#ifdef IDC_EDIT_TRUCK_HEIGHT
		const geo::CGeoAcceptedTruckRouteOptions& options = static_cast<const geo::CGeoAcceptedTruckRouteOptions&>(*m_pAcceptedRouteOptions);

		auto checkAndSaveTruckValue = [&](int nItemID, UINT msgError, double& dValue, unsigned int maxValue, unsigned int& regValue) -> bool
		{
			unsigned int uiTruckValue = static_cast<int>(dValue * 10);
			if (uiTruckValue < 0 || uiTruckValue > maxValue)
			{
				if (uiTruckValue > maxValue)
					dValue = static_cast<double>(static_cast<int>(maxValue)) / 10;
				else
					dValue = 0;

				CString resStr;
				resStr.Format(msgError, 0, maxValue / 10);
				AfxMessageBox(resStr, MB_OK | MB_ICONSTOP);

				CWnd* pCwd = GetDlgItem(nItemID);
				if (pCwd)
					pCwd->SetFocus();

				return false;
			}

			regValue = uiTruckValue;
			return true;
		};
#endif

#ifdef IDC_COMBO_TRUCK_CATEGORY
		getComboBoxValue(GetDlgItem(IDC_COMBO_TRUCK_CATEGORY), CITNConverterApp::RegParam().TruckCategory());
#endif

#ifdef IDC_COMBO_TRUCK_TRAILERS
		getComboBoxValue(GetDlgItem(IDC_COMBO_TRUCK_TRAILERS), CITNConverterApp::RegParam().TruckTrailersCount());
#endif

#ifdef IDC_EDIT_TRUCK_HEIGHT
		bOk &= checkAndSaveTruckValue(IDC_EDIT_TRUCK_HEIGHT, IDS_ERR_TRUCK_HEIGHT, m_dTruckHeight, options.height().acceptedValue, CITNConverterApp::RegParam().TruckHeight());
#endif
#ifdef IDC_EDIT_TRUCK_WIDTH
		bOk &= checkAndSaveTruckValue(IDC_EDIT_TRUCK_WIDTH, IDS_ERR_TRUCK_WIDTH, m_dTruckWidth, options.width().acceptedValue, CITNConverterApp::RegParam().TruckWidth());
#endif
#ifdef IDC_EDIT_TRUCK_LENGTH
		bOk &= checkAndSaveTruckValue(IDC_EDIT_TRUCK_LENGTH, IDS_ERR_TRUCK_LENGTH, m_dTruckLength, options.length().acceptedValue, CITNConverterApp::RegParam().TruckLength());
#endif
#ifdef IDC_EDIT_TRUCK_LIMITED_WEIGHT
		bOk &= checkAndSaveTruckValue(IDC_EDIT_TRUCK_LIMITED_WEIGHT, IDS_ERR_TRUCK_LIMITED_WEIGHT, m_dTruckLimitedWeight, options.limitedWeight().acceptedValue, CITNConverterApp::RegParam().TruckLimitedWeight());
#endif
#ifdef IDC_EDIT_TRUCK_WEIGHT_AXLE
		bOk &= checkAndSaveTruckValue(IDC_EDIT_TRUCK_WEIGHT_AXLE, IDS_ERR_TRUCK_WEIGHT_AXLE, m_dTruckWeightAxle, options.weightPerAxle().acceptedValue, CITNConverterApp::RegParam().TruckWeightAxle());
#endif
	}

	if (bOk)
		CDialog::OnOK();
	else
		UpdateData(FALSE);
}

void CConfigDlg::OnSelchangeComboRouteProvider()
{
#ifdef IDC_COMBO_ROUTE_PROVIDER
	CComboBox* pComboBox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_ROUTE_PROVIDER));
	if (pComboBox)
	{
		int nIndex = pComboBox->GetCurSel();
		if (nIndex == -1)
		{
			nIndex = 0;
			pComboBox->SetCurSel(0);
		}

		geo::E_GEO_PROVIDER eGeoProvider = static_cast<geo::E_GEO_PROVIDER>(pComboBox->GetItemData(nIndex));
		geo::CGeoDirections geoDirections(eGeoProvider);
		m_CurrentTravelOptions = geoDirections->getSupportedTravelOptions();
	}
#endif // IDC_COMBO_ROUTE_PROVIDER

#ifdef IDC_COMBO_VEHICLE_TYPE
	FillComboBox(GetDlgItem(IDC_COMBO_VEHICLE_TYPE), cs_VehiculeType, m_CurrentTravelOptions, CITNConverterApp::RegParam().VehicleType());
	OnSelchangeComboVehicleType();
#endif
}

void CConfigDlg::OnSelchangeComboVehicleType()
{
#ifdef IDC_COMBO_VEHICLE_TYPE
	CComboBox* pComboBox = static_cast<CComboBox*>(GetDlgItem(IDC_COMBO_VEHICLE_TYPE));
	int nIndex = pComboBox->GetCurSel();
	if (nIndex == -1)
	{
		nIndex = 0;
		pComboBox->SetCurSel(0);
	}

	geo::GeoVehicleType::type_t vehicleType = static_cast<geo::GeoVehicleType::type_t>(pComboBox->GetItemData(nIndex));
	auto it = m_CurrentTravelOptions.find(vehicleType);
	if (it != m_CurrentTravelOptions.end())
		m_pAcceptedRouteOptions = it->second.get();
	else if (!m_CurrentTravelOptions.empty())
		m_pAcceptedRouteOptions = m_CurrentTravelOptions.begin()->second.get();

#ifdef IDC_COMBO_ITINERARY_TYPE
	FillComboBox(GetDlgItem(IDC_COMBO_ITINERARY_TYPE), cs_ItineraryType, m_pAcceptedRouteOptions->itineraries().acceptedValue, CITNConverterApp::RegParam().ItineraryType());
#endif

	// Road
	bool bHighway = false;
	bool bTolls = false;

	if (m_pAcceptedRouteOptions->typeIs(geo::GeoRouteOptionsType::Road))
	{
		const geo::CGeoAcceptedRoadRouteOptions& options = static_cast<const geo::CGeoAcceptedRoadRouteOptions&>(*m_pAcceptedRouteOptions);
		bHighway = options.highway().acceptedValue;
		bTolls = options.tolls().acceptedValue;
	}

#ifdef IDC_CHECK_AVOID_HIGHWAY
	enableDlgItemWindow(GetDlgItem(IDC_CHECK_AVOID_HIGHWAY), bHighway);
#endif
#ifdef IDC_CHECK_AVOID_TOLLS
	enableDlgItemWindow(GetDlgItem(IDC_CHECK_AVOID_TOLLS), bTolls);
#endif

	// Truck
	bool bTruck = m_pAcceptedRouteOptions->typeIs(geo::GeoRouteOptionsType::Truck);
	unsigned int maxTruckTrailers = 0;
	std::set<geo::GeoTruckCategoryType::type_t> truckCategories;

	if (bTruck)
	{
		const geo::CGeoAcceptedTruckRouteOptions& options = static_cast<const geo::CGeoAcceptedTruckRouteOptions&>(*m_pAcceptedRouteOptions);
		maxTruckTrailers = options.trailersCount().acceptedValue;
		truckCategories = options.categories().acceptedValue;
	}

#ifdef IDC_STATIC_OPT_TRUCK
	enableDlgItemWindow(GetDlgItem(IDC_STATIC_OPT_TRUCK), bTruck);
#endif
#ifdef IDC_EDIT_TRUCK_HEIGHT
	enableDlgItemWindow(GetDlgItem(IDC_EDIT_TRUCK_HEIGHT), bTruck);
#endif
#ifdef IDC_EDIT_TRUCK_WIDTH
	enableDlgItemWindow(GetDlgItem(IDC_EDIT_TRUCK_WIDTH), bTruck);
#endif
#ifdef IDC_EDIT_TRUCK_LENGTH
	enableDlgItemWindow(GetDlgItem(IDC_EDIT_TRUCK_LENGTH), bTruck);
#endif
#ifdef IDC_EDIT_TRUCK_LIMITED_WEIGHT
	enableDlgItemWindow(GetDlgItem(IDC_EDIT_TRUCK_LIMITED_WEIGHT), bTruck);
#endif
#ifdef IDC_EDIT_TRUCK_WEIGHT_AXLE
	enableDlgItemWindow(GetDlgItem(IDC_EDIT_TRUCK_WEIGHT_AXLE), bTruck);
#endif

#ifdef IDC_COMBO_TRUCK_CATEGORY
	FillComboBox(GetDlgItem(IDC_COMBO_TRUCK_CATEGORY), cs_TruckCategoryType, truckCategories, CITNConverterApp::RegParam().TruckCategory());
#endif

#ifdef IDC_COMBO_TRUCK_TRAILERS
	std::vector<comboBoxPair<unsigned int>> truckTrailersList;
	for (unsigned int i = 0; i < maxTruckTrailers; ++i)
		truckTrailersList.push_back(comboBoxPair<unsigned int>{i, 0});

	FillComboBox(GetDlgItem(IDC_COMBO_TRUCK_TRAILERS), truckTrailersList, AcceptAll<unsigned int>(), CITNConverterApp::RegParam().TruckTrailersCount());
#endif
#endif // IDC_COMBO_VEHICLE_TYPE
}

void CConfigDlg::OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;

	switch (pLVKeyDow->wVKey)
	{
	case VK_RETURN:
	case VK_SPACE:
		break;

	case VK_SUBTRACT:
		//		OnButtonUp();
		break;

	case VK_ADD:
		//		OnButtonDown();
		break;

	default:
		break;
	}

	*pResult = 0;
}

#ifdef IDC_TREE_MAPS
// http://support.microsoft.com/kb/261289/en-us
void CConfigDlg::OnClickTreeMaps(NMHDR*, LRESULT* pResult)
{
	UINT flags;

	// verify that we have a mouse click in the check box area
	DWORD pos = GetMessagePos();
	CPoint point(LOWORD(pos), HIWORD(pos));
	m_TreeMaps.ScreenToClient(&point);
	HTREEITEM item = m_TreeMaps.HitTest(point, &flags);

	if (item && (flags & TVHT_ONITEMSTATEICON))
		PostMessage(TREE_MAPS_CHECK_STATE_CHANGE, 0, (LPARAM)item);

	*pResult = 0;
}

LRESULT CConfigDlg::OnTreeMapsCheckStateChange(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(wParam);
	HTREEITEM hItemChanged = (HTREEITEM)lParam;

	TreeCtrlCheckChildren(m_TreeMaps, hItemChanged);
	TreeCtrlCheckParent(m_TreeMaps, hItemChanged);

	return 0;
}

void CConfigDlg::OnSelectAllMaps()
{
	TreeCtrlCheckAll(m_TreeMaps, TRUE);
}

void CConfigDlg::OnUnselectAllMaps()
{
	TreeCtrlCheckAll(m_TreeMaps, FALSE);
}
#endif
