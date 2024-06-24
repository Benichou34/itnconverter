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
#include "ProgressDlg.h"

CProgressDlg::~CProgressDlg()
{
	Close();
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROGRESS_MESSAGE, m_LabelMessage);
	DDX_Control(pDX, IDC_PROGRESS_ICON, m_IconMessage);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_ProgressBar);
}

bool CProgressDlg::DoEvents()
{
	MSG msg;

	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		DispatchMessage(&msg);

	return (GetDlgItem(IDCANCEL)->IsWindowEnabled() == FALSE);
}

BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetPos(-1);
	m_LabelMessage.SetWindowText(_T(""));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CProgressDlg::OnCancel()
{
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	CDialog::OnCancel();
}

void CProgressDlg::SetIcon(UINT uiResIcon)
{
	if (m_hWnd)
	{
		m_IconMessage.SetIcon((HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(uiResIcon), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR));
		UpdateWindow();
	}
}

void CProgressDlg::SetMessage(LPCTSTR szMesg)
{
	if (m_hWnd)
	{
		m_LabelMessage.SetWindowText(szMesg);
		UpdateWindow();
	}
}

void CProgressDlg::SetRange(int nlower, int nUpper, int nStep)
{
	if (m_hWnd)
	{
		m_ProgressBar.SetRange32(nlower, nUpper);
		m_ProgressBar.SetStep(nStep);
		m_ProgressBar.SetPos(nlower);
	}
}

void CProgressDlg::SetPos(int nPos)
{
	if (m_hWnd)
		m_ProgressBar.SetPos(nPos);
}

int CProgressDlg::GetPos()
{
	return m_hWnd ? m_ProgressBar.GetPos() : -1;
}

int CProgressDlg::StepIt()
{
	return m_hWnd ? m_ProgressBar.StepIt() : -1;
}

void CProgressDlg::Display(LPCTSTR szTitle, bool bCanceable, CWnd* pParent)
{
	Create(IDD_PROGRESS_DLG, pParent);

	if (!bCanceable)
	{
		RECT cancelRect;
		GetDlgItem(IDCANCEL)->GetWindowRect(&cancelRect);

		RECT rect;
		GetWindowRect(&rect);
		SetWindowPos(nullptr, 0, 0, rect.right - rect.left, cancelRect.top - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	}

	SetWindowText(szTitle);
	CenterWindow();
	ShowWindow(SW_SHOW);
	UpdateWindow();
}

void CProgressDlg::Close()
{
	if (m_hWnd)
	{
		CDialog::OnOK();
		DestroyWindow();
	}
}
