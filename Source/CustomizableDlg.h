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

#if !defined(AFX_CUSTOMIZABLE_DLG_H_INCLUDED_)
#define AFX_CUSTOMIZABLE_DLG_H_INCLUDED_

#include <memory>
#include <list>

class CCustomizableDialog : public CDialog
{
public:
	typedef enum
	{
		E_RESIZE_PROP_TOP = 0x01,
		E_RESIZE_PROP_LEFT = 0x02
	} E_RESIZE_PROP;

	CCustomizableDialog(UINT nIDTemplate, CWnd* pParentWnd = nullptr);
	virtual ~CCustomizableDialog();

	void SetBackgroundColor(const COLORREF& background);
	void AddBackgroundImage(UINT res, int top = 0, int left = 0, size_t eProperties = 0);
	const CSize& GetSize() const { return m_size; }

private:
	struct backBitmap
	{
		int top;
		int left;
		size_t eProperties;
		std::unique_ptr<CBitmap> pBitmap;
	};

	std::unique_ptr<CBrush> m_backgroundBrush;
	std::list<backBitmap> m_lstBitmapBackground;
	CSize m_size;

protected:
	virtual BOOL OnInitDialog();
	virtual afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual afx_msg void OnSize(UINT nType, int cx, int cy);
	//	virtual afx_msg void OnSizing(UINT nType, LPRECT lpRect);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_CUSTOMIZABLE_DLG_H_INCLUDED_)
