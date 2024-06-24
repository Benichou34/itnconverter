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

#include "stdafx.h"
#include "CustomizableDlg.h"

CCustomizableDialog::CCustomizableDialog(UINT nIDTemplate, CWnd* pParentWnd) :
	CDialog(nIDTemplate, pParentWnd),
	m_size(0, 0)
{
}

CCustomizableDialog::~CCustomizableDialog()
{
}

BEGIN_MESSAGE_MAP(CCustomizableDialog, CDialog)
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CCustomizableDialog::SetBackgroundColor(const COLORREF& background)
{
	m_backgroundBrush = std::make_unique<CBrush>(background);
}

void CCustomizableDialog::AddBackgroundImage(UINT res, int top, int left, size_t eProperties)
{
	backBitmap bBtmp;
	bBtmp.top = top;
	bBtmp.left = left;
	bBtmp.eProperties = eProperties;
	bBtmp.pBitmap = std::make_unique<CBitmap>();
	bBtmp.pBitmap->LoadBitmap(res); //Load bitmap

	m_lstBitmapBackground.push_back(std::move(bBtmp));
}

BOOL CCustomizableDialog::OnInitDialog()
{
	return CDialog::OnInitDialog();
}

void CCustomizableDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (!m_size.cx && !m_size.cy)
	{
		m_size.cx = cx;
		m_size.cy = cy;
		return;
	}

	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
	{
		int offcx = cx - m_size.cx;
		int offcy = cy - m_size.cy;

		for (auto it = m_lstBitmapBackground.begin(); it != m_lstBitmapBackground.end(); ++it)
		{
			if (it->eProperties & E_RESIZE_PROP_LEFT)
				it->left += offcx;

			if (it->eProperties & E_RESIZE_PROP_TOP)
				it->top += offcy;
		}

		m_size.cx = cx;
		m_size.cy = cy;
	}
}

BOOL CCustomizableDialog::OnEraseBkgnd(CDC* pDC)
{
	BOOL ret = CDialog::OnEraseBkgnd(pDC);

	for (auto it = m_lstBitmapBackground.begin(); it != m_lstBitmapBackground.end(); ++it)
	{
		BITMAP bm; //Create bitmap Handle to get dimensions
		it->pBitmap->GetBitmap(&bm); //Load bitmap into handle

		CDC dcMemory;
		dcMemory.CreateCompatibleDC(pDC);
		CBitmap* pOldbitmap = dcMemory.SelectObject(it->pBitmap.get());
		pDC->BitBlt(it->left, it->top, bm.bmWidth, bm.bmHeight, &dcMemory, 0, 0, SRCCOPY);
		dcMemory.SelectObject(pOldbitmap);
	}

	return ret;
}

HBRUSH CCustomizableDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (pWnd->GetExStyle() & WS_EX_TRANSPARENT)
		pDC->SetBkMode(TRANSPARENT);

	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
		if (m_backgroundBrush)
			return *m_backgroundBrush;
		else
			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	case CTLCOLOR_STATIC:
		//	case CTLCOLOR_BTN:
		if (pWnd->GetExStyle() & WS_EX_TRANSPARENT)
		{
			if (m_backgroundBrush)
				return *m_backgroundBrush;
			else
				return (HBRUSH)GetStockObject(NULL_BRUSH);
		}
		else
			return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	default:
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}

}
