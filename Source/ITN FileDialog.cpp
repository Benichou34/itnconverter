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
 * Purpose : Custom file dialog
 */

#include "stdafx.h"
#include <dlgs.h>
#include "ITN Tools.h"
#include "ITN FileDialog.h"

#if (_WIN32_WINNT > _WIN32_WINNT_NT4) && (_MFC_VER < 0x0700)
#error _WIN32_WINNT must be equal to _WIN32_WINNT_NT4 with this version of MFC.
#endif

CITNFileDialog::CITNFileDialog(
	LPCTSTR lpszDefExt,
	LPCTSTR lpszFileName,
	LPCTSTR lpszFilter,
	CWnd* pParentWnd,
	BOOL bAppendCheckBox) : CFileDialog(
		TRUE,
		lpszDefExt,
		lpszFileName,
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
		lpszFilter,
		pParentWnd
#if _MFC_VER > 0x0700
		, 0        // Since Visual Studio 2008
		, FALSE
#endif
	)
{
	m_bAppend = false;

	if (bAppendCheckBox)
	{
		m_ofn.Flags |= OFN_ENABLETEMPLATE;
		m_ofn.lpTemplateName = MAKEINTRESOURCE(IDD_APPEND_FRAME);
		m_ofn.hInstance = AfxGetInstanceHandle();
	}
}

BOOL CITNFileDialog::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	if (m_ofn.Flags & OFN_ENABLETEMPLATE)
	{
		RECT wndRect;
		GetParent()->GetDlgItem(cmb1)->GetWindowRect(&wndRect);
		GetParent()->ScreenToClient(&wndRect);

		GetDlgItem(IDC_CHECK_APPEND)->SetWindowText(CWToolsString::Load(IDS_APPEND_ROUTE).c_str());
		GetDlgItem(IDC_CHECK_APPEND)->SetWindowPos(nullptr, wndRect.left, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CITNFileDialog::OnFileNameOK()
{
	if (m_ofn.Flags & OFN_ENABLETEMPLATE)
		m_bAppend = (((CButton*)GetDlgItem(IDC_CHECK_APPEND))->GetCheck() == BST_CHECKED);

	return FALSE;
}
