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
 * Purpose : Common declarations header
 */

#if !defined(_ITNTOOLS_H_INCLUDED_)
#define _ITNTOOLS_H_INCLUDED_

#include <string>
#include <stdint.h>
#include <cmath>
#include "stdx/string_helper.h"
#include "stdx/format.h"
#include "ToolsLibrary/Internet.h"
#include "ToolsLibrary/ToolsString.h"
#include "GpsRoute.h"
#include "GpsTrack.h"
#include "GpsWaypointArray.h"
#include "ITN Converter.h"
#include "GpsPoiArray.h"
#include "resource.h"

#define SET_TYPE(x) _T(#x)

#define TYPE_AXE  SET_TYPE(axe)
#define TYPE_AXG  SET_TYPE(axg)
#define TYPE_PTM  SET_TYPE(ptm)
#define TYPE_EST  SET_TYPE(est)
#define TYPE_GDB  SET_TYPE(gdb)
#define TYPE_MPS  SET_TYPE(mps)
#define TYPE_ITN  SET_TYPE(itn)
#define TYPE_RTE  SET_TYPE(rte)
#define TYPE_WPT  SET_TYPE(wpt)
#define TYPE_DAT  SET_TYPE(dat)
#define TYPE_CSV  SET_TYPE(csv)
#define TYPE_GPX  SET_TYPE(gpx)
#define TYPE_KML  SET_TYPE(kml)
#define TYPE_RDN  SET_TYPE(rdn)
#define TYPE_BCR  SET_TYPE(bcr)
#define TYPE_RT2  SET_TYPE(rt2)
#define TYPE_PLT  SET_TYPE(plt)
#define TYPE_MAG  SET_TYPE(mag)
#define TYPE_UPT  SET_TYPE(upt)
#define TYPE_PTT  SET_TYPE(ptt)
#define TYPE_AXT  SET_TYPE(axt)
#define TYPE_STT  SET_TYPE(stt)
#define TYPE_LOC  SET_TYPE(loc)
#define TYPE_GGM  SET_TYPE(ggm)
#define TYPE_OV2  SET_TYPE(ov2)
#define TYPE_XVM  SET_TYPE(xvm)
#define TYPE_TRP  SET_TYPE(trp)
#define TYPE_XML  SET_TYPE(xml)
#define TYPE_URL  SET_TYPE(url)
#define TYPE_LMX  SET_TYPE(lmx)
#define TYPE_TTH  SET_TYPE(tth)
#define TYPE_ITF  SET_TYPE(itf)
#define TYPE_STG  SET_TYPE(stg)
#define TYPE_TRB  SET_TYPE(trb)
#define TYPE_KRT  SET_TYPE(krt)
#define TYPE_TRL  SET_TYPE(trl)
#define TYPE_TXT  SET_TYPE(txt)
#define TYPE_OSM  SET_TYPE(osm)
#define TYPE_NVG  SET_TYPE(freshroute)
#define TYPE_FLK  SET_TYPE(tour)
#define TYPE_TK   SET_TYPE(tk)
#define TYPE_RT   SET_TYPE(rt)
#define TYPE_ROUTE  SET_TYPE(route)
#define TYPE_STORE  SET_TYPE(store)
#define TYPE_UPOI  SET_TYPE(upoi)
#define TYPE_HERE  SET_TYPE(here)
#define TYPE_VMM  SET_TYPE(vmm)
#define TYPE_TTD  SET_TYPE(ttd)
#define TYPE_HTM  SET_TYPE(html)

int ReadITN(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadGPX(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadRTE(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadAXE(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadMPS(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadGDB(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadCSV(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadKML(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadMNX(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadDAT(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadRDN(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadBCR(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadOZI(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadRT2(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadPLT(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadMAG(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadOV2(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadXVM(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadTRP(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadURL(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadLMX(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadNVG(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadNVM(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadITF(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadGPL(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadFLK(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadKRT(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadTRL(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadGCL(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadWPT(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadOSM(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadTK(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadXML(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadROUTE(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadUPOI(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadMPFCTR(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadDaimlerGPX(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);
int ReadCP10(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);

bool isGoogleURL(const std::wstring& strUrl);
int ReadGoogleURL(const std::wstring& strUrl, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);

bool isHereURL(const std::wstring& strUrl);
int ReadHereURL(const std::wstring& strUrl, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);

bool isViaMichelinURL(const std::wstring& strUrl);
int ReadViaMichelinURL(const std::wstring& strUrl, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);

bool isTomtomURL(const std::wstring& strUrl);
int ReadTomtomURL(const std::wstring& strUrl, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine);

int WriteITN(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteGPX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine); // GPX format 1.0
int WriteGPG(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine); // GPX format 1.1 MapSource
int WriteGBC(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine); // GPX format 1.1 BaseCamp
int WriteRTE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteAXE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WritePTM(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteEST(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteMPS(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteGDB(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteCSV(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteKML(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteMN4(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteMN5(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteMN6(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteMN7(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteDAT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteRDN(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteBCR(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteOZI(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteRT2(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WritePLT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteMAG(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteLOC(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteGGM(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteXVM(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteTRP(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteLMX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteNVG(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteNVM(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteOldITF(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteITF11(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteITF13(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteGPL(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteFLK(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteKRT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteCP8(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteCP10(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteTRL(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteGCL(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteWPT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteOSM(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteOV2(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteRT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteTK(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteXML(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteROUTE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteUPOI80(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteUPOI83(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteNDRIVE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteMPFCTR(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteHERE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteViaMichelin(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteTomtomDrive(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteDaimler50GPX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteDaimler55GPX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteDaimlerUbxGPX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);
int WriteSygicUrl(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool bCmdLine);

template <typename T> void swap_endian(T& t) { std::reverse(reinterpret_cast<char*>(&t), reinterpret_cast<char*>(&t) + sizeof(T)); }

std::wstring GetStringStatus(int nStatusCode);

void ReplaceFileExt(LPCTSTR lpszPathName, LPCTSTR szFileExt, CString& szNewPathName);
TCHAR* ReplaceFileExt(LPCTSTR lpszPathName, LPCTSTR szFileExt);

void GetPrivateProfileStdString(const std::string& strAppName, const std::string& strKeyName, const std::string& strFileName, std::string& strValue, const std::string& strDefault = std::string());
void GetPrivateProfileStdString(const std::wstring& strAppName, const std::wstring& strKeyName, const std::wstring& strFileName, std::wstring& strValue, const std::wstring& strDefault = std::wstring());

void DisplayFileError(HRESULT hr);
int HTTPErrorBox(DWORD dwError);
HBITMAP LoadTransparentBitmap(UINT uid);
geo::GeoVehicleType::type_t FillRouteOptionsWithSettings(std::unique_ptr<geo::CGeoRouteOptions>& cgOptions, bool bLinked = false);

bool IsVistaOrSuperior();
void SetLanguage(const std::wstring& lpszPrimaryLanguage = std::wstring());

std::wstring GetStringDistance(size_t ulDistance, bool bMiles);
std::wstring GetStringDuration(size_t ulDuration);
void GetStringCoordinates(const geo::CGeoLatLng& gLatLng, std::wstring& strLat, std::wstring& strLng);

class ScopedWaitCursor
{
public:
	ScopedWaitCursor(CWnd& wnd) : m_wnd(wnd)
	{
		m_wnd.EnableWindow(FALSE);
		m_wnd.BeginWaitCursor();
	}

	~ScopedWaitCursor()
	{
		m_wnd.EndWaitCursor();
		m_wnd.EnableWindow(TRUE);
	}

private:
	CWnd& m_wnd;
};

class CWinApiException : public std::exception
{
public:
	CWinApiException(HRESULT error) throw();
	CWinApiException(long error, const std::string& msg) throw();
	virtual ~CWinApiException() throw() {}

	virtual const char* what() const throw() { return m_Message.c_str(); } // Get message associated to exception
	HRESULT code() const throw() { return m_ErrorCode; } // Returns the error_code object associated with the exception.

private:
	HRESULT m_ErrorCode;
	std::string m_Message;
};

/* End line definition */
#define CRLF            "\r\n"

// Convert feet to meters
// 1 feet = 0,3048 meters
#define FEET2MTR(feet) (static_cast<double>(feet) * 0.3048)
#define MTR2FEET(mtr)  (static_cast<double>(mtr) / 0.3048)

// 1 kilometer = 1000 meters
#define MTR2KM(mtr) (static_cast<double>(mtr) / 1000)
#define KM2MTR(km)  (static_cast<double>(km) * 1000)

// 1 kilometer = 0.62137 miles
// 1 mile = 1,609344 kilometers
#define KM2MI(km) (static_cast<double>(km) / 1.609344)
#define MI2KM(mi) (static_cast<double>(mi) * 1.609344)

#define MTR2MI(mtr) KM2MI(MTR2KM(mtr))
#define MI2MTR(mi)  KM2MTR(MI2KM(mi))

// 1 yard  =  0,9144 meters
#define MTR2YD(mtr) (static_cast<double>(mtr) / 0.9144)
#define YD2MTR(yd)  (static_cast<double>(yd) * 0.9144)

#define NO_THROW(x) do { try { x; } catch(...) {} } while(0)

#endif // !defined(_ITNTOOLS_H_INCLUDED_)
