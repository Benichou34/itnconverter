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
 * Purpose : Main dialog Windows header
 */

#if !defined(AFX_ITNCONVERTERDLG_H_INCLUDED_)
#define AFX_ITNCONVERTERDLG_H_INCLUDED_

#include "ITN Converter.h"
#include "ITN Tools.h"
#include "GpsPointView.h"
#include "SizeableDlg.h"
#include "FileFormat.h"
#include "sendtogps.h"

 /////////////////////////////////////////////////////////////////////////////
 // CITNConverterDlg dialog

class CITNConverterDlg : public CSizeableDialog
{
	// Construction
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CITNConverterDlg(CWnd* pParent = nullptr);	// standard constructor

private:
	// Dialog Data
	//{{AFX_DATA(CITNConverterDlg)
	enum { IDD = IDD_ITNCONVERTER_DIALOG };
	CGpsPointView	m_ListPoint;
	CEdit	m_EditName;
	CEdit	m_EditCutItn;
	CStatic	m_LabelPointNumber;
	CStatic	m_LabelFileName;
	CComboBox	m_cbCutItn;
	CComboBox	m_cbExportExt;
	//}}AFX_DATA

	enum
	{
		BMP_MENU_MODIFY,
		BMP_MENU_REMOVE,
		BMP_MENU_TOMTOM_MAP_VIEW,
		BMP_MENU_GOOGLE_MAP_VIEW,
		BMP_MENU_VIRTUAL_EARTH_VIEW,
		BMP_MENU_HERE_MAP_VIEW,
		BMP_MENU_ADD_TO_TOMTOM,
		BMP_MENU_SEND_TO_NVGFRESH,
		BMP_MENU_SEND_TO_NAVGATE,
		BMP_MENU_SEND_TO_MIOMORE,
		BMP_MENU_SEND_TO_GARMIN,
		BMP_MENU_NUMBER
	};

	CGpsRoute m_cGpsRoute;
	std::wstring	m_strPathName;
	CMenu m_PopupMenu;
	CBitmap m_bmpMenuArray[BMP_MENU_NUMBER]; // bmp 16x16 pixels
	std::vector<FileFormatDesc> m_FileFormats;

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CITNConverterDlg)
protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	HICON m_hIcon;
	CToolTipCtrl m_tooltip;

	bool CheckCommandLine();

	bool CheckModified();
	void Clear();
	int OpenFile(const CString& sFileName, bool bAppend, bool bCmdLine);
	int ExportFile(const CString& sFileName, const FileFormatDesc& fileFormat, bool bCmdLine);
	int ReadFile(_ReadFile* pReadFile, const CString& sFileName, bool bAppend, bool bCmdLine);
	int CheckReadFile(const CString& sFileName, bool bAppend, HRESULT hr);

	void UpdateButtons(int nIndex = -1);
	void UpdateFileExportCombo();

	void OnButtonVirtualEarth();
	void OnButtonGoogleMaps();
	void OnButtonTomtomMaps();
	void OnButtonHereMaps();
	void OnButtonSendToGps(CSendToGps::E_GPS eGps);
	void OnButtonModify();

	// Generated message map functions
	//{{AFX_MSG(CITNConverterDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpen();
	afx_msg void OnExport();
	afx_msg void OnClose();
	afx_msg void OnChangeEditName();
	afx_msg void OnSelchangeComboFileexport();
	afx_msg void OnSelchangeComboCutItn();
	afx_msg void OnButtonUp();
	afx_msg void OnButtonDown();
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnButtonInvert();
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonEditor();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnDropFiles(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITNCONVERTERDLG_H_INCLUDED_)
