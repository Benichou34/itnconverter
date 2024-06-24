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
 * Purpose : Reader for Microsoft Autoroute and MapPoint files (.axe, .ptm)
 */

#include "stdafx.h"
#include "axeHeader.h"

HRESULT GetStream(const std::wstring& wstrFileName, const std::wstring& wstrStreamName, BYTE** ppBuffer, DWORD* pdwLength)
{
	HRESULT hr = S_OK;
	STATSTG statstg;
	IStorage* pStg = nullptr;
	IEnumSTATSTG* penum = nullptr;
	IStream* isStream = nullptr;

	if (!ppBuffer || !pdwLength)
		return E_INVALIDARG;

	memset(&statstg, 0, sizeof(statstg));
	// Open the root storage.
	hr = StgOpenStorage(wstrFileName.c_str(),
		nullptr,
		STGM_READ | STGM_SHARE_DENY_WRITE,
		nullptr,
		0,
		&pStg);

	if (FAILED(hr))
		goto end_function;

	// Get an enumerator for this storage.
	hr = pStg->EnumElements(0, nullptr, 0, &penum);
	if (FAILED(hr))
		goto end_function;

	// Get the name of the first element (stream/storage) in the enumeration.
	penum->Reset();
	hr = penum->Next(1, &statstg, 0);
	while (hr == S_OK)
	{
		// Search Stream
		if (statstg.type == STGTY_STREAM && !wstrStreamName.compare(statstg.pwcsName))
		{
			hr = pStg->OpenStream(statstg.pwcsName, nullptr, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &isStream);
			if (FAILED(hr))
				goto end_function;

			*pdwLength = statstg.cbSize.LowPart;
			*ppBuffer = (BYTE*)calloc(*pdwLength, sizeof(BYTE));
			if (!*ppBuffer)
			{
				hr = E_OUTOFMEMORY;
				goto end_function;
			}

			hr = isStream->Read(*ppBuffer, *pdwLength, nullptr);
			if (FAILED(hr))
				goto end_function;

			break;
		}

		// Move on to the next element in the enumeration of this storage.
		CoTaskMemFree(statstg.pwcsName);
		memset(&statstg, 0, sizeof(statstg));
		hr = penum->Next(1, &statstg, 0);
	}

end_function:
	if (statstg.pwcsName)
	{
		CoTaskMemFree(statstg.pwcsName);
		memset(&statstg, 0, sizeof(statstg));
	}

	if (isStream)
		isStream->Release();

	if (penum)
		penum->Release();

	if (pStg)
		pStg->Release();

	return hr;
}

int ReadOldAXE(const std::wstring& strPathName, CGpsRoute& cGpsRoute)
{
	HRESULT hr = S_OK;
	HANDLE pFile = nullptr;
	HANDLE pFileMapping = nullptr;
	LPBYTE pMapFile = nullptr;
	LPBYTE pStreamContent = nullptr;
	DWORD dwWayPointNumber = 0;
	sJourneyHeader* pJourneyHeader = nullptr;
	sJourneyPointHeader* pJourneyPointHeader = nullptr;
	sJourneyPointMiddle* pJourneyPointMiddle = nullptr;
	DWORD i;

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

	pStreamContent = pMapFile + 2;
	pJourneyHeader = (sJourneyHeader*)pStreamContent;

	// Check Identifier
	if (memcmp(pJourneyHeader->id, JOURNEY_ID, 4))
	{
		// Error must return
		hr = S_FALSE;
		goto end_function;
	}

	dwWayPointNumber = pJourneyHeader->dwPointsNumber;
	if (!dwWayPointNumber)
		goto end_function;

	pStreamContent += sizeof(sJourneyHeader);
	for (i = 0; i < dwWayPointNumber; i++)
	{
		CGpsPoint cGpsPoint;

		// Read header
		pJourneyPointHeader = (sJourneyPointHeader*)pStreamContent;
		pStreamContent += sizeof(sJourneyPointHeader);

		// Read char* text
		cGpsPoint.name(std::string((char*)pStreamContent, pJourneyPointHeader->cbCommentLength));
		pStreamContent += pJourneyPointHeader->cbCommentLength;

		pJourneyPointMiddle = (sJourneyPointMiddle*)pStreamContent;
		pStreamContent += sizeof(sJourneyPointMiddle);

		// Read wchar* text
		pStreamContent += (pJourneyPointMiddle->cbCommentLength * 2) + 20;

		cGpsPoint.lat(AXE_TO_DEG(pJourneyPointHeader->nLatitude));
		cGpsPoint.lng(AXE_TO_DEG(pJourneyPointHeader->nLongitude));

		cGpsRoute.push_back(cGpsPoint);
	}

end_function:
	if (pMapFile) UnmapViewOfFile(pMapFile);
	if (pFileMapping) CloseHandle(pFileMapping);
	if (pFile) CloseHandle(pFile);

	return hr;
}

DWORD ReadVersion(LPBYTE pBuffer, DWORD /*dwLength*/)
{
	sPropertyHeader* pPropertyHeader;
	sSectionHeader* pSectionHeader;
	sPropertyContent* pPropertyContent = nullptr;
	DWORD i;

	pPropertyHeader = (sPropertyHeader*)pBuffer;
	pSectionHeader = (sSectionHeader*)(pBuffer + pPropertyHeader->dwOffset);

	for (i = 0; i < pSectionHeader->dwPropertiesNumber; i++)
	{
		if (((sSectionOffset*)&(pSectionHeader->pSectionOffset))[i].dwId == PROP_VERSION_ID)
		{
			pPropertyContent = (sPropertyContent*)((LPBYTE)pSectionHeader + ((sSectionOffset*)&(pSectionHeader->pSectionOffset))[i].dwOffset);
			break;
		}
	}

	if (pPropertyContent && pPropertyContent->dwType == PROP_TYPE_DWORD)
		return pPropertyContent->dwValue;

	return 0;
}

int ReadAXE(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	LPBYTE pStreamContent = nullptr;
	DWORD dwLength = 0;
	DWORD dwCurrentLength = 0;
	sJourneyHeader* pJourneyHeader = nullptr;
	sJourneyPointHeader* pJourneyPointHeader = nullptr;
	sJourneyPointMiddle* pJourneyPointMiddle = nullptr;
	sJourneyPointData* pJourneyPointData = nullptr;
	BYTE cbCommentLength;
	DWORD dwMapVersion = 0;
	DWORD dwWayPointNumber = 0;
	DWORD i = 0;

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	/* Read file version */
	HRESULT hr = GetStream(strPathName, STREAM_PROPERTIES, &pStreamContent, &dwLength);
	if (FAILED(hr) || !pStreamContent || !dwLength)
		goto end_function;

	dwMapVersion = ReadVersion(pStreamContent, dwLength);

	if (pStreamContent)
	{
		free(pStreamContent);
		pStreamContent = nullptr;
	}

	/* Read roadbook */
	hr = GetStream(strPathName, STREAM_JOURNEY, &pStreamContent, &dwLength);
	if (FAILED(hr) || !pStreamContent || !dwLength)
		goto end_function;

	pJourneyHeader = (sJourneyHeader*)pStreamContent;
	dwWayPointNumber = pJourneyHeader->dwPointsNumber;

	if (!dwWayPointNumber)
		goto end_function;

	dwCurrentLength = sizeof(sJourneyHeader);
	for (i = 0; i < dwWayPointNumber; i++)
	{
		CGpsPoint cGpsPoint;

		// Read header
		pJourneyPointHeader = (sJourneyPointHeader*)(pStreamContent + dwCurrentLength);
		dwCurrentLength += sizeof(sJourneyPointHeader);

		if (dwMapVersion < MAP_VERSION_2007)
		{
			cbCommentLength = pJourneyPointHeader->cbCommentLength;
		}
		else if (dwMapVersion < MAP_VERSION_2009)
		{
			dwCurrentLength += AE2007_PADDING;

			cbCommentLength = *(pStreamContent + dwCurrentLength);
			dwCurrentLength++;
		}
		else
		{
			dwCurrentLength += AE2009_PADDING;

			cbCommentLength = *(pStreamContent + dwCurrentLength);
			dwCurrentLength++;
		}

		// Read char* text
//		cGpsPoint.name() = std::string((char*)(pStreamContent + dwCurrentLength), cbCommentLength);
		dwCurrentLength += cbCommentLength;
		pJourneyPointMiddle = (sJourneyPointMiddle*)(pStreamContent + dwCurrentLength);
		dwCurrentLength += sizeof(sJourneyPointMiddle);

		// Read wchar* text
		cGpsPoint.name(stdx::wstring_helper::to_utf8(std::wstring((wchar_t*)(pStreamContent + dwCurrentLength), cbCommentLength)));
		dwCurrentLength += pJourneyPointMiddle->cbCommentLength * 2;

		// Read data
		pJourneyPointData = (sJourneyPointData*)(pStreamContent + dwCurrentLength);
		dwCurrentLength += sizeof(sJourneyPointData);

		if (abs(pJourneyPointData->nStartLatitude - pJourneyPointHeader->nLatitude) > MAX_DIFF_SC_RD
			|| abs(pJourneyPointData->nStopLongitude - pJourneyPointHeader->nLongitude) > MAX_DIFF_SC_RD)
		{
			cGpsPoint.lat(AXE_TO_DEG(pJourneyPointData->nStartLatitude));
			cGpsPoint.lng(AXE_TO_DEG(pJourneyPointData->nStopLongitude));
		}
		else
		{
			cGpsPoint.lat(AXE_TO_DEG(pJourneyPointHeader->nLatitude));
			cGpsPoint.lng(AXE_TO_DEG(pJourneyPointHeader->nLongitude));
		}

		cGpsRoute.push_back(cGpsPoint);
	}

end_function:
	if (pStreamContent)
		free(pStreamContent);

	/* No storage object, try to open old format */
	if (hr == STG_E_FILEALREADYEXISTS)
		hr = ReadOldAXE(strPathName, cGpsRoute);

	return hr;
}
