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

#if !defined(AFX_CHOOSEARRAY_DLG_H_INCLUDED_)
#define AFX_CHOOSEARRAY_DLG_H_INCLUDED_

#include "SizeableDlg.h"

class CChooseArrayDlg : public CSizeableDialog
{
public:
	CChooseArrayDlg(const std::wstring& strFileName, const std::vector<CGpsPointArray*>& vecGpsArray, CWnd* pParent = nullptr);
	virtual ~CChooseArrayDlg();

	virtual int DoModal();
	const std::vector<CGpsPointArray*>& GetSelectedArray() const { return m_vecSelectedArray; }

private:
	enum { IDD = IDD_DIALOG_CHOOSE_ARRAY };
	const std::vector<CGpsPointArray*>& m_vecGpsArray;
	const std::wstring m_strFileName;
	std::vector<CGpsPointArray*> m_vecSelectedArray;
	CTreeCtrl	m_TreeArray;
	CBitmap m_IconBitmap;
	CImageList m_ImageList;

protected:
	void AddItemToSelectedArray(HTREEITEM hItem);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};

#endif // !defined(AFX_CHOOSEARRAY_DLG_H_INCLUDED_)
