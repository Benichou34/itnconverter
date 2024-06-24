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
 * Purpose : Common functions
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "GeoServices/GeoServices.h"
#include "ToolsLibrary/Internet.h"

CWinApiException::CWinApiException(HRESULT error) throw() :
	m_ErrorCode(error)
{
	CITNConverterApp::log() << clock() << ": CWinApiException(0x" << std::hex << m_ErrorCode << ")" << std::endl;
}

CWinApiException::CWinApiException(long error, const std::string& msg) throw() :
	m_ErrorCode(error),
	m_Message(msg)
{
	CITNConverterApp::log() << clock() << ": CWinApiException(0x" << std::hex << m_ErrorCode << ")[" << m_Message << "]" << std::endl;
}

std::wstring GetStringStatus(int nStatusCode)
{
	unsigned int uiRes = IDS_EMPTY;

	switch (nStatusCode)
	{
	case geo::E_GEO_UNKNOWN_ERROR:
		uiRes = IDS_GEO_SERVER_ERROR;;
		break;

	case geo::E_GEO_INVALID_REQUEST:
		uiRes = IDS_GEO_MISSING_ADDRESS;
		break;

	case geo::E_GEO_ZERO_RESULTS:
	case geo::E_GEO_NOT_FOUND:
		uiRes = IDS_GEO_UNKNOWN_ADDRESS;
		break;

	case geo::E_GEO_OVER_QUERY_LIMIT:
	case geo::E_GEO_REQUEST_DENIED:
		uiRes = IDS_UNAVAILABLE_ADDRESS;
		break;

	default:
		break;
	}

	return CWToolsString::Load(uiRes);
}

void ReplaceFileExt(LPCTSTR lpszPathName, LPCTSTR szFileExt, CString& szNewPathName)
{
	// Retrieve file extension
	szNewPathName = lpszPathName;
	szNewPathName = szNewPathName.Left(szNewPathName.ReverseFind('.') + 1);

	szNewPathName += szFileExt;
}

TCHAR* ReplaceFileExt(LPCTSTR lpszPathName, LPCTSTR szFileExt)
{
	CString szNewPathName;
	TCHAR* lpTab = nullptr;

	// Retrieve file extension
	ReplaceFileExt(lpszPathName, szFileExt, szNewPathName);

	DWORD dwLength = szNewPathName.GetLength();
	lpTab = (TCHAR*)malloc((dwLength + 1) * sizeof(TCHAR));
	if (lpTab)
		wcscpy(lpTab, szNewPathName);

	return lpTab;
}

geo::GeoVehicleType::type_t FillRouteOptionsWithSettings(std::unique_ptr<geo::CGeoRouteOptions>& cgOptions, bool bLinked)
{
	geo::CGeoDirections geoDirections(CITNConverterApp::RegParam().RouteProvider());
	geo::GeoVehicleType::type_t vehicleType = CITNConverterApp::RegParam().VehicleType();

	geo::GeoRouteTravelOptions travelOptions = geoDirections->getSupportedTravelOptions();
	if (travelOptions.count(vehicleType) == 0 && !travelOptions.empty())
		vehicleType = travelOptions.begin()->first;

	cgOptions = geo::CGeoRouteOptions::getFromVehicleType(vehicleType);

	cgOptions->setLinked(bLinked);
	cgOptions->setItineraryType(CITNConverterApp::RegParam().ItineraryType());

	if (cgOptions->typeIs(geo::GeoRouteOptionsType::Road))
	{
		geo::CGeoRoadRouteOptions& cgRoadOptions = static_cast<geo::CGeoRoadRouteOptions&>(*cgOptions);

		cgRoadOptions.setHighway(!CITNConverterApp::RegParam().AvoidHighway());
		cgRoadOptions.setTolls(!CITNConverterApp::RegParam().AvoidTolls());
		//		cgRoadOptions.setBoatFerry(!CITNConverterApp::RegParam().AvoidBoatFerry());
		//		cgRoadOptions.setRailFerry(!CITNConverterApp::RegParam().AvoidRailFerry());
		//		cgRoadOptions.setTunnel(!CITNConverterApp::RegParam().AvoidTunnel());
		//		cgRoadOptions.setDirtRoad(!CITNConverterApp::RegParam().AvoidDirtRoad());
	}

	if (cgOptions->typeIs(geo::GeoRouteOptionsType::Truck))
	{
		geo::CGeoTruckRouteOptions& cgTruckOptions = static_cast<geo::CGeoTruckRouteOptions&>(*cgOptions);

		//		cgTruckOptions.setTractor(CITNConverterApp::RegParam().TruckTractor());
		cgTruckOptions.setTrailersCount(CITNConverterApp::RegParam().TruckTrailersCount());
		//		cgTruckOptions.setAxleCount(CITNConverterApp::RegParam().TruckAxleCount());
		cgTruckOptions.setCategory(CITNConverterApp::RegParam().TruckCategory());
		cgTruckOptions.setLimitedWeight(CITNConverterApp::RegParam().TruckLimitedWeight());
		cgTruckOptions.setWeightPerAxle(CITNConverterApp::RegParam().TruckWeightAxle());
		cgTruckOptions.setHeight(CITNConverterApp::RegParam().TruckHeight());
		cgTruckOptions.setWidth(CITNConverterApp::RegParam().TruckWidth());
		cgTruckOptions.setLength(CITNConverterApp::RegParam().TruckLength());
	}

	if (cgOptions->typeIs(geo::GeoRouteOptionsType::Thrilling))
	{
		//		geo::CGeoThrillingRouteOptions& cgThrillingOptions = static_cast<geo::CGeoThrillingRouteOptions&>(*cgOptions);

		//		cgThrillingOptions.setAlreadyUsedRoads(CITNConverterApp::RegParam().AlreadyUsedRoads());
		//		cgThrillingOptions.setHilliness(CITNConverterApp::RegParam().ThrillingHilliness());
		//		cgThrillingOptions.setWindingness(CITNConverterApp::RegParam().ThrillingWindingness());
	}

	return vehicleType;
}

#define TMP_BUFFER_SIZE	10240 // 10Ko

template<class T>
void GetPrivateProfileStdStringT(const std::basic_string<T>& strAppName, const std::basic_string<T>& strKeyName, const std::basic_string<T>& strFileName, std::basic_string<T>& strValue, const std::basic_string<T>& strDefault)
{
}

void GetPrivateProfileStdString(const std::string& strAppName, const std::string& strKeyName, const std::string& strFileName, std::string& strValue, const std::string& strDefault)
{
	size_t ulSize = 0;
	std::vector<char> vecTmpBuffer(TMP_BUFFER_SIZE);

	do
	{
		if (ulSize == vecTmpBuffer.size() - 1)
			vecTmpBuffer.resize(vecTmpBuffer.size() * 2);

		ulSize = GetPrivateProfileStringA(strAppName.c_str(), strKeyName.c_str(), strDefault.empty() ? nullptr : strDefault.c_str(), vecTmpBuffer.data(), vecTmpBuffer.size(), strFileName.c_str());
	} while (ulSize == vecTmpBuffer.size() - 1);

	strValue.assign(vecTmpBuffer.data(), ulSize);
}

void GetPrivateProfileStdString(const std::wstring& strAppName, const std::wstring& strKeyName, const std::wstring& strFileName, std::wstring& strValue, const std::wstring& strDefault)
{
	size_t ulSize = 0;
	std::vector<wchar_t> vecTmpBuffer(TMP_BUFFER_SIZE);

	do
	{
		if (ulSize == vecTmpBuffer.size() - 1)
			vecTmpBuffer.resize(vecTmpBuffer.size() * 2);

		ulSize = GetPrivateProfileStringW(strAppName.c_str(), strKeyName.c_str(), strDefault.empty() ? nullptr : strDefault.c_str(), vecTmpBuffer.data(), vecTmpBuffer.size(), strFileName.c_str());
	} while (ulSize == vecTmpBuffer.size() - 1);

	strValue.assign(vecTmpBuffer.data(), ulSize);
}

void DisplayFileError(HRESULT hr)
{
	switch (hr)
	{
	case S_OK:
		break;

	case ERROR_BAD_FORMAT:
		AfxMessageBox(IDS_BADFORMAT, MB_OK | MB_ICONSTOP);
		break;

	case ERROR_SHARING_VIOLATION:
	case STG_E_SHAREVIOLATION:
		AfxMessageBox(IDS_SHAREVIOLATION, MB_OK | MB_ICONSTOP);
		break;

	case ERROR_CANCELLED:
		break;

	default:
		AfxMessageBox(IDS_ERROPEN, MB_OK | MB_ICONSTOP);
		break;
	}
}

HBITMAP LoadTransparentBitmap(UINT uid)
{
	return (HBITMAP)LoadImage(AfxGetResourceHandle(),
		MAKEINTRESOURCE(uid),
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADTRANSPARENT | LR_LOADMAP3DCOLORS);
}

// http://msdn2.microsoft.com/en-us/library/aa768365.aspx
int HTTPErrorBox(DWORD dwError)
{
	int nRet = IDOK; // IDCANCEL
	std::wstring strError;

	switch (dwError)
	{
	case S_OK: // No error
		break;

#if _MSC_VER > 1000
	case INET_E_INVALID_URL: // (0x800C0002L)
		strError = L"The URL could not be parsed.";
		break;

	case INET_E_NO_SESSION: // (0x800C0003L)
		strError = L"No Internet session was established.";
		break;

	case INET_E_CANNOT_CONNECT: // (0x800C0004L)
		strError = L"The attempt to connect to the Internet has failed.";
		break;

	case INET_E_RESOURCE_NOT_FOUND: // (0x800C0005L)
		strError = L"The server or proxy was not found.";
		break;

	case INET_E_OBJECT_NOT_FOUND: // (0x800C0006L)
		strError = L"The object was not found.";
		break;

	case INET_E_DATA_NOT_AVAILABLE: // (0x800C0007L)
		strError = L"An Internet connection was established, but the data cannot be retrieved.";
		break;

	case INET_E_DOWNLOAD_FAILURE: // (0x800C0008L)
		strError = L"The download has failed (the connection was interrupted).";
		break;

	case INET_E_AUTHENTICATION_REQUIRED: // (0x800C0009L)
		strError = L"Authentication is needed to access the object.";
		break;

	case INET_E_NO_VALID_MEDIA: // (0x800C000AL)
		strError = L"The object is not in one of the acceptable MIME types.";
		break;

	case INET_E_CONNECTION_TIMEOUT: // (0x800C000BL)
		strError = L"The Internet connection has timed out.";
		break;

	case INET_E_INVALID_REQUEST: // (0x800C000CL)
		strError = L"The request was invalid.";
		break;

	case INET_E_UNKNOWN_PROTOCOL: // (0x800C000DL)
		strError = L"The protocol is not known and no pluggable protocols have been entered that match.";
		break;

	case INET_E_SECURITY_PROBLEM: // (0x800C000EL)
		strError = L"A security problem was encountered.";
		break;

	case INET_E_CANNOT_LOAD_DATA: // (0x800C000FL)
		strError = L"The object could not be loaded.";
		break;

	case INET_E_CANNOT_INSTANTIATE_OBJECT: // (0x800C0010L)
		strError = L"CoCreateInstance failed.";
		break;

	case INET_E_USE_DEFAULT_PROTOCOLHANDLER: // (0x800C0011L)
		strError = L"Use the default protocol handler.";
		break;

	case INET_E_USE_DEFAULT_SETTING: // (0x800C0012L)
		strError = L"Use the default settings.";
		break;

	case INET_E_QUERYOPTION_UNKNOWN: // (0x800C0013L)
		strError = L"The requested option is unknown.";
		break;

	case INET_E_REDIRECTING: // (0x800C0014L)
		strError = L"The request is being redirected.";
		break;

	case INET_E_REDIRECT_TO_DIR: // (0x800C0015L)
		strError = L"The request is being redirected to a directory.";
		break;

	case INET_E_CANNOT_LOCK_REQUEST: // (0x800C0016L)
		strError = L"The requested resource could not be locked.";
		break;
#endif // _MSC_VER > 1000

	default:// Unknown error
		LPWSTR pBuffer = nullptr;
		if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			dwError,
			0,
			(LPWSTR)&pBuffer,
			0,
			nullptr))
		{
			strError = L"Internal error.";
		}
		else
		{
			strError = pBuffer;
		}
		LocalFree(pBuffer);
		break;
	}

	if (!strError.empty())
		nRet = AfxMessageBox(stdx::wformat(CWToolsString::Load(IDS_NETWORK_ERR))(dwError)(strError).str().c_str(), MB_RETRYCANCEL | MB_ICONERROR); //MB_ICONEXCLAMATION);

	return nRet;
}

std::wstring GetStringDistance(size_t ulDistance, bool bMiles)
{
	std::wstring strDistance;

	if (bMiles)
	{
		if (ulDistance < MI2MTR(1)) // 1 mile
			strDistance = stdx::wformat(L"%.0f%s")(MTR2YD(ulDistance))(L"yd");
		else
			strDistance = stdx::wformat(L"%.1f%s")(MTR2MI(ulDistance))(L"mi");
	}
	else
	{
		if (ulDistance < KM2MTR(1)) // 1 km
			strDistance = stdx::wformat(L"%d%s")(ulDistance)(L"m");
		else
			strDistance = stdx::wformat(L"%.1f%s")(MTR2KM(ulDistance))(L"km");
	}

	return strDistance;
}

std::wstring GetStringDuration(size_t ulDuration)
{
	return stdx::wformat(L"%02d:%02d")(ulDuration / 3600)((ulDuration % 3600) / 60);
}

void GetStringCoordinates(const geo::CGeoLatLng& gLatLng, std::wstring& strLat, std::wstring& strLng)
{
	geo::CGeoLatLngDegrees gLatLngDegrees(gLatLng);

	switch (CITNConverterApp::RegParam().Coordinates())
	{
	case CRegParam::E_COORDINATES_DD:
		strLat = stdx::wstring_helper::to_string(gLatLngDegrees.latLng().lat());
		strLng = stdx::wstring_helper::to_string(gLatLngDegrees.latLng().lng());
		break;

	case CRegParam::E_COORDINATES_DM:
	{
		const geo::CGeoLatLngDegrees::S_DM& dmLat = gLatLngDegrees.dmLat();
		strLat = stdx::wformat(L"%c%d°%.2f'")(dmLat.bDirection ? 'S' : 'N')(dmLat.lDegrees)(dmLat.dMinutes);

		const geo::CGeoLatLngDegrees::S_DM& dmLng = gLatLngDegrees.dmLng();
		strLng = stdx::wformat(L"%c%d°%.2f'")(dmLng.bDirection ? 'W' : 'E')(dmLng.lDegrees)(dmLng.dMinutes);
	}
	break;

	case CRegParam::E_COORDINATES_DMS:
	{
		const geo::CGeoLatLngDegrees::S_DMS& dmsLat = gLatLngDegrees.dmsLat();
		strLat = stdx::wformat(L"%c%d°%d'%.2f\"")(dmsLat.bDirection ? 'S' : 'N')(dmsLat.lDegrees)(dmsLat.lMinutes)(dmsLat.dSeconds);

		const geo::CGeoLatLngDegrees::S_DMS& dmsLng = gLatLngDegrees.dmsLng();
		strLng = stdx::wformat(L"%c%d°%d'%.2f\"")(dmsLng.bDirection ? 'W' : 'E')(dmsLng.lDegrees)(dmsLng.lMinutes)(dmsLng.dSeconds);
	}
	break;

	default:
		strLat.clear();
		strLng.clear();
		break;
	}
}

bool IsVistaOrSuperior()
{
#if _WIN32_WINNT > _WIN32_WINNT_NT4
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 6; // Vista = 6.0

	// Initialize the condition mask.
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);

	// Perform the test.
	return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR, dwlConditionMask) == TRUE;
#else
	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	// Fix for warnings when building with new SDK
	// warning C4996: 'GetVersionExW': was declared deprecated
	// Deprecated. Use VerifyVersionInfo* or IsWindows* macros from VersionHelpers.
#pragma warning( disable : 4996 )
	GetVersionEx(&osvi);
#pragma warning( default : 4996 )
	return (osvi.dwMajorVersion > 5); // Vista = 6.0
#endif
}

typedef struct _Language
{
	DWORD dwPrimaryLanguage;
	TCHAR* szPrimaryLanguage;
} sLanguage;

static sLanguage sTabLanguage[] =
{
	{ LANG_FRENCH , _T("LANG_FRENCH")  }, // French
	{ LANG_ENGLISH, _T("LANG_ENGLISH") }, // English
	{ LANG_DUTCH  , _T("LANG_DUTCH")   }, // Dutch
	{ LANG_GERMAN , _T("LANG_GERMAN")  }, // German
	{LANG_ENGLISH , nullptr               }  // Default
};

void SetLanguage(const std::wstring& strPrimaryLanguage)
{
	int i = 0;
	LCID lcid = GetThreadLocale(); // Read current system language

	if (strPrimaryLanguage.empty())
	{
		DWORD dwPrimaryLanguage = CITNConverterApp::RegParam().Language();
		if (!dwPrimaryLanguage)
			dwPrimaryLanguage = PRIMARYLANGID(LANGIDFROMLCID(lcid)); // SUBLANGID(LANGIDFROMLCID(lcid))

		while (sTabLanguage[i].szPrimaryLanguage && dwPrimaryLanguage != sTabLanguage[i].dwPrimaryLanguage)
			i++;
	}
	else
	{
		while (sTabLanguage[i].szPrimaryLanguage && strPrimaryLanguage.compare(sTabLanguage[i].szPrimaryLanguage) != 0)
			i++;
	}

	SetThreadLocale(MAKELCID(MAKELANGID(sTabLanguage[i].dwPrimaryLanguage, SUBLANG_NEUTRAL), SORTIDFROMLCID(lcid)));

#if defined(_DEBUG) || defined(FORCE_LOG)
	CITNConverterApp::wlog() << clock() << L": LCID=" << lcid << L" [" << sTabLanguage[i].szPrimaryLanguage << L"]" << std::endl;
#endif //_DEBUG
}
