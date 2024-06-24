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
 * Purpose : Dialog Windows for insert or modify waypoints
 */

#if !defined(AFX_INSERTMODIFY_H_INCLUDED_)
#define AFX_INSERTMODIFY_H_INCLUDED_

#include "SizeableDlg.h"

 /////////////////////////////////////////////////////////////////////////////
 // CInsertModify dialog

class CInsertModify : public CSizeableDialog
{
	// Construction
public:
	CInsertModify(CWnd* pParent = nullptr);   // standard constructor

	// Dialog Data
	//{{AFX_DATA(CInsertModify)
	enum { IDD = IDD_INSERT_MODIFY };
	CComboBox m_cbCoordFormat;
	double	m_dLatitude;
	double	m_dLongitude;
	CComboBox m_cbLatNS;
	long	m_lLatDegrees;
	double  m_dLatMinutes;
	long    m_lLatMinutes;
	double  m_dLatSeconds;
	CComboBox m_cbLngEW;
	long	m_lLngDegrees;
	double  m_dLngMinutes;
	long    m_lLngMinutes;
	double  m_dLngSeconds;
	double	m_dAltitude;
	CString	m_csAddress;
	CString	m_csSnippet;
	//}}AFX_DATA

	CGpsPoint* m_pcGpsPoint;
	geo::CGeoLatLngDegrees m_gLatLngDegrees;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertModify)
public:
	virtual int DoModal(CGpsPoint& cGpsPoint);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	bool UpdateAndCheckCoords(int nIndex);

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInsertModify)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboCoordinates();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTMODIFY_H_INCLUDED_)
