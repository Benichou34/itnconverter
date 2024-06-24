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

#if !defined(AFX_NAVPOINTVIEW_H_INCLUDED_)
#define AFX_NAVPOINTVIEW_H_INCLUDED_

#include "GpsPointView.h"
#include "Navigator.h"

class CNavPointView : public CGpsPointView
{
public:
	typedef enum
	{
		E_VIEW_TYPE_DEFAULT,
		E_VIEW_TYPE_START,
		E_VIEW_TYPE_PAUSE,
		E_VIEW_TYPE_END,
		E_VIEW_TYPE_MARKER,
		E_VIEW_TYPE_SEARCH,
		E_VIEW_TYPE_PROXIMITY,
		E_VIEW_TYPE_POI,
		E_VIEW_TYPE_FAVORITE,
		E_VIEW_TYPE_ROADBOOK
	} E_VIEW_TYPE;

	// Implementation
private:
	E_VIEW_TYPE m_eViewType;

protected:
	CNavigator* m_pNavigator;
	int m_nTabIndex;

protected:
	virtual void OnRefresh(int nFirstIndex, int nNumber = 1);
	virtual bool OnInsert(int nIndex);
	virtual void OnDelete(int nIndex);
	virtual void OnSelect(int nIndex);

public:
	CNavPointView();
	virtual ~CNavPointView();

	void SetNavigator(CNavigator& cNavigator, int nTabIndex, E_VIEW_TYPE eViewType = E_VIEW_TYPE_DEFAULT);
	E_VIEW_TYPE GetType(int nIndex);

	virtual void Refresh();
	virtual void Clear();
};

#endif // !defined(AFX_NAVPOINTVIEW_H_INCLUDED_)
