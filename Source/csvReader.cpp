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
#include <fstream>
#include "ITN Tools.h"
#include "CsvDlg.h"
#include "stdx/string_helper.h"
#include "stdx/bom.h"

int ReadCSV(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool bCmdLine)
{
	CCsvDlg::_CSV_CONFIG csvConfig;
	CCsvDlg csvDlg(csvConfig, true);

	if (!bCmdLine && csvDlg.DoModal() != IDOK)
		return ERROR_CANCELLED;

	std::wifstream ifsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ifsFile)
		return S_FALSE;

	stdx::wbom::imbue(ifsFile, stdx::wbom::read(ifsFile));

	for (int i = 0; i < 5; i++)
		(csvConfig.ntabCol[i])--;

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	std::wstring separators(L"\"");
	int i = 0;
	while (csvConfig.szSList[i] && i < 4)
	{
		separators += csvConfig.szSList[i];
		++i;
	}

	std::wstring strLine;
	while (std::getline(ifsFile, strLine))
	{
		CGpsPoint cGpsPoint;

		std::vector<std::wstring> vecResult;
		size_t ulPos = 0;
		size_t ulPrevPos = 0;
		size_t ulCounter = 0;

		do
		{
			ulPos = strLine.find_first_of(separators, ulPos);
			if (ulPos != std::wstring::npos)
			{
				if (strLine[ulPos] == '"')
				{
					++ulCounter;
				}
				else if (ulCounter % 2 == 0)
				{
					size_t ulsize = ulPos - ulPrevPos;
					vecResult.push_back(strLine.substr(ulPrevPos, ulsize));

					ulPrevPos = ulPos + 1;
					ulCounter = 0;
				}

				++ulPos;
			}
			else
			{
				vecResult.push_back(strLine.substr(ulPrevPos));
				ulCounter = 0;
			}
		} while (ulPos != std::wstring::npos);

		// Latitude
		if (csvConfig.nColLatitude != -1 && vecResult.size() > static_cast<size_t>(csvConfig.nColLatitude))
		{
			stdx::wstring_helper::trimleft(vecResult[csvConfig.nColLatitude], L"\" ");
			stdx::wstring_helper::trimright(vecResult[csvConfig.nColLatitude], L"\" \r");

			stdx::wstring_helper::replace(vecResult[csvConfig.nColLatitude], stdx::wstring_helper::widen(csvConfig.szSDecimal), L".");
			if (stdx::wstring_helper::istype<double>(vecResult[csvConfig.nColLatitude]))
				cGpsPoint.lat(stdx::wstring_helper::string_to<double>(vecResult[csvConfig.nColLatitude]));
		}

		// Longitude
		if (csvConfig.nColLongitude != -1 && vecResult.size() > static_cast<size_t>(csvConfig.nColLongitude))
		{
			stdx::wstring_helper::trimleft(vecResult[csvConfig.nColLongitude], L"\" ");
			stdx::wstring_helper::trimright(vecResult[csvConfig.nColLongitude], L"\" \r");

			stdx::wstring_helper::replace(vecResult[csvConfig.nColLongitude], stdx::wstring_helper::widen(csvConfig.szSDecimal), L".");
			if (stdx::wstring_helper::istype<double>(vecResult[csvConfig.nColLongitude]))
				cGpsPoint.lng(stdx::wstring_helper::string_to<double>(vecResult[csvConfig.nColLongitude]));
		}

		// Altitude
		if (csvConfig.nColAltitude != -1 && vecResult.size() > static_cast<size_t>(csvConfig.nColAltitude))
		{
			stdx::wstring_helper::trimleft(vecResult[csvConfig.nColAltitude], L"\" ");
			stdx::wstring_helper::trimright(vecResult[csvConfig.nColAltitude], L"\" \r");

			stdx::wstring_helper::replace(vecResult[csvConfig.nColAltitude], stdx::wstring_helper::widen(csvConfig.szSDecimal), L".");
			if (stdx::wstring_helper::istype<double>(vecResult[csvConfig.nColAltitude]))
				cGpsPoint.alt(stdx::wstring_helper::string_to<double>(vecResult[csvConfig.nColAltitude]));
		}

		// Address
		if (csvConfig.nColAddress != -1 && vecResult.size() > static_cast<size_t>(csvConfig.nColAddress))
		{
			stdx::wstring_helper::trimleft(vecResult[csvConfig.nColAddress], L"\" ");
			stdx::wstring_helper::trimright(vecResult[csvConfig.nColAddress], L"\" \r");
			cGpsPoint.name(stdx::wstring_helper::to_utf8(vecResult[csvConfig.nColAddress]));
		}

		// Snippet
		if (csvConfig.nColSnippet != -1 && vecResult.size() > static_cast<size_t>(csvConfig.nColSnippet))
		{
			stdx::wstring_helper::trimleft(vecResult[csvConfig.nColSnippet], L"\" ");
			stdx::wstring_helper::trimright(vecResult[csvConfig.nColSnippet], L"\" \r");
			cGpsPoint.comment(stdx::wstring_helper::to_utf8(vecResult[csvConfig.nColSnippet]));
		}

		if (cGpsPoint)
			cGpsRoute.push_back(cGpsPoint);
	}

	return S_OK;
}
