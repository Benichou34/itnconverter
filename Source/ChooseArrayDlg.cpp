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
 * Purpose : Dialog Windows for display various operation progress
 */

#include "stdafx.h"
#include "ITN Converter.h"
#include "ITN Tools.h"
#include "ChooseArrayDlg.h"

CChooseArrayDlg::CChooseArrayDlg(const std::wstring& strFileName, const std::vector<CGpsPointArray*>& vecGpsArray, CWnd* pParent) :
	CSizeableDialog(CChooseArrayDlg::IDD, pParent),
	m_vecGpsArray(vecGpsArray),
	m_strFileName(strFileName)
{
}

CChooseArrayDlg::~CChooseArrayDlg()
{
}

void CChooseArrayDlg::DoDataExchange(CDataExchange* pDX)
{
	CSizeableDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TREE_CHOOSE_ARRAY, m_TreeArray);
}

BOOL CChooseArrayDlg::OnInitDialog()
{
	CSizeableDialog::OnInitDialog();

	std::vector<HTREEITEM> vecRoots(CGpsPointArray::E_ARRAY_NB, nullptr);
	std::vector<CGpsPointArray*>::const_iterator cit;

	m_IconBitmap.LoadBitmap(IDB_CHOOSE_ICON);
	m_ImageList.Create(16, 16, ILC_COLOR32, 5, 1);
	m_ImageList.Add(&m_IconBitmap, RGB(0, 0, 0));

	m_TreeArray.SetImageList(&m_ImageList, TVSIL_NORMAL);
	AddSizeableControl(&m_TreeArray, E_RESIZE_PROP_WIDTH | E_RESIZE_PROP_HEIGHT);

	HTREEITEM hRoot = m_TreeArray.InsertItem(m_strFileName.c_str(), 2, 2);

	for (cit = m_vecGpsArray.begin(); cit != m_vecGpsArray.end(); ++cit)
	{
		CGpsPointArray* pGpsPointArray = *cit;
		if (!pGpsPointArray->empty())
		{
			CGpsPointArray::E_ARRAY_TYPE eArrayType = pGpsPointArray->getType();

			if (vecRoots[eArrayType] == nullptr)
			{
				unsigned int uID;
				int nImage;
				switch (eArrayType)
				{
				case CGpsPointArray::E_ARRAY_ROUTE:
					uID = IDS_ROUTE;
					nImage = 3;
					break;
				case CGpsPointArray::E_ARRAY_TRACK:
					uID = IDS_TRACK;
					nImage = 4;
					break;
				case CGpsPointArray::E_ARRAY_WPT:
					uID = IDS_WAYPOINTS;
					nImage = 5;
					break;
				case CGpsPointArray::E_ARRAY_POI:
					uID = IDS_POI;
					nImage = 6;
					break;
				default:
					uID = IDS_UNKNOWN;
					nImage = 5;
					break;
				}

				vecRoots[eArrayType] = m_TreeArray.InsertItem(CWToolsString::Load(uID).c_str(), nImage, nImage, hRoot);
			}

			std::wstring strName = stdx::wstring_helper::from_utf8(pGpsPointArray->name());
			if (strName.empty())
				CWToolsString::Load(IDS_NOTITLE, strName);

			HTREEITEM hItem = m_TreeArray.InsertItem(stdx::wformat(CWToolsString::Load(IDS_ARRAYDESC))(strName)(pGpsPointArray->size()).str().c_str(), 0, 1, vecRoots[eArrayType]);
			m_TreeArray.SetItemData(hItem, reinterpret_cast<DWORD>(pGpsPointArray));
		}
	}

	std::vector<HTREEITEM>::iterator it;
	for (it = vecRoots.begin(); it != vecRoots.end(); ++it)
	{
		if (*it != nullptr)
			m_TreeArray.Expand(*it, TVE_EXPAND);
	}
	m_TreeArray.Expand(hRoot, TVE_EXPAND);
	m_TreeArray.SelectItem(hRoot);

	CWnd* pWnd = GetDlgItem(IDC_STATIC);
	pWnd->SetWindowText(CWToolsString::Load(IDS_CHOOSE_LABEL).c_str());
	AddSizeableControl(pWnd, E_RESIZE_PROP_WIDTH);
	AddSizeableControl(GetDlgItem(IDOK), E_RESIZE_PROP_TOP | E_RESIZE_PROP_LEFT);
	AddSizeableControl(GetDlgItem(IDCANCEL), E_RESIZE_PROP_TOP);

	// Change title
	SetWindowText(CWToolsString::Load(IDS_CHOOSE_TITLE).c_str());
	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CChooseArrayDlg::DoModal()
{
	m_vecSelectedArray.clear();

	if (m_vecGpsArray.empty())
		return IDCANCEL;

	if (m_vecGpsArray.size() == 1)
	{
		m_vecSelectedArray = m_vecGpsArray;
		return IDOK;
	}

	return CSizeableDialog::DoModal();
}

void CChooseArrayDlg::AddItemToSelectedArray(HTREEITEM hItem)
{
	if (hItem != nullptr)
	{
		if (m_TreeArray.GetItemData(hItem) != 0)
			m_vecSelectedArray.push_back(reinterpret_cast<CGpsPointArray*>(m_TreeArray.GetItemData(hItem)));

		HTREEITEM hChildItem = m_TreeArray.GetChildItem(hItem);
		while (hChildItem != nullptr)
		{
			AddItemToSelectedArray(hChildItem);
			hChildItem = m_TreeArray.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
}

void CChooseArrayDlg::OnOK()
{
	AddItemToSelectedArray(m_TreeArray.GetSelectedItem());
	CSizeableDialog::OnOK();
}
