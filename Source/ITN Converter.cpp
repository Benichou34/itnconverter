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

#include "stdafx.h"
#include "ITN Converter.h"
#include "ITN ConverterDlg.h"
#include "ToolsLibrary/ToolsString.h"
#include "storage/Registry.h"

#ifdef LOG_TO_FILE
#include <iostream>
#include <Shlobj.h>
#elif defined(_DEBUG) || defined(FORCE_LOG)
#define AUTO_OVERLOAD_STDLOG
#include "stdx/logstream.h"
#else
#include <iostream>
#endif

#ifdef _USE_VLD
#include <vld.h>
#endif // _USE_VLD

 /////////////////////////////////////////////////////////////////////////////
 // CITNConverterApp

BEGIN_MESSAGE_MAP(CITNConverterApp, CWinApp)
	//{{AFX_MSG_MAP(CITNConverterApp)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CITNConverterApp construction

CITNConverterApp::CITNConverterApp()
{
#ifdef LOG_TO_FILE
	wchar_t desktopPath[MAX_PATH + 1];
	std::wstring logPath;

	if (SHGetSpecialFolderPathW(nullptr, desktopPath, CSIDL_DESKTOPDIRECTORY, FALSE) == TRUE)
	{
		logPath = desktopPath;
		logPath += L"\\";
	}

	logPath += L"itnconv.log";
	m_logFile.open(logPath, std::ios_base::app);
	if (m_logFile.is_open())
	{
		m_logFile.imbue(std::locale(m_wlogFile.getloc(), new std::codecvt_utf8<char>));
		std::clog.rdbuf(m_logFile.rdbuf());
	}

	m_wlogFile.open(logPath, std::ios_base::app);
	if (m_wlogFile.is_open())
	{
		m_wlogFile.imbue(std::locale(m_wlogFile.getloc(), new std::codecvt_utf8<wchar_t>));
		std::wclog.rdbuf(m_wlogFile.rdbuf());
	}
#endif

#if defined(_DEBUG) || defined(FORCE_LOG)
	wlog() << time(nullptr) << L": " SOFT_FULL_NAME << L" [" << GetCommandLineW() << L"]" << std::endl;
#endif
}

CITNConverterApp::~CITNConverterApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CITNConverterApp object

CITNConverterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CITNConverterApp initialization

CRegParam& CITNConverterApp::RegParam()
{
	return theApp.m_RegParam;
}

std::ostream& CITNConverterApp::log()
{
#ifdef LOG_TO_FILE
	return theApp.m_logFile;
#else
	return std::clog;
#endif
}

std::wostream& CITNConverterApp::wlog()
{
#ifdef LOG_TO_FILE
	return theApp.m_wlogFile;
#else
	return std::wclog;
#endif
}

void CITNConverterApp::GetCommandLineArgv(stdx::wstring_helper::vector& vecString, bool bKeepProgName)
{
	std::wstring strCommandLine(GetCommandLine());
	size_t ulPosBegin = 0;
	size_t ulPosEnd = 0;

	vecString.clear();
	stdx::wstring_helper::trimright(strCommandLine);

	while (strCommandLine.size())
	{
		stdx::wstring_helper::trimleft(strCommandLine);
		if (strCommandLine[0] == '"')
		{
			ulPosBegin = 1;
			ulPosEnd = strCommandLine.find('"', ulPosBegin);
		}
		else
		{
			ulPosBegin = 0;
			ulPosEnd = strCommandLine.find(' ', ulPosBegin);
		}

		if (ulPosEnd != std::wstring::npos)
		{
			vecString.push_back(strCommandLine.substr(ulPosBegin, ulPosEnd - ulPosBegin));
			strCommandLine.erase(0, ulPosEnd + 1);
		}
		else
		{
			vecString.push_back(strCommandLine.substr(ulPosBegin));
			strCommandLine.clear();
		}
	}

	if (!bKeepProgName && vecString.size())
		vecString.erase(vecString.begin());
}

BOOL CITNConverterApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	AfxEnableControlContainer();

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif // _MFC_VER < 0x0700

	// Set Google Api parameters
#ifdef GGM_API_KEY
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_GOOGLE_API, std::nothrow).setKey(GGM_API_KEY);
#endif
#ifdef IDS_GOOGLE_LANG
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_GOOGLE_API, std::nothrow).setLang(CToolsString::Load(IDS_GOOGLE_LANG));
#endif

	// Set Bing Api parameters
#ifdef MBM_API_KEY
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_BING_API, std::nothrow).setKey(MBM_API_KEY);
#endif
#ifdef IDS_BING_LANG
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_BING_API, std::nothrow).setLang(CToolsString::Load(IDS_BING_LANG));
#endif

	// Set ViaMichelin Api parameters
#ifdef VMM_API_KEY
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_VIAMICHELIN_API, std::nothrow).setKey(VMM_API_KEY);
#endif
#ifdef IDS_VIAMICHELIN_LANG
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_VIAMICHELIN_API, std::nothrow).setLang(CToolsString::Load(IDS_VIAMICHELIN_LANG));
#endif

#ifdef IDS_GOOGLE_LANG
	// Set OSRM Api parameters
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_OSRM_API, std::nothrow).setLang(CToolsString::Load(IDS_GOOGLE_LANG));
	//	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_OSR_API, std::nothrow).setLang(CToolsString::Load(IDS_GOOGLE_LANG));
#endif

	// Set GeoNames Api parameters
#ifdef GNS_API_KEY
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_GEONAMES_API, std::nothrow).setKey(GNS_API_KEY);
#endif
#ifdef IDS_GOOGLE_LANG
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_GEONAMES_API, std::nothrow).setLang(CToolsString::Load(IDS_GOOGLE_LANG));
#endif

	// Set Tomtom Api parameters
#ifdef TOMTOM_API_KEY
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_TOMTOM_API, std::nothrow).setKey(TOMTOM_API_KEY);
#endif
#ifdef TOMTOM_ROUTING_API_KEY
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_TOMTOM_ROUTING_API, std::nothrow).setKey(TOMTOM_ROUTING_API_KEY);
#endif
#ifdef TOMTOM_SEARCH_API_KEY
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_TOMTOM_SEARCH_API, std::nothrow).setKey(TOMTOM_SEARCH_API_KEY);
#endif

#ifdef IDS_GOOGLE_LANG
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_TOMTOM_API, std::nothrow).setLang(CToolsString::Load(IDS_GOOGLE_LANG));
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_TOMTOM_ROUTING_API, std::nothrow).setLang(CToolsString::Load(IDS_GOOGLE_LANG));
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_TOMTOM_SEARCH_API, std::nothrow).setLang(CToolsString::Load(IDS_GOOGLE_LANG));
#endif

	// Set Here Api parameters
	geo::CGeoProviderHereApi& hereApi = geo::CGeoProviders::instance().add<geo::CGeoProviderHereApi>(geo::E_GEO_PROVIDER_HERE_API);
#if defined(HERE_API_ID) && defined(HERE_API_KEY)
	hereApi.setId(HERE_API_ID);
	hereApi.setKey(HERE_API_KEY);
#endif
#ifdef IDS_BING_LANG
	hereApi.setLang(CToolsString::Load(IDS_BING_LANG));
#endif

	// Set GeoPortal Api parameters
#ifdef GEOPORTAL_API_KEY
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_GEOPORTAL_API, std::nothrow).setKey(GEOPORTAL_API_KEY);
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_GEOPORTAL_API, std::nothrow).setReferer(GEOPORTAL_API_REFERER);
#endif
#ifdef IDS_GOOGLE_LANG
	geo::CGeoProviders::instance().get(geo::E_GEO_PROVIDER_GEOPORTAL_API, std::nothrow).setLang(CToolsString::Load(IDS_GOOGLE_LANG));
#endif

	geo::CGeoProviders::instance().setDefaultProvider(geo::E_GEO_PROVIDER_BING_API);

	RegParam().Init(_T(REGISTRY_KEY));

	// Set appropriate language
	stdx::wstring_helper::vector vecArglist;
	CITNConverterApp::GetCommandLineArgv(vecArglist, true);

	if (!vecArglist.empty())
	{
		std::wstring appName = CWToolsString::FileName(vecArglist.front());
		vecArglist.erase(vecArglist.begin());

		// Check IE version
		const uint32_t minIEVersion = 10000;
		storage::Registry regCurrentUser;
		storage::SFolder ieEmulation = regCurrentUser(_T("Software\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION"));
		if (!const_cast<const storage::SFolder&>(ieEmulation)[appName].exist() || static_cast<uint32_t>(ieEmulation[appName]) < minIEVersion)
			ieEmulation[appName] = minIEVersion;
	}

	if (!vecArglist.empty() && vecArglist.front().find(_T("LANG_")) != std::wstring::npos)
		SetLanguage(vecArglist.front().c_str());
	else
		SetLanguage();

	CITNConverterDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
