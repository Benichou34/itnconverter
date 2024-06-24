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
 * Purpose : Comma Separated Values (.csv)
 *           tools.ietf.org/html/rfc4180
 */

#include "stdafx.h"
#include <algorithm>
#include "ITN Tools.h"
#include "CsvDlg.h"
#include "stdx/string_helper.h"

int WriteCSV(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool bCmdLine)
{
	HANDLE pFile = nullptr;
	DWORD dwNumberOfBytesWritten = 0;
	CGpsRoute::const_iterator it;
	std::vector<std::string>::iterator itLine;

	CCsvDlg::_CSV_CONFIG csvConfig;
	CCsvDlg csvDlg(csvConfig, false);

	if (!bCmdLine && csvDlg.DoModal() != IDOK)
		return ERROR_CANCELLED;

	int nFieldCount = *std::max_element(csvConfig.ntabCol, csvConfig.ntabCol + 5);
	for (int i = 0; i < 5; i++)
		(csvConfig.ntabCol[i])--;

	pFile = CreateFile(strPathName.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (!pFile)
		return GetLastError();

	// Header
	{
		std::string strLine;
		std::vector<std::string> vecLine(nFieldCount);

		if (csvConfig.nColLatitude != -1)
			CToolsString::Load(IDS_LATITUDE, vecLine[csvConfig.nColLatitude]);

		if (csvConfig.nColLongitude != -1)
			CToolsString::Load(IDS_LONGITUDE, vecLine[csvConfig.nColLongitude]);

		if (csvConfig.nColAltitude != -1)
			CToolsString::Load(IDS_ALTITUDE, vecLine[csvConfig.nColAltitude]);

		if (csvConfig.nColAddress != -1)
			CToolsString::Load(IDS_ADDRESS, vecLine[csvConfig.nColAddress]);

		if (csvConfig.nColSnippet != -1)
			CToolsString::Load(IDS_SNIPPET, vecLine[csvConfig.nColSnippet]);

		for (itLine = vecLine.begin(); itLine != vecLine.end(); ++itLine)
		{
			if (itLine != vecLine.begin())
				strLine += csvConfig.szSList;
			strLine += *itLine;
		}
		strLine += CRLF;

		WriteFile(pFile, strLine.c_str(), strLine.size(), &dwNumberOfBytesWritten, nullptr);
	}

	// Route
	for (it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
	{
		std::string strLine;
		std::vector<std::string> vecLine(nFieldCount);
		const CGpsPoint& cGpsPoint = *it;

		if (csvConfig.nColLatitude != -1)
		{
			vecLine[csvConfig.nColLatitude] = stdx::string_helper::to_string(cGpsPoint.lat());
			stdx::string_helper::replace(vecLine[csvConfig.nColLatitude], ".", csvConfig.szSDecimal);
		}

		if (csvConfig.nColLongitude != -1)
		{
			vecLine[csvConfig.nColLongitude] = stdx::string_helper::to_string(cGpsPoint.lng());
			stdx::string_helper::replace(vecLine[csvConfig.nColLongitude], ".", csvConfig.szSDecimal);
		}

		if (csvConfig.nColAltitude != -1)
		{
			vecLine[csvConfig.nColAltitude] = stdx::string_helper::to_string(cGpsPoint.alt());
			stdx::string_helper::replace(vecLine[csvConfig.nColAltitude], ".", csvConfig.szSDecimal);
		}

		if (csvConfig.nColAddress != -1)
			vecLine[csvConfig.nColAddress] = '"' + stdx::string_helper::from_utf8(cGpsPoint.name()) + '"';

		if (csvConfig.nColSnippet != -1)
			vecLine[csvConfig.nColSnippet] = '"' + stdx::string_helper::from_utf8(cGpsPoint.comment()) + '"';

		for (itLine = vecLine.begin(); itLine != vecLine.end(); ++itLine)
		{
			if (itLine != vecLine.begin())
				strLine += csvConfig.szSList;
			strLine += *itLine;
		}
		strLine += CRLF;

		WriteFile(pFile, strLine.c_str(), strLine.size(), &dwNumberOfBytesWritten, nullptr);
	}

	CloseHandle(pFile);

	return S_OK;
}
