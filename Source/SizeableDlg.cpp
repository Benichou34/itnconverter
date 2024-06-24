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
 * Purpose : Auto sizeable Dialog Windows
 */

#include "stdafx.h"
#include "SizeableDlg.h"

CSizeableDialog::CSizeableDialog(UINT nIDTemplate, CWnd* pParentWnd) :
	CCustomizableDialog(nIDTemplate, pParentWnd),
	m_bRedrawAll(false)
{
	m_rect.top = 0;
	m_rect.left = 0;
	m_rect.right = 0;
	m_rect.bottom = 0;
}

CSizeableDialog::~CSizeableDialog()
{
}

BEGIN_MESSAGE_MAP(CSizeableDialog, CCustomizableDialog)
	ON_WM_SIZING()
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CSizeableDialog::ForceRedrawAll(bool bForce)
{
	m_bRedrawAll = bForce;
}

void CSizeableDialog::AddSizeableControl(CWnd* pWnd, size_t eProperties)
{
	m_mapSizingWnd.push_back(std::make_pair(pWnd, eProperties));
}

void CSizeableDialog::RemoveSizeableControl(CWnd*)
{
}

void CSizeableDialog::OnSizing(UINT nType, LPRECT lpRect)
{
	CCustomizableDialog::OnSizing(nType, lpRect);

	if ((lpRect->right - lpRect->left) < m_rect.right)
		lpRect->right = lpRect->left + m_rect.right;

	if ((lpRect->bottom - lpRect->top) < m_rect.bottom)
		lpRect->bottom = lpRect->top + m_rect.bottom;
}

void CSizeableDialog::OnSize(UINT nType, int cx, int cy)
{
	CCustomizableDialog::OnSize(nType, cx, cy);

	if (!m_rect.right && !m_rect.bottom)
	{
		RECT rect;
		GetWindowRect(&rect);

		m_rect.left = cx;
		m_rect.top = cy;
		m_rect.right = rect.right - rect.left;
		m_rect.bottom = rect.bottom - rect.top;

		return;
	}

	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
	{
		int offcx = cx - m_rect.left;
		int offcy = cy - m_rect.top;

		for (auto cit = m_mapSizingWnd.begin(); cit != m_mapSizingWnd.end(); ++cit)
		{
			CWnd* pWnd = cit->first;
			size_t eResizeProp = cit->second;

			if (pWnd->m_hWnd)
			{
				WINDOWPLACEMENT wp;
				pWnd->GetWindowPlacement(&wp);
				RECT rect =
				{
					wp.rcNormalPosition.left,
					wp.rcNormalPosition.top,
					wp.rcNormalPosition.right - wp.rcNormalPosition.left,
					wp.rcNormalPosition.bottom - wp.rcNormalPosition.top
				};
				UINT nFlag = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER;

				if (eResizeProp & E_RESIZE_PROP_LEFT)
				{
					rect.left += offcx;
					nFlag &= ~SWP_NOMOVE;
				}

				if (eResizeProp & E_RESIZE_PROP_TOP)
				{
					rect.top += offcy;
					nFlag &= ~SWP_NOMOVE;
				}

				if (eResizeProp & E_RESIZE_PROP_WIDTH)
				{
					rect.right += offcx;
					nFlag &= ~SWP_NOSIZE;
				}

				if (eResizeProp & E_RESIZE_PROP_HEIGHT)
				{
					rect.bottom += offcy;
					nFlag &= ~SWP_NOSIZE;
				}

				pWnd->SetWindowPos(nullptr, rect.left, rect.top, rect.right, rect.bottom, nFlag);

				if (eResizeProp & E_RESIZE_PROP_REDRAW && !m_bRedrawAll)
					pWnd->RedrawWindow();
			}
		}

		m_rect.left = cx;
		m_rect.top = cy;

		if (m_bRedrawAll)
			RedrawWindow();
	}
}
