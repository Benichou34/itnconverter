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

#if !defined(AFX_NAVROUTEVIEW_H_INCLUDED_)
#define AFX_NAVROUTEVIEW_H_INCLUDED_

#include "NavPointView.h"
#include "Navigator.h"
#include "ProgressDlg.h"

class CNavRouteView : public CNavPointView
{
public:
	CNavRouteView();
	virtual ~CNavRouteView();

	void SetControls(CStatic* pDistanceLabel, CStatic* pInfoLabel);

	virtual void Refresh();
	virtual void Clear();

	virtual void Up();
	virtual void Down();
	virtual void Add(const CGpsPoint& cGpsPoint);
	virtual void Insert(int nIndex, const CGpsPoint& cGpsPoint);
	virtual void Remove(int nIndex = -1);
	virtual void Invert();
	virtual void Deplace(int nIndex, const geo::CGeoLatLng& cgLatLng);

	geo::CGeoLatLngs GetRoutePreview(int nIndex, const geo::CGeoLatLng& gLatLng, bool bStep);
	void AddIntermediateAll(bool bUseProgressDlg = true);
	void CalculateAll(bool bUseProgressDlg = true);
	void DisableAutoCalc();
	void ClearDriving();
	void RefreshDriving();
	void CalculateSegment(int nIndex);
	void Optimize();

protected:
	virtual void OnEndDrop(int nDragIndex, int nDropIndex);
	virtual bool OnInsert(int nIndex);

private:
	int AddIntermediate(int nSrcIndex, int nDstIndex);
	int DrivingInstruction(int nSrcIndex, int nDstIndex);
	int UpdateSummary();

private:
	CProgressDlg m_cProgressDlg;
	CStatic* m_pInfoLabel;
	CStatic* m_pDistanceLabel;
	bool m_bTempAutoCalc;
	bool m_bRefreshAll;
};

#endif // !defined(AFX_NAVROUTEVIEW_H_INCLUDED_)
