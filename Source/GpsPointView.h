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

#if !defined(AFX_GPSPOINTVIEW_H_INCLUDED_)
#define AFX_GPSPOINTVIEW_H_INCLUDED_

#include "ITN Tools.h"
#include "Navigator.h"

class CGpsPointView : public CListCtrl
{
public:
	typedef enum
	{
		E_RVCOL_ADDRESS,
		E_RVCOL_SNIPPET,
		E_RVCOL_LATITUDE,
		E_RVCOL_LONGITUDE,
		E_RVCOL_ALTITUDE,
		E_RVCOL_NUMBER
	} E_RVCOL_TYPE;

	typedef struct _RVCOLUMN {
		UINT unText;
		int nFormat;
		int nWidth;
		E_RVCOL_TYPE eRvColType;
	} RVCOLUMN, * LPRVCOLUMN;

	// Implementation
private:
	enum
	{
		E_CURSOR_DEFAULT,
		E_CURSOR_DRAG,
		E_CURSOR_ERROR,
		E_CURSOR_NUMBER
	};

	bool m_bDraggable;
	HCURSOR m_hCursor[E_CURSOR_NUMBER];
	bool m_bDragging;
	int m_nRefWidth;
	int m_nAddressColWidth;

protected:
	CGpsPointArray* m_pcGpsPointArray;
	int m_nDragIndex;
	int m_nDropIndex;

private:
	void FillRow(int nIndex);
	void Move(int nSrcIndex, int nDestIndex);

protected:
	virtual void OnEndDrop(int nDragIndex, int nDropIndex);

	virtual void OnRefresh(int nFirstIndex, int nNumber = 1);
	virtual bool OnInsert(int nIndex);
	virtual void OnDelete(int nIndex);
	virtual void OnSelect(int nIndex);

public:
	CGpsPointView();
	virtual ~CGpsPointView();

	virtual void Init(CGpsPointArray& cGpsPointArray, LPRVCOLUMN lprvColumn, bool bDraggable = false, bool bAutoResize = false);
	virtual CGpsPointArray& GpsPointArray() { return *m_pcGpsPointArray; }

	virtual void Refresh();
	virtual void Clear();

	virtual void Up();
	virtual void Down();
	virtual void Add(const CGpsPoint& cGpsPoint);
	virtual void Insert(int nIndex, const CGpsPoint& cGpsPoint);
	virtual void Remove(int nIndex = -1);
	virtual void Invert();
	virtual void Modify();
	virtual void Deplace(int nIndex, const geo::CGeoLatLng& cgLatLng);
	virtual void Rename(int nIndex, const std::string& strName);
	virtual void SelectItem(int nIndex);

	void MouseMove(UINT nFlags, CPoint point);
	void LButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_GPSPOINTVIEW_H_INCLUDED_)
