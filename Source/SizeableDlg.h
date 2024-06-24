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

#if !defined(AFX_SIZEABLE_DLG_H_INCLUDED_)
#define AFX_SIZEABLE_DLG_H_INCLUDED_

#include <map>
#include "CustomizableDlg.h"

class CSizeableDialog : public CCustomizableDialog
{
public:
	typedef enum
	{
		E_RESIZE_PROP_TOP = 0x01,
		E_RESIZE_PROP_LEFT = 0x02,
		E_RESIZE_PROP_WIDTH = 0x04,
		E_RESIZE_PROP_HEIGHT = 0x08,
		E_RESIZE_PROP_REDRAW = 0x10
	} E_RESIZE_PROP;

	CSizeableDialog(UINT nIDTemplate, CWnd* pParentWnd = nullptr);
	virtual ~CSizeableDialog();

	void ForceRedrawAll(bool bForce = true);

	void AddSizeableControl(CWnd* pWnd, size_t eProperties);
	void RemoveSizeableControl(CWnd* pWnd);

private:
	bool m_bRedrawAll;
	std::list<std::pair<CWnd*, size_t>> m_mapSizingWnd;
	RECT m_rect;

protected:
	virtual afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual afx_msg void OnSizing(UINT nType, LPRECT lpRect);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SIZEABLE_DLG_H_INCLUDED_)
