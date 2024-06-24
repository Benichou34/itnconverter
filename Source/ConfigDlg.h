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
 * Purpose : Config dialog header file
 */

#if !defined(AFX_CONFIGDLG_H_INCLUDED_)
#define AFX_CONFIGDLG_H_INCLUDED_

 /////////////////////////////////////////////////////////////////////////////
 // CConfigDlg dialog

class jsMapsInfos;
class jsProvidersInfos;

class CConfigDlg : public CDialog
{
	// Construction
public:
	CConfigDlg(jsMapsInfos& mapsInfos, const jsProvidersInfos& providersInfos, CWnd* pParent = nullptr);   // standard constructor

	// Dialog Data
		//{{AFX_DATA(CConfigDlg)
	enum { IDD = IDD_DIALOG_CONFIG };
	// NOTE: the ClassWizard will add data members here
#ifdef IDC_EDIT_USER_ID
	CString m_csUserId;
#endif
#ifdef IDC_TREE_MAPS
	CTreeCtrl m_TreeMaps;
#endif
#ifdef IDC_EDIT_TRUCK_HEIGHT
	double m_dTruckHeight;
#endif
#ifdef IDC_EDIT_TRUCK_WIDTH
	double m_dTruckWidth;
#endif
#ifdef IDC_EDIT_TRUCK_LENGTH
	double m_dTruckLength;
#endif
#ifdef IDC_EDIT_TRUCK_LIMITED_WEIGHT
	double m_dTruckLimitedWeight;
#endif
#ifdef IDC_EDIT_TRUCK_WEIGHT_AXLE
	double m_dTruckWeightAxle;
#endif

	CBitmap m_IconBitmap;
	CImageList m_ImageList;
	//}}AFX_DATA

private:
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CConfigDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	template <class T>
	void SetDlgItemWindowText(int nID, unsigned int uID)
	{
		T* pDlgItem = static_cast<T*>(GetDlgItem(nID));
		if (pDlgItem)
			pDlgItem->SetWindowText(CWToolsString::Load(uID).c_str());
	}

	template <class T>
	void SetDlgItemCheck(int nID, bool bState)
	{
		T* pDlgItem = static_cast<T*>(GetDlgItem(nID));
		if (pDlgItem)
			pDlgItem->SetCheck(bState);
	}

	template <class T>
	void GetDlgItemCheck(int nID, bool& bState)
	{
		const T* pDlgItem = static_cast<const T*>(GetDlgItem(nID));
		if (pDlgItem)
			bState = pDlgItem->GetCheck() == BST_CHECKED;
	}

	// Implementation
protected:
	CToolTipCtrl m_tooltip;

	// Generated message map functions
	//{{AFX_MSG(CConfigDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeComboRouteProvider();
	afx_msg void OnSelchangeComboVehicleType();
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelectAllMaps();
	afx_msg void OnUnselectAllMaps();
	afx_msg void OnClickTreeMaps(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnTreeMapsCheckStateChange(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	jsMapsInfos& m_MapsInfos;
	geo::GeoRouteTravelOptions m_CurrentTravelOptions;
	geo::CGeoAcceptedRouteOptions* m_pAcceptedRouteOptions;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGDLG_H_INCLUDED_)
