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
#include <algorithm>
#include "stdx/format.h"
#include "stdx/string_helper.h"
#include "GeoServices/GeoServices.h"
#include "GpsPointView.h"

BEGIN_MESSAGE_MAP(CGpsPointView, CListCtrl)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT_EX(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_SIZE()
END_MESSAGE_MAP()

CGpsPointView::CGpsPointView() : CListCtrl()
{
	m_pcGpsPointArray = nullptr;

	m_hCursor[E_CURSOR_DEFAULT] = LoadCursor(nullptr, IDC_ARROW);
	m_hCursor[E_CURSOR_DRAG] = LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_MOVE));
	m_hCursor[E_CURSOR_ERROR] = LoadCursor(nullptr, IDC_NO);

	m_bDragging = false;
	m_bDraggable = false;
	m_nDragIndex = -1;
	m_nDropIndex = -1;

	m_nRefWidth = 0;
	m_nAddressColWidth = 0;
}

CGpsPointView::~CGpsPointView()
{
}

BOOL CGpsPointView::OnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE lpNMItem = (LPNMITEMACTIVATE)pNMHDR;

	if (lpNMItem->uKeyFlags == 0)
		SelectItem(lpNMItem->iItem);

	*pResult = 0;
	return TRUE;
}

void CGpsPointView::Init(CGpsPointArray& cGpsPointArray, LPRVCOLUMN lprvColumn, bool bDraggable, bool bAutoResize)
{
	int i = 0;

	m_bDraggable = bDraggable;
	m_pcGpsPointArray = &cGpsPointArray;

	// Init columns
	while (lprvColumn[i].unText)
	{
		if (lprvColumn[i].nWidth == -1)
		{
			RECT rect;
			CListCtrl::GetClientRect(&rect);

			lprvColumn[i].nWidth = rect.right;
		}

		CListCtrl::InsertColumn(
			i,
			CWToolsString::Load(lprvColumn[i].unText).c_str(),
			lprvColumn[i].nFormat,
			lprvColumn[i].nWidth,
			lprvColumn[i].eRvColType);

		i++;
	}

	CListCtrl::SetExtendedStyle(CListCtrl::GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	if (bAutoResize)
	{
		RECT rect;
		GetWindowRect(&rect);
		m_nRefWidth = rect.right - rect.left;
	}
}

void CGpsPointView::FillRow(int nIndex)
{
	LVCOLUMN lvColumn;
	int i = 0;
	const CGpsPoint& cGpsPoint = m_pcGpsPointArray->at(nIndex);

	std::wstring strLat, strLng;
	GetStringCoordinates(cGpsPoint, strLat, strLng);

	lvColumn.mask = LVCF_SUBITEM;
	while (CListCtrl::GetColumn(i, &lvColumn))
	{
		switch (lvColumn.iSubItem)
		{
		case E_RVCOL_ADDRESS:
			CListCtrl::SetItemText(nIndex, i, stdx::wstring_helper::from_utf8(cGpsPoint.name()).c_str());
			break;

		case E_RVCOL_SNIPPET:
			CListCtrl::SetItemText(nIndex, i, stdx::wstring_helper::from_utf8(cGpsPoint.comment()).c_str());
			break;

		case E_RVCOL_LATITUDE:
			CListCtrl::SetItemText(nIndex, i, strLat.c_str());
			break;

		case E_RVCOL_LONGITUDE:
			CListCtrl::SetItemText(nIndex, i, strLng.c_str());
			break;

		case E_RVCOL_ALTITUDE:
			CListCtrl::SetItemText(nIndex, i, stdx::wstring_helper::to_string(cGpsPoint.alt()).c_str());
			break;

		case E_RVCOL_NUMBER:
			CListCtrl::SetItemText(nIndex, i, stdx::wformat(_T("%02d"))(nIndex + 1).str().c_str());
			break;

		default:
			break;
		}

		i++;
	}
}

void CGpsPointView::Move(int nSrcIndex, int nDestIndex)
{
	if (nSrcIndex != nDestIndex)
	{
		m_pcGpsPointArray->move(nSrcIndex, nDestIndex);

		int nBegin = nSrcIndex;
		int nEnd = nDestIndex;

		if (nSrcIndex > nDestIndex)
		{
			nBegin = nDestIndex;
			nEnd = nSrcIndex + 1;
		}

		if (nEnd < GetItemCount())
			nEnd++;

		OnRefresh(nBegin, nEnd - nBegin);
	}
}

void CGpsPointView::OnRefresh(int nFirstIndex, int nNumber)
{
	for (int i = nFirstIndex; i < nFirstIndex + nNumber; i++)
		FillRow(i);
}

bool CGpsPointView::OnInsert(int nIndex)
{
	CListCtrl::InsertItem(nIndex, _T(""));
	FillRow(nIndex);

	return true;
}

void CGpsPointView::OnDelete(int nIndex)
{
	m_pcGpsPointArray->erase(nIndex);
	CListCtrl::DeleteItem(nIndex);
}

void CGpsPointView::OnSelect(int nIndex)
{
	for (int i = 0; i < CListCtrl::GetItemCount(); i++)
	{
		CListCtrl::SetItemState(i, 0, LVIS_SELECTED);
		CListCtrl::SetItemState(i, 0, LVIS_DROPHILITED);
	}

	if (nIndex != -1)
		CListCtrl::SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
}

void CGpsPointView::Refresh()
{
	CListCtrl::DeleteAllItems();
	for (size_t i = 0; i < m_pcGpsPointArray->size(); i++)
	{
		if (!OnInsert(i))
			break;
	}
}

void CGpsPointView::Clear()
{
	CListCtrl::DeleteAllItems();
	if (m_pcGpsPointArray)
		m_pcGpsPointArray->clear();
}

void CGpsPointView::Up()
{
	if (!m_pcGpsPointArray)
		return;

	POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = CListCtrl::GetNextSelectedItem(pos);
		if (nIndex > 0)
		{
			Move(nIndex, nIndex - 1);
			SelectItem(nIndex - 1); // Update list
		}
	}
}

void CGpsPointView::Down()
{
	if (!m_pcGpsPointArray)
		return;

	POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = CListCtrl::GetNextSelectedItem(pos);
		if (static_cast<size_t>(nIndex) < m_pcGpsPointArray->upper_bound())
		{
			Move(nIndex, nIndex + 2);
			SelectItem(nIndex + 1); // Update list
		}
	}
}

void CGpsPointView::Add(const CGpsPoint& cGpsPoint)
{
	int nIndex = -1;

	if (!m_pcGpsPointArray)
		return;

	POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
	if (pos)
	{
		nIndex = CListCtrl::GetNextSelectedItem(pos);
		if (static_cast<size_t>(nIndex) < m_pcGpsPointArray->upper_bound())
		{
			nIndex++;
			m_pcGpsPointArray->insert(nIndex, cGpsPoint);
		}
		else
		{
			m_pcGpsPointArray->push_back(cGpsPoint);
			nIndex = m_pcGpsPointArray->upper_bound();
		}
	}
	else
	{
		m_pcGpsPointArray->push_back(cGpsPoint);
		nIndex = m_pcGpsPointArray->upper_bound();
	}

	OnInsert(nIndex);

	if (nIndex > 0)
		OnRefresh(nIndex - 1);

	OnRefresh(nIndex + 1, GetItemCount() - (nIndex + 1));
	SelectItem(nIndex);
}

void CGpsPointView::Insert(int nIndex, const CGpsPoint& cGpsPoint)
{
	if (!m_pcGpsPointArray)
		return;

	m_pcGpsPointArray->insert(nIndex, cGpsPoint);
	OnInsert(nIndex);

	if (nIndex > 0)
		OnRefresh(nIndex - 1);

	OnRefresh(nIndex + 1, GetItemCount() - (nIndex + 1));
	SelectItem(nIndex);
}

void CGpsPointView::Remove(int nIndex)
{
	if (!m_pcGpsPointArray)
		return;

	if (nIndex == -1)
	{
		std::vector<int> vecArray;
		std::vector<int>::reverse_iterator it;

		POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
		while (pos)
			vecArray.push_back(CListCtrl::GetNextSelectedItem(pos));

		std::sort(vecArray.begin(), vecArray.end());
		for (it = vecArray.rbegin(); it != vecArray.rend(); ++it)
			OnDelete(*it);

		if (!vecArray.empty())
			nIndex = vecArray.front();
	}
	else
	{
		OnDelete(nIndex);
	}

	if (static_cast<size_t>(nIndex) >= m_pcGpsPointArray->size())
		nIndex--;

	int nBeginRefresh = nIndex;
	if (nBeginRefresh < 0)
	{
		nBeginRefresh = 0;
		nIndex = -1;
	}

	OnRefresh(nBeginRefresh, GetItemCount() - nBeginRefresh);
	SelectItem(nIndex);
}

void CGpsPointView::Invert()
{
	if (!m_pcGpsPointArray)
		return;

	/* Invert Roadbook */
	m_pcGpsPointArray->reverse();
	OnRefresh(0, m_pcGpsPointArray->size());

	SelectItem(-1);
}

void CGpsPointView::Modify()
{
	if (!m_pcGpsPointArray)
		return;

	POSITION pos = CListCtrl::GetFirstSelectedItemPosition();
	while (pos)
	{
		int nIndex = CListCtrl::GetNextSelectedItem(pos);
		OnRefresh(nIndex);
		SelectItem(nIndex);
	}
}

void CGpsPointView::Rename(int nIndex, const std::string& strName)
{
	if (!m_pcGpsPointArray)
		return;

	m_pcGpsPointArray->at(nIndex).name(strName);
	OnRefresh(nIndex);
	SelectItem(nIndex);
}

void CGpsPointView::Deplace(int nIndex, const geo::CGeoLatLng& cgLatLng)
{
	if (!m_pcGpsPointArray)
		return;

	CGpsPoint& cGpsPoint = m_pcGpsPointArray->at(nIndex);
	cGpsPoint = cgLatLng;
	cGpsPoint.clearRouteInfo();

	if (nIndex + 1 < GetItemCount())
	{
		m_pcGpsPointArray->at(nIndex + 1).clearRouteInfo();
		OnRefresh(nIndex, 2);
	}
	else
	{
		OnRefresh(nIndex);
	}

	SelectItem(nIndex);
}

void CGpsPointView::SelectItem(int nIndex)
{
	OnSelect(nIndex);
}

BOOL CGpsPointView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (m_bDraggable)
	{
		// save the index of the item being dragged in m_nDragIndex
		m_nDragIndex = ((NM_LISTVIEW*)pNMHDR)->iItem;
		// set dragging flag
		m_bDragging = TRUE;
		m_nDropIndex = -1;

		m_hCursor[E_CURSOR_DEFAULT] = SetCursor(m_hCursor[E_CURSOR_DRAG]);
		if (m_hCursor[E_CURSOR_DEFAULT])
			m_hCursor[E_CURSOR_DEFAULT] = LoadCursor(nullptr, IDC_ARROW);

		// capture all mouse messages
		GetParent()->SetCapture();
	}

	*pResult = 0;
	return TRUE;
}

void CGpsPointView::MouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		CPoint pt(point);
		GetParent()->ClientToScreen(&pt);

		if (m_nDropIndex != -1)
		{
			// turn off hilight for previous drop target
			CListCtrl::SetItemState(m_nDropIndex, 0, LVIS_DROPHILITED);
			// redraw item
			CListCtrl::RedrawItems(m_nDropIndex, m_nDropIndex);
			m_nDropIndex = -1;
		}

		// if we drag outside current window
		if (WindowFromPoint(pt) != this)
		{
			SetCursor(m_hCursor[E_CURSOR_ERROR]);
		}
		else
		{
			SetCursor(m_hCursor[E_CURSOR_DRAG]);
			ScreenToClient(&pt);

			// get the item that is below cursor
			m_nDropIndex = HitTest(pt, &nFlags);

			if (m_nDropIndex != -1)
			{
				// highlight it
				CListCtrl::SetItemState(m_nDropIndex, LVIS_DROPHILITED, LVIS_DROPHILITED);
				// redraw item
				CListCtrl::RedrawItems(m_nDropIndex, m_nDropIndex);
			}
		}

		CListCtrl::UpdateWindow();
	}
}

void CGpsPointView::OnEndDrop(int nDragIndex, int nDropIndex)
{
	Move(nDragIndex, nDropIndex + ((nDropIndex > nDragIndex) ? 1 : 0));
}

void CGpsPointView::LButtonUp(UINT /*nFlags*/, CPoint /*point*/)
{
	if (m_bDragging)
	{
		if (m_nDropIndex != -1 && m_nDropIndex != m_nDragIndex)
		{
			OnEndDrop(m_nDragIndex, m_nDropIndex);
			SelectItem(m_nDropIndex);
		}

		ReleaseCapture();
		SetCursor(m_hCursor[E_CURSOR_DEFAULT]);
		m_bDragging = FALSE;
	}
}

void CGpsPointView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	if (!m_nRefWidth)
		return;

	LVCOLUMN lvColumn;
	int i = 0;

	lvColumn.mask = LVCF_SUBITEM;
	while (CListCtrl::GetColumn(i, &lvColumn))
	{
		if (lvColumn.iSubItem == E_RVCOL_ADDRESS)
		{
			if (!m_nAddressColWidth)
				m_nAddressColWidth = GetColumnWidth(i);

			SetColumnWidth(i, m_nAddressColWidth + (cx - m_nRefWidth));
			break;
		}

		i++;
	}
}
