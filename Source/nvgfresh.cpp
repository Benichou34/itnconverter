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
 * Purpose : Send coordinates to Navigon Fresh, if available
 */

#include "stdafx.h"
#include <fstream>
#include "nvgfresh.h"
#include "Storage/Registry.h"
#include "ITN Tools.h"
#include "GeoServices/GeoNamesCountryCode.h"
#include "GeoServices/GeoNamesCountryInfo.h"
#include "SAXParser/SAXWriter.h"

namespace
{
	void getLocation(std::wstring& strLocation, CSendToGps::E_TYPE eType)
	{
		std::wstring strBase;
		strLocation.clear();

		if (eType == CSendToGps::E_TYPE_ROUTE)
			strBase = _T("NAVIGON.freshroute");
		else
			strBase = _T("NAVIGON.freshcontact");

		try
		{
			const storage::Registry regStorage(HKEY_CLASSES_ROOT);
			strLocation = regStorage(strBase + _T("\\shell\\open\\command"))[_T("")];
		}
		catch (storage::Exception&)
		{
			try
			{
				const storage::Registry regStorage(HKEY_LOCAL_MACHINE);
				strLocation = regStorage(_T("SOFTWARE\\Classes\\") + strBase + _T("\\shell\\open\\command"))[_T("")];
			}
			catch (storage::Exception&)
			{
			}
		}
	}

	void sendTo(const std::wstring& strFileName, CSendToGps::E_TYPE eType)
	{
		std::wstring strLocation;
		getLocation(strLocation, eType);

		if (strLocation.size())
		{
			size_t pos = strLocation.find(_T("%1"));
			strLocation.replace(pos, 2, strFileName);

			STARTUPINFO startupInfo = { 0 };
			PROCESS_INFORMATION processInformation;

			CreateProcess(nullptr,
				const_cast<wchar_t*>(strLocation.c_str()),
				nullptr,
				nullptr,
				false,
				0,
				nullptr,
				nullptr,
				&startupInfo,
				&processInformation);
		}
	}
}

bool CNavigonFresh::IsAvailabled(CSendToGps::E_TYPE eType)
{
	std::wstring strLocation;
	getLocation(strLocation, eType);
	return !strLocation.empty();
}

bool CNavigonFresh::WriteRoute(const CGpsPointArray& cGpsPointArray, const std::wstring& strFileName)
{
	// Create and fill file
	std::ofstream ofsFile(strFileName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return false;

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();

		// Header
		SAXWriter::Tag tagRoute = xmlWriter.tag("Route");
		xmlWriter.tag("Name").content(cGpsPointArray.name());

		// Points
		for (CGpsPointArray::const_iterator it = cGpsPointArray.begin(); it != cGpsPointArray.end(); ++it)
		{
			const CGpsPoint& cGpsPoint = *it;

			SAXWriter::Tag tagPoint = xmlWriter.tag("Point");
			xmlWriter.tag("Name").content(cGpsPoint.name());
			xmlWriter.tag("x").content(stdx::string_helper::to_string(cGpsPoint.lng()));
			xmlWriter.tag("y").content(stdx::string_helper::to_string(cGpsPoint.lat()));
		}
	}

	return ofsFile.good();
}

void CNavigonFresh::SendTo(const CGpsPoint& cGpsPoint)
{
	TCHAR szTempPath[MAX_PATH + 1];
	GetTempPath(MAX_PATH, szTempPath);

	std::wstring strFileName(szTempPath);
	strFileName += _T("itnconv.freshcontact");

	// Create and fill file
	std::ofstream ofsFile(strFileName.c_str(), std::ios_base::binary);
	if (ofsFile)
	{
		std::wstring wstrLine;
		stdx::wstring_helper::from_utf8(stdx::format(SOFT_FULL_NAME "|%s|%s|-|-|-|-|%f|%f")(cGpsPoint.name())(GetCountry(cGpsPoint))(cGpsPoint.lng())(cGpsPoint.lat()).str(), wstrLine);
		ofsFile << static_cast<unsigned char>(0xFF) << static_cast<unsigned char>(0xFE);
		ofsFile.write(reinterpret_cast<const char*>(wstrLine.data()), wstrLine.size() * 2);
		ofsFile.close();

		sendTo(strFileName, CSendToGps::E_TYPE_POINT);
	}
}

void CNavigonFresh::SendTo(const CGpsPointArray& cGpsPointArray)
{
	TCHAR szTempPath[MAX_PATH + 1];
	GetTempPath(MAX_PATH, szTempPath);

	std::wstring strFileName(szTempPath);
	strFileName += _T("\\itnconv.freshroute");

	if (WriteRoute(cGpsPointArray, strFileName))
		sendTo(strFileName, CSendToGps::E_TYPE_ROUTE);
}

std::string CNavigonFresh::GetCountry(const CGpsPoint& cGpsPoint)
{
	std::string strCountry;
	CToolsString::Load(IDS_COUNTRY_ISOALPHA3, strCountry);

	// Recherche du pays
	geo::CGeoNamesCountryCode gnCountryCode(cGpsPoint);
	if (gnCountryCode.getStatus() == geo::E_GEO_OK)
	{
		geo::CGeoNamesCountryInfo gnCountryInfo(gnCountryCode.getCountryCode());
		if (gnCountryInfo.getStatus() == geo::E_GEO_OK)
			strCountry = gnCountryInfo.getIsoAlpha3();
	}

	return strCountry;
}
