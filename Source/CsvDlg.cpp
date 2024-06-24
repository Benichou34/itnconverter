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
 * Purpose : Dialog Windows for open or export csv files
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "ITN Converter.h"
#include "CsvDlg.h"

static const CCsvDlg::_CSV_CONFIG gs_CsvTemplate[] =
{
	{IDS_DEFAULT, 2, 1, 0, 3, 0, false, {'.', '\0'}, {',', '\0'}},
	{IDS_INFORAD, 1, 2, 0, 3, 0, false, {',', '\0'}, {';', '\0'}},
	{IDS_PIONEER, 2, 1, 0, 3, 0, false, {'.', '\0'}, {';', '\0'}},
	{IDS_DESTINATOR, 4, 3, 0, 6, 7, false, {'.', '\0'}, {',', '\0'}},
	{IDS_CUT_ITN_CUSTOM, -1, -1, -1, -1, -1, false, {0x00}, {0x00}},

	{0, -1, -1, -1, -1, -1, false, {0x00}, {0x00}}
};

/////////////////////////////////////////////////////////////////////////////
// CCsvDlg dialog

CCsvDlg::CCsvDlg(_CSV_CONFIG& csvConfig, bool bRead, CWnd* pParent /*=nullptr*/)
	: CDialog(CCsvDlg::IDD, pParent),
	m_bRead(bRead),
	m_csvConfig(csvConfig)
{
	CITNConverterApp::RegParam().getStorageFolder()(_T("CSV"))[m_bRead ? _T("read") : _T("write")].asBinary(&m_csvConfig, sizeof(_CSV_CONFIG), 0x00);
}

void CCsvDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCsvDlg)
	DDX_Control(pDX, IDC_COMBO_TEMPLATE, m_cbTemplate);
	DDX_Control(pDX, IDC_COMBO_LATITUDE, m_cbLatitude);
	DDX_Control(pDX, IDC_COMBO_LONGITUDE, m_cbLongitude);
	DDX_Control(pDX, IDC_COMBO_ALTITUDE, m_cbAltitude);
	DDX_Control(pDX, IDC_COMBO_ADDRESS, m_cbAddress);
	DDX_Control(pDX, IDC_COMBO_SNIPPET, m_cbSnippet);
	DDX_Control(pDX, IDC_CHECK_LOCALE, m_btCheckLocale);

	DDX_Text(pDX, IDC_EDIT_SDECIMAL, m_csSDecimal);
	DDV_MaxChars(pDX, m_csSDecimal, 3);
	DDX_Text(pDX, IDC_EDIT_SLIST, m_csSList);
	DDV_MaxChars(pDX, m_csSList, 3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCsvDlg, CDialog)
	//{{AFX_MSG_MAP(CCsvDlg)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_COMBO_TEMPLATE, OnSelchangeComboTemplate)
	ON_BN_CLICKED(IDC_CHECK_LOCALE, OnClickCheckLocale)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCsvDlg message handlers
BOOL CCsvDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Labels
	GetDlgItem(IDC_STATIC_TEMPLATE)->SetWindowText(CWToolsString::Load(IDS_TEMPLATE).c_str());
	GetDlgItem(IDC_STATIC_LONGITUDE)->SetWindowText(CWToolsString::Load(IDS_LONGITUDE).c_str());
	GetDlgItem(IDC_STATIC_LATITUDE)->SetWindowText(CWToolsString::Load(IDS_LATITUDE).c_str());
	GetDlgItem(IDC_STATIC_ALTITUDE)->SetWindowText(CWToolsString::Load(IDS_ALTITUDE).c_str());
	GetDlgItem(IDC_STATIC_ADDRESS)->SetWindowText(CWToolsString::Load(IDS_ADDRESS).c_str());
	GetDlgItem(IDC_STATIC_SNIPPET)->SetWindowText(CWToolsString::Load(IDS_SNIPPET).c_str());

	GetDlgItem(IDC_CHECK_LOCALE)->SetWindowText(CWToolsString::Load(IDS_LOCALE_SETTINGS).c_str());
	GetDlgItem(IDC_STATIC_SDECIMAL)->SetWindowText(CWToolsString::Load(IDS_LOCALE_SDECIMAL).c_str());
	GetDlgItem(IDC_STATIC_SLIST)->SetWindowText(CWToolsString::Load(IDS_LOCALE_SLIST).c_str());

	int i = 0;
	while (gs_CsvTemplate[i].nTemplate)
	{
		m_cbTemplate.AddString(CWToolsString::Load(gs_CsvTemplate[i].nTemplate).c_str());
		i++;
	}

	std::wstring strCol;
	CWToolsString::Load(IDS_NONE, strCol);

	m_cbLatitude.AddString(strCol.c_str());
	m_cbLongitude.AddString(strCol.c_str());
	m_cbAltitude.AddString(strCol.c_str());
	m_cbAddress.AddString(strCol.c_str());
	m_cbSnippet.AddString(strCol.c_str());

	for (i = 1; i < 20; i++)
	{
		strCol = stdx::wformat(CWToolsString::Load(IDS_COLUMN))(i).str();

		m_cbLatitude.AddString(strCol.c_str());
		m_cbLongitude.AddString(strCol.c_str());
		m_cbAltitude.AddString(strCol.c_str());
		m_cbAddress.AddString(strCol.c_str());
		m_cbSnippet.AddString(strCol.c_str());
	}

	// Parametres par defaut
	m_cbLongitude.SetCurSel(m_csvConfig.nColLongitude);
	m_cbLatitude.SetCurSel(m_csvConfig.nColLatitude);
	m_cbAltitude.SetCurSel(m_csvConfig.nColAltitude);
	m_cbAddress.SetCurSel(m_csvConfig.nColAddress);
	m_cbSnippet.SetCurSel(m_csvConfig.nColSnippet);
	m_btCheckLocale.SetCheck(m_csvConfig.bUseLocale);
	m_csSDecimal = m_csvConfig.szSDecimal;
	m_csSList = m_csvConfig.szSList;

	if (m_cbTemplate.SetCurSel(m_csvConfig.nTemplate) == CB_ERR)
		m_cbTemplate.SetCurSel(0);
	OnSelchangeComboTemplate();

	// Change title
	strCol = CWToolsString::Load(IDS_CFG_SETTINGS) + _T(" CSV");
	SetWindowText(strCol.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCsvDlg::OnOK()
{
	UpdateData();

	m_csvConfig.nTemplate = m_cbTemplate.GetCurSel();
	m_csvConfig.nColLatitude = m_cbLatitude.GetCurSel();
	m_csvConfig.nColLongitude = m_cbLongitude.GetCurSel();
	m_csvConfig.nColAltitude = m_cbAltitude.GetCurSel();
	m_csvConfig.nColAddress = m_cbAddress.GetCurSel();
	m_csvConfig.nColSnippet = m_cbSnippet.GetCurSel();

	m_csvConfig.bUseLocale = (m_btCheckLocale.GetCheck() == BST_CHECKED);
	strncpy(m_csvConfig.szSDecimal, stdx::string_helper::narrow((LPCTSTR)m_csSDecimal.GetBuffer(3)).c_str(), sizeof(m_csvConfig.szSDecimal));
	strncpy(m_csvConfig.szSList, stdx::string_helper::narrow((LPCTSTR)m_csSList.GetBuffer(3)).c_str(), sizeof(m_csvConfig.szSList));

	CITNConverterApp::RegParam().getStorageFolder()(_T("CSV"))[m_bRead ? _T("read") : _T("write")].toBinary(&m_csvConfig, sizeof(_CSV_CONFIG));

	CDialog::OnOK();
}

void CCsvDlg::OnSelchangeComboTemplate()
{
	int nTemplate = m_cbTemplate.GetCurSel();
	bool bCustom = (gs_CsvTemplate[nTemplate].nTemplate == IDS_CUT_ITN_CUSTOM);

	if (!bCustom)
	{
		m_cbLongitude.SetCurSel(gs_CsvTemplate[nTemplate].nColLongitude);
		m_cbLatitude.SetCurSel(gs_CsvTemplate[nTemplate].nColLatitude);
		m_cbAltitude.SetCurSel(gs_CsvTemplate[nTemplate].nColAltitude);
		m_cbAddress.SetCurSel(gs_CsvTemplate[nTemplate].nColAddress);
		m_cbSnippet.SetCurSel(gs_CsvTemplate[nTemplate].nColSnippet);

		m_btCheckLocale.SetCheck(gs_CsvTemplate[nTemplate].bUseLocale);
	}

	m_cbLongitude.EnableWindow(bCustom);
	m_cbLatitude.EnableWindow(bCustom);
	m_cbAltitude.EnableWindow(bCustom);
	m_cbAddress.EnableWindow(bCustom);
	m_cbSnippet.EnableWindow(bCustom);
	m_btCheckLocale.EnableWindow(bCustom);

	OnClickCheckLocale();
}

void CCsvDlg::OnClickCheckLocale()
{
	TCHAR szLocaleInfo[4];
	int nTemplate = m_cbTemplate.GetCurSel();
	bool bCustom = (gs_CsvTemplate[nTemplate].nTemplate == IDS_CUT_ITN_CUSTOM);
	bool bUselocale = (m_btCheckLocale.GetCheck() == BST_CHECKED);

	if (bUselocale)
	{
		if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szLocaleInfo, sizeof(szLocaleInfo)) != 0)
			m_csSDecimal = szLocaleInfo;
		if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, szLocaleInfo, sizeof(szLocaleInfo)) != 0)
			m_csSList = szLocaleInfo;
	}
	else if (!bCustom)
	{
		m_csSDecimal = gs_CsvTemplate[nTemplate].szSDecimal;
		m_csSList = gs_CsvTemplate[nTemplate].szSList;
	}

	GetDlgItem(IDC_EDIT_SDECIMAL)->EnableWindow(!bUselocale && bCustom);
	GetDlgItem(IDC_EDIT_SLIST)->EnableWindow(!bUselocale && bCustom);

	UpdateData(FALSE);
}
