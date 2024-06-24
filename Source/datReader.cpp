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
 * Purpose : Reader for Destinator PN file (.dat)
 *           www.destinatortechnologies.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "stdx/string_helper.h"

int ReadDAT(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	HRESULT hr = S_OK;
	DWORD dwFileSize = 0;
	HANDLE pFile = nullptr;
	HANDLE pFileMapping = nullptr;
	LPBYTE pMapFile = nullptr;

	double fCoords;
	LPBYTE pBuffer = nullptr;
	LPBYTE pEndFile = nullptr;

	pFile = CreateFile(strPathName.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (pFile == INVALID_HANDLE_VALUE)
	{
		// Error must return
		hr = GetLastError();
		goto end_function;
	}

	pFileMapping = CreateFileMapping(pFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (!pFileMapping)
	{
		// Error must return
		hr = S_FALSE;
		goto end_function;
	}

	pMapFile = (LPBYTE)MapViewOfFile(pFileMapping, FILE_MAP_READ, 0, 0, 0);
	if (!pMapFile)
	{
		// Error must return
		hr = S_FALSE;
		goto end_function;
	}

	dwFileSize = GetFileSize(pFile, nullptr);
	if (!dwFileSize)
		goto end_function;

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	pEndFile = pMapFile + dwFileSize;
	pBuffer = pMapFile;
	do
	{
		// Read Header
		uint16_t* pwText = (uint16_t*)pBuffer;
		std::wstring strTmp(pwText);

		if (!strTmp.compare(L"City->Street")
			|| !strTmp.compare(L"Street->City")
			|| !strTmp.compare(L"Zip->Street")
			|| !strTmp.compare(L"Intersection")
			|| !strTmp.compare(L"View_Link")
			|| !strTmp.compare(L"Poi...")
			|| !strTmp.compare(L"Dynamic POI")
			|| !strTmp.compare(L""))
		{
			CGpsPoint cGpsPoint;

			// Read Name
			pwText += strTmp.size() + 1;
			cGpsPoint.name(stdx::wstring_helper::to_utf8(pwText));

			// Read Comments
			pwText += std::wstring(pwText).size() + 1;

			if (cGpsPoint.name().empty())
				cGpsPoint.name(stdx::wstring_helper::to_utf8(pwText));
			else
				cGpsPoint.comment(stdx::wstring_helper::to_utf8(pwText));

			// Read street number
			pwText += std::wstring(pwText).size() + 1;

			// Read street name
			pwText += std::wstring(pwText).size() + 1;

			// Read city name
			pwText += std::wstring(pwText).size() + 1;

			// Read unknown field
			pwText += std::wstring(pwText).size() + 1;

			// Read zip code
			pwText += std::wstring(pwText).size() + 1;

			// Read unknown field
			pwText += std::wstring(pwText).size() + 1;

			// + 2 entiers que je ne sais pas ???
			pwText += std::wstring(pwText).size() + 1;
			pBuffer = (LPBYTE)pwText + 8;

			// Coordonnées
			// Premiere coordonnées
			pBuffer += sizeof(double);
			pBuffer += sizeof(double);

			// Deuxieme coordonnées
			fCoords = *(double*)pBuffer;
			cGpsPoint.lng(fCoords);
			pBuffer += sizeof(double);

			fCoords = *(double*)pBuffer;
			cGpsPoint.lat(fCoords);
			pBuffer += sizeof(double);

			// + 4 entiers que je ne sais pas ???
			pBuffer += 16;

			cGpsRoute.push_back(cGpsPoint);
		}
		else
		{
			break;
		}
	} while (pBuffer < pEndFile);

end_function:
	if (pMapFile) UnmapViewOfFile(pMapFile);
	if (pFileMapping) CloseHandle(pFileMapping);
	if (pFile) CloseHandle(pFile);

	return hr;
}
