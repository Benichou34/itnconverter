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
 * Purpose : Dialog Windows for insert or modify waypoints
 */

#include "stdafx.h"
#include "ITN Converter.h"
#include "ITN Tools.h"
#include "InsertModify.h"

#define MIN_LATITUDE   -90
#define MAX_LATITUDE    90
#define MIN_LONGITUDE -180
#define MAX_LONGITUDE  180

#define COORDS_DD	0
#define COORDS_DM	1
#define COORDS_DMS	2

 /////////////////////////////////////////////////////////////////////////////
 // CInsertModify dialog


CInsertModify::CInsertModify(CWnd* pParent /*=nullptr*/)
	: CSizeableDialog(CInsertModify::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertModify)
	m_dLatitude = 0;
	m_lLatDegrees = 0;
	m_dLatMinutes = 0;
	m_lLatMinutes = 0;
	m_dLatSeconds = 0;
	m_dLongitude = 0;
	m_lLngDegrees = 0;
	m_dLngMinutes = 0;
	m_lLngMinutes = 0;
	m_dLngSeconds = 0;
	m_dAltitude = 0;
	m_csAddress = _T("");
	m_csSnippet = _T("");
	//}}AFX_DATA_INIT
}

void CInsertModify::DoDataExchange(CDataExchange* pDX)
{
	CSizeableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertModify)
	DDX_Control(pDX, IDC_COMBO_COORD_FORMAT, m_cbCoordFormat);
	DDX_Control(pDX, IDC_COMBO_LAT_NS, m_cbLatNS);
	DDX_Control(pDX, IDC_COMBO_LNG_WE, m_cbLngEW);

	DDX_Text(pDX, IDC_EDIT_LATITUDE, m_dLatitude);
	DDX_Text(pDX, IDC_EDIT_LAT_DEG, m_lLatDegrees);
	DDX_Text(pDX, IDC_EDIT_LAT_DM_MIN, m_dLatMinutes);
	DDX_Text(pDX, IDC_EDIT_LAT_DMS_MIN, m_lLatMinutes);
	DDX_Text(pDX, IDC_EDIT_LAT_SEC, m_dLatSeconds);

	DDX_Text(pDX, IDC_EDIT_LONGITUDE, m_dLongitude);
	DDX_Text(pDX, IDC_EDIT_LNG_DEG, m_lLngDegrees);
	DDX_Text(pDX, IDC_EDIT_LNG_DM_MIN, m_dLngMinutes);
	DDX_Text(pDX, IDC_EDIT_LNG_DMS_MIN, m_lLngMinutes);
	DDX_Text(pDX, IDC_EDIT_LNG_SEC, m_dLngSeconds);

	DDX_Text(pDX, IDC_EDIT_ALTITUDE, m_dAltitude);
	DDX_Text(pDX, IDC_EDIT_SNIPPET, m_csSnippet);
	DDX_Text(pDX, IDC_EDIT_ADDRESS, m_csAddress);

	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CInsertModify, CSizeableDialog)
	//{{AFX_MSG_MAP(CInsertModify)
	ON_CBN_SELCHANGE(IDC_COMBO_COORD_FORMAT, OnSelchangeComboCoordinates)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertModify message handlers

BOOL CInsertModify::OnInitDialog()
{
	CString resStr;

	CSizeableDialog::OnInitDialog();

	// Labels
	resStr.LoadString(IDS_COORD_FORMAT);
	GetDlgItem(IDC_STATIC_COORD_FORMAT)->SetWindowText(resStr);

	resStr.LoadString(IDS_LONGITUDE);
	GetDlgItem(IDC_STATIC_LONGITUDE)->SetWindowText(resStr);

	resStr.LoadString(IDS_LATITUDE);
	GetDlgItem(IDC_STATIC_LATITUDE)->SetWindowText(resStr);

	resStr.LoadString(IDS_ALTITUDE);
	GetDlgItem(IDC_STATIC_ALTITUDE)->SetWindowText(resStr);

	resStr.LoadString(IDS_ADDRESS);
	GetDlgItem(IDC_STATIC_ADDRESS)->SetWindowText(resStr);

	resStr.LoadString(IDS_SNIPPET);
	GetDlgItem(IDC_STATIC_SNIPPET)->SetWindowText(resStr);

	AddSizeableControl(GetDlgItem(IDOK), E_RESIZE_PROP_LEFT);
	AddSizeableControl(GetDlgItem(IDCANCEL), E_RESIZE_PROP_LEFT);
	AddSizeableControl(GetDlgItem(IDC_EDIT_ADDRESS), E_RESIZE_PROP_WIDTH);
	AddSizeableControl(GetDlgItem(IDC_EDIT_SNIPPET), E_RESIZE_PROP_WIDTH | E_RESIZE_PROP_HEIGHT);

	resStr.LoadString(IDS_DECIMAL_DEGREES);
	m_cbCoordFormat.AddString(resStr);
	resStr.LoadString(IDS_DEGREES_MINUTES);
	m_cbCoordFormat.AddString(resStr);
	resStr.LoadString(IDS_DEGREES_MINUTES_SECONDS);
	m_cbCoordFormat.AddString(resStr);

	m_cbLatNS.AddString(_T("N"));
	m_cbLatNS.AddString(_T("S"));
	m_cbLngEW.AddString(_T("E"));
	m_cbLngEW.AddString(_T("W"));

	if (m_cbCoordFormat.SetCurSel(CITNConverterApp::RegParam().Coordinates()) == CB_ERR)
		m_cbCoordFormat.SetCurSel(COORDS_DD);

	// Fill edit fields
	if (!(*m_pcGpsPoint))
	{
		resStr.LoadString(IDS_ADD);
	}
	else
	{
		m_gLatLngDegrees = *m_pcGpsPoint;
		m_dAltitude = m_pcGpsPoint->alt();
		m_csAddress = stdx::wstring_helper::from_utf8(m_pcGpsPoint->name()).c_str();
		m_csSnippet = stdx::wstring_helper::from_utf8(m_pcGpsPoint->comment()).c_str();

		resStr.LoadString(IDS_MODIFY);
	}

	OnSelchangeComboCoordinates();

	// Change title
	SetWindowText(resStr);
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool CInsertModify::UpdateAndCheckCoords(int nIndex)
{
	if (UpdateData() == FALSE)
		return false;

	switch (nIndex)
	{
	case COORDS_DD: // Decimal Degrees
		m_gLatLngDegrees.assign(m_dLatitude, m_dLongitude);
		break;

	case COORDS_DM: // DM
	{
		geo::CGeoLatLngDegrees::S_DM dmLat, dmLng;
		dmLat.bDirection = (m_cbLatNS.GetCurSel() > 0);
		dmLat.lDegrees = m_lLatDegrees;
		dmLat.dMinutes = m_dLatMinutes;

		dmLng.bDirection = (m_cbLngEW.GetCurSel() > 0);
		dmLng.lDegrees = m_lLngDegrees;
		dmLng.dMinutes = m_dLngMinutes;

		m_gLatLngDegrees.assign(dmLat, dmLng);
	}
	break;

	case COORDS_DMS: // DMS
	{
		geo::CGeoLatLngDegrees::S_DMS dmsLat, dmsLng;
		dmsLat.bDirection = (m_cbLatNS.GetCurSel() > 0);
		dmsLat.lDegrees = m_lLatDegrees;
		dmsLat.lMinutes = m_lLatMinutes;
		dmsLat.dSeconds = m_dLatSeconds;

		dmsLng.bDirection = (m_cbLngEW.GetCurSel() > 0);
		dmsLng.lDegrees = m_lLngDegrees;
		dmsLng.lMinutes = m_lLngMinutes;
		dmsLng.dSeconds = m_dLngSeconds;

		m_gLatLngDegrees.assign(dmsLat, dmsLng);
	}
	break;

	default:
		break;
	}

	const double& dLatitude = m_gLatLngDegrees.latLng().lat();
	const double& dLongitude = m_gLatLngDegrees.latLng().lng();

	if (dLatitude < MIN_LATITUDE || dLatitude > MAX_LATITUDE)
	{
		CString resStr;
		resStr.Format(IDS_ERR_LATITUDE, MIN_LATITUDE, MAX_LATITUDE);
		AfxMessageBox(resStr, MB_OK | MB_ICONSTOP);

		GetDlgItem(IDC_EDIT_LATITUDE)->SetFocus();
		return false;
	}

	if (dLongitude < MIN_LONGITUDE || dLongitude > MAX_LONGITUDE)
	{
		CString resStr;
		resStr.Format(IDS_ERR_LONGITUDE, MIN_LONGITUDE, MAX_LONGITUDE);
		AfxMessageBox(resStr, MB_OK | MB_ICONSTOP);

		GetDlgItem(IDC_EDIT_LONGITUDE)->SetFocus();
		return false;
	}

	return true;
}

void CInsertModify::OnOK()
{
	if (!UpdateAndCheckCoords(m_cbCoordFormat.GetCurSel()))
		return;

	*m_pcGpsPoint = m_gLatLngDegrees.latLng();
	m_pcGpsPoint->alt(m_dAltitude);
	m_pcGpsPoint->name(stdx::wstring_helper::to_utf8((LPCTSTR)m_csAddress));
	m_pcGpsPoint->comment(stdx::wstring_helper::to_utf8((LPCTSTR)m_csSnippet));

	CSizeableDialog::OnOK();
}

int CInsertModify::DoModal(CGpsPoint& cGpsPoint)
{
	m_pcGpsPoint = &cGpsPoint;

	return CSizeableDialog::DoModal();
}

void CInsertModify::OnSelchangeComboCoordinates()
{
	if (GetDlgItem(IDC_EDIT_LATITUDE)->IsWindowVisible() && GetDlgItem(IDC_EDIT_LATITUDE)->IsWindowVisible())
		UpdateAndCheckCoords(COORDS_DD);
	else if (GetDlgItem(IDC_EDIT_LAT_DM_MIN)->IsWindowVisible() && GetDlgItem(IDC_EDIT_LNG_DM_MIN)->IsWindowVisible())
		UpdateAndCheckCoords(COORDS_DM);
	else if (GetDlgItem(IDC_EDIT_LAT_DMS_MIN)->IsWindowVisible() && GetDlgItem(IDC_EDIT_LNG_DMS_MIN)->IsWindowVisible())
		UpdateAndCheckCoords(COORDS_DMS);

	switch (m_cbCoordFormat.GetCurSel())
	{
	case COORDS_DD: // Decimal Degrees
		GetDlgItem(IDC_EDIT_LATITUDE)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_COMBO_LAT_NS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LAT_DEG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LAT_DMS_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LAT_DM_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LAT_SEC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LAT_DEG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LAT_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LAT_SEC)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_EDIT_LONGITUDE)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_COMBO_LNG_WE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LNG_DEG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LNG_DMS_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LNG_DM_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LNG_SEC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LNG_DEG)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LNG_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LNG_SEC)->ShowWindow(SW_HIDE);

		{
			const geo::CGeoLatLng& gLatLgng = m_gLatLngDegrees.latLng();
			m_dLatitude = gLatLgng.lat();
			m_dLongitude = gLatLgng.lng();
		}
		break;

	case COORDS_DM: // DM
		GetDlgItem(IDC_EDIT_LATITUDE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LAT_NS)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LAT_DEG)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LAT_DMS_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LAT_DM_MIN)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LAT_SEC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LAT_DEG)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LAT_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LAT_SEC)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LAT_SEC)->SetWindowText(_T("'"));

		GetDlgItem(IDC_EDIT_LONGITUDE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LNG_WE)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LNG_DEG)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LNG_DMS_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LNG_DM_MIN)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LNG_SEC)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LNG_DEG)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LNG_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_LNG_SEC)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LNG_SEC)->SetWindowText(_T("'"));

		{
			geo::CGeoLatLngDegrees::S_DM dmLat = m_gLatLngDegrees.dmLat();
			m_cbLatNS.SetCurSel(dmLat.bDirection ? 1 : 0);
			m_lLatDegrees = dmLat.lDegrees;
			m_dLatMinutes = dmLat.dMinutes;

			geo::CGeoLatLngDegrees::S_DM dmLng = m_gLatLngDegrees.dmLng();
			m_cbLngEW.SetCurSel(dmLng.bDirection ? 1 : 0);
			m_lLngDegrees = dmLng.lDegrees;
			m_dLngMinutes = dmLng.dMinutes;
		}
		break;

	case COORDS_DMS: // DMS
		GetDlgItem(IDC_EDIT_LATITUDE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LAT_NS)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LAT_DEG)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LAT_DMS_MIN)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LAT_DM_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LAT_SEC)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LAT_DEG)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LAT_MIN)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LAT_SEC)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LAT_SEC)->SetWindowText(_T("\""));

		GetDlgItem(IDC_EDIT_LONGITUDE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_LNG_WE)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LNG_DEG)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LNG_DMS_MIN)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_EDIT_LNG_DM_MIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_LNG_SEC)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LNG_DEG)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LNG_MIN)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LNG_SEC)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_STATIC_LNG_SEC)->SetWindowText(_T("\""));

		{
			geo::CGeoLatLngDegrees::S_DMS dmsLat = m_gLatLngDegrees.dmsLat();
			m_cbLatNS.SetCurSel(dmsLat.bDirection ? 1 : 0);
			m_lLatDegrees = dmsLat.lDegrees;
			m_lLatMinutes = dmsLat.lMinutes;
			m_dLatSeconds = dmsLat.dSeconds;

			geo::CGeoLatLngDegrees::S_DMS dmsLng = m_gLatLngDegrees.dmsLng();
			m_cbLngEW.SetCurSel(dmsLng.bDirection ? 1 : 0);
			m_lLngDegrees = dmsLng.lDegrees;
			m_lLngMinutes = dmsLng.lMinutes;
			m_dLngSeconds = dmsLng.dSeconds;
		}
		break;

	default:
		break;
	}

	UpdateData(FALSE);
}