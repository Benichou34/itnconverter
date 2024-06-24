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
#include "stdx/format.h"
#include "NavPointView.h"

CNavPointView::CNavPointView() : CGpsPointView()
{
	m_pNavigator = nullptr;
	m_nTabIndex = -1;
	m_eViewType = E_VIEW_TYPE_DEFAULT;
}

CNavPointView::~CNavPointView()
{
}

void CNavPointView::SetNavigator(CNavigator& cNavigator, int nTabIndex, E_VIEW_TYPE eViewType)
{
	m_pNavigator = &cNavigator;

	if (nTabIndex != -1)
	{
		m_nTabIndex = nTabIndex;
		m_eViewType = eViewType;

		m_pNavigator->NewPinList(*this, m_nTabIndex);
	}
}

CNavPointView::E_VIEW_TYPE CNavPointView::GetType(int nIndex)
{
	E_VIEW_TYPE eViewType = m_eViewType;

	if (eViewType == E_VIEW_TYPE_ROADBOOK && nIndex != -1)
	{
		if (nIndex == 0)
			eViewType = E_VIEW_TYPE_START;
		else if (static_cast<size_t>(nIndex) == m_pcGpsPointArray->upper_bound())
			eViewType = E_VIEW_TYPE_END;
		else
			eViewType = E_VIEW_TYPE_PAUSE;
	}

	return eViewType;
}

void CNavPointView::Refresh()
{
	if (m_pNavigator)
		m_pNavigator->JavaScript_ClearPushPins(m_nTabIndex);
	CGpsPointView::Refresh();
}

void CNavPointView::Clear()
{
	if (m_pNavigator)
		m_pNavigator->JavaScript_ClearPushPins(m_nTabIndex);
	CGpsPointView::Clear();
}

void CNavPointView::OnRefresh(int nFirstIndex, int nNumber)
{
	CGpsPointView::OnRefresh(nFirstIndex, nNumber);
	if (m_pNavigator && nNumber)
		m_pNavigator->JavaScript_RefreshPushPins(m_nTabIndex, nFirstIndex, nNumber);
}

bool CNavPointView::OnInsert(int nIndex)
{
	CGpsPointView::OnInsert(nIndex);
	if (m_pNavigator)
		m_pNavigator->JavaScript_InsertPushPin(m_nTabIndex, nIndex);

	return true;
}

void CNavPointView::OnDelete(int nIndex)
{
	if (m_pNavigator)
		m_pNavigator->JavaScript_DelPushPin(m_nTabIndex, nIndex);
	CGpsPointView::OnDelete(nIndex);
}

void CNavPointView::OnSelect(int nIndex)
{
	CGpsPointView::OnSelect(nIndex);
	if (nIndex != -1 && m_pNavigator)
		m_pNavigator->JavaScript_ViewPushPin(m_nTabIndex, nIndex);
}
