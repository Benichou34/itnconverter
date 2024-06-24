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
 * Purpose : Dialog Windows for open or export csv files
 */

#if !defined(AFX_CSVDLG_H_INCLUDED_)
#define AFX_CSVDLG_H_INCLUDED_

 /////////////////////////////////////////////////////////////////////////////
 // CCsvDlg dialog

class CCsvDlg : public CDialog
{
public:
#pragma pack(push, 1) // packing is now 1
	typedef struct
	{
		int nTemplate;
		union
		{
			struct
			{
				int nColLatitude;
				int nColLongitude;
				int nColAltitude;
				int nColAddress;
				int nColSnippet;
			};
			int ntabCol[5];
		};
		bool bUseLocale;
		char szSDecimal[4];
		char szSList[4];
	} _CSV_CONFIG;
#pragma pack(pop) // packing is default value

private:
	_CSV_CONFIG& m_csvConfig;
	bool m_bRead;

	// Dialog Data
	//{{AFX_DATA(CCsvDlg)
	enum { IDD = IDD_DIALOG_CSV };
	CComboBox m_cbTemplate;
	CComboBox m_cbLatitude;
	CComboBox m_cbLongitude;
	CComboBox m_cbAltitude;
	CComboBox m_cbAddress;
	CComboBox m_cbSnippet;
	CButton m_btCheckLocale;
	CString m_csSDecimal;
	CString m_csSList;
	//}}AFX_DATA

	// Construction
public:
	CCsvDlg(_CSV_CONFIG& csvConfig, bool bRead, CWnd* pParent = nullptr);   // standard constructor

	// Implementation
protected:
	// Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboTemplate();
	afx_msg void OnClickCheckLocale();

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CSVDLG_H_INCLUDED_)
