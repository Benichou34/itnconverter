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
 * Purpose : Writer for Microsoft Autoroute and MapPoint files (.axe, .ptm)
 */

#include "stdafx.h"
#include "axeHeader.h"

static BYTE bMiddlePadding[3] = { 0xFF, 0xFE, 0xFF };

static sJourneyPointData sDefaultPointData =
{
	0, // nRouteType
	1, // nField1
	4, // nField2
	0, // nField3
	1, // nField4
	0, // nIdentifier
	0, // dDistance
	0, // nStopLatitude
	0, // nStopLongitude
	0, // nStartLatitude
	0, // nStartLongitude
	0, // iunkn8
	0, // iunkn9
	0, // iunkn10
	0, // iunkn11
	6, // iunkn12
	0, // iunkn13
	0  // iunkn14
};

static int CreateAXE(const std::wstring& strPathName, UINT unRessource)
{
	HRESULT hr = S_OK;
	HRSRC hrRes = nullptr;
	HGLOBAL hRes = nullptr;
	LPBYTE lpRes = nullptr;
	DWORD dwResSize = 0;
	HANDLE pFile = nullptr;
	DWORD dwNumberOfBytesWritten = 0;

	// Load ressource to memory
	hrRes = FindResource(nullptr, MAKEINTRESOURCE(unRessource), _T("AXE_SRC"));
	if (!hrRes)
	{
		hr = S_FALSE;
		goto end_function;
	}

	hRes = LoadResource(nullptr, hrRes);
	if (!hRes)
	{
		hr = S_FALSE;
		goto end_function;
	}

	dwResSize = SizeofResource(nullptr, hrRes);
	lpRes = (LPBYTE)LockResource(hRes);
	if (!lpRes || !dwResSize)
	{
		hr = S_FALSE;
		goto end_function;
	}

	// Write ressource to a file
	pFile = CreateFile(strPathName.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (!pFile)
	{
		hr = GetLastError();
		goto end_function;
	}

	if (!WriteFile(pFile, lpRes, dwResSize, &dwNumberOfBytesWritten, nullptr))
	{
		hr = S_FALSE;
	}

end_function:
	if (pFile)
		CloseHandle(pFile);

	return hr;
}

static int WriteTemplate(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, UINT unRessource)
{
	LPBYTE* ppTabStream = nullptr;
	DWORD* pdwTabLength = nullptr;
	DWORD dwLength = 0;
	LPBYTE lpPos = nullptr;
	int i;

	STATSTG statstg;
	IStorage* pStg = nullptr;
	IEnumSTATSTG* penum = nullptr;
	IStream* isStream = nullptr;
	LPBYTE lpStreamContent = nullptr;
	DWORD dwStreamLength = 0;
	DWORD dwMapVersion = 0;

	int nWayPointNumber = cGpsRoute.size();

	memset(&statstg, 0, sizeof(statstg));

	// First, create file
	HRESULT hr = CreateAXE(strPathName, unRessource);
	if (FAILED(hr))
		goto end_function;

	/* Retrieve file version */
	hr = GetStream(strPathName, STREAM_PROPERTIES, &lpStreamContent, &dwStreamLength);
	if (FAILED(hr) || !lpStreamContent || !dwStreamLength)
		goto end_function;

	dwMapVersion = ReadVersion(lpStreamContent, dwStreamLength);

	if (lpStreamContent)
		free(lpStreamContent);

	lpStreamContent = nullptr;
	dwStreamLength = 0;

	// Prepare future stream buffer
	ppTabStream = new LPBYTE[nWayPointNumber];
	if (!ppTabStream)
	{
		hr = E_OUTOFMEMORY;
		goto end_function;
	}

	pdwTabLength = new DWORD[nWayPointNumber];
	if (!pdwTabLength)
	{
		hr = E_OUTOFMEMORY;
		goto end_function;
	}

	memset(ppTabStream, 0x00, nWayPointNumber * sizeof(LPBYTE));
	memset(pdwTabLength, 0x00, nWayPointNumber * sizeof(DWORD));

	for (i = 0; i < nWayPointNumber; i++)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];

		std::wstring wName = stdx::wstring_helper::from_utf8(cGpsPoint.name());
		BYTE cbNameSize = (BYTE)wName.size();

		dwLength = sizeof(sJourneyPointHeader) + sizeof(sJourneyPointMiddle) + sizeof(sJourneyPointData) + (cbNameSize * 3);
		if (dwMapVersion >= MAP_VERSION_2007)
			dwLength += AE2007_PADDING + 1;

		ppTabStream[i] = new BYTE[dwLength];
		if (!ppTabStream[i])
		{
			hr = E_OUTOFMEMORY;
			goto end_function;
		}

		memset(ppTabStream[i], 0x00, dwLength);
		pdwTabLength[i] = dwLength;

		lpPos = ppTabStream[i];
		((sJourneyPointHeader*)lpPos)->cbCommentLength = cbNameSize;

		((sJourneyPointHeader*)lpPos)->nLatitude = DEG_TO_AXE(cGpsPoint.lat());
		((sJourneyPointHeader*)lpPos)->nLongitude = DEG_TO_AXE(cGpsPoint.lng());

		lpPos += sizeof(sJourneyPointHeader);

		// Insert padding for 2007 version
		if (dwMapVersion >= MAP_VERSION_2007)
		{
			lpPos += AE2007_PADDING;

			*lpPos = cbNameSize;
			lpPos++;
		}

		// Write comment
		memcpy(lpPos, stdx::string_helper::narrow(wName).data(), cbNameSize);
		lpPos += cbNameSize;

		// Write Middle
		memcpy(((sJourneyPointMiddle*)lpPos)->padding, bMiddlePadding, sizeof(bMiddlePadding));
		((sJourneyPointMiddle*)lpPos)->cbCommentLength = cbNameSize;
		lpPos += sizeof(sJourneyPointMiddle);

		memcpy(lpPos, wName.data(), cbNameSize * 2);
		lpPos += cbNameSize * 2;

		// Write data
		memcpy(lpPos, &sDefaultPointData, sizeof(sJourneyPointData));
		((sJourneyPointData*)lpPos)->nStartLatitude = DEG_TO_AXE(cGpsPoint.lat());
		((sJourneyPointData*)lpPos)->nStartLongitude = DEG_TO_AXE(cGpsPoint.lng());
		((sJourneyPointData*)lpPos)->nStopLatitude = DEG_TO_AXE(cGpsPoint.lat());
		((sJourneyPointData*)lpPos)->nStopLongitude = DEG_TO_AXE(cGpsPoint.lng());
	}

	// Calcul total length
	dwLength = 0;
	for (i = 0; i < nWayPointNumber; i++)
		dwLength += pdwTabLength[i];

	hr = StgOpenStorage(strPathName.c_str(),
		nullptr,
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
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
		// Journey is a Stream
		if (statstg.type == STGTY_STREAM && !wcscmp(statstg.pwcsName, L"Journey"))
		{
			// Open stream
			hr = pStg->OpenStream(statstg.pwcsName, nullptr, STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &isStream);
			if (FAILED(hr))
				goto end_function;

			// Allocate buffer for receive stream
			dwStreamLength = statstg.cbSize.LowPart + dwLength;
			lpStreamContent = new BYTE[dwStreamLength];
			if (!lpStreamContent)
			{
				hr = E_OUTOFMEMORY;
				goto end_function;
			}

			// Read stream
			hr = isStream->Read(lpStreamContent, statstg.cbSize.LowPart, nullptr);
			if (FAILED(hr))
				goto end_function;

			// Construct new stream
			((sJourneyHeader*)lpStreamContent)->dwPointsNumber = nWayPointNumber;
			((sJourneyHeader*)lpStreamContent)->nField1 = 0; // IT'S WORKS !!!
			((sJourneyHeader*)lpStreamContent)->nField2 = 6; // IT'S WORKS !!!
			lpPos = lpStreamContent + sizeof(sJourneyHeader);
			memcpy(lpPos + dwLength, lpPos, statstg.cbSize.LowPart - sizeof(sJourneyHeader));

			// Add road points
			for (i = 0; i < nWayPointNumber; i++)
			{
				memcpy(lpPos, ppTabStream[i], pdwTabLength[i]);
				lpPos += pdwTabLength[i];
			}

			// Grow stream
			ULARGE_INTEGER dNewSize;
			dNewSize.QuadPart = dwStreamLength;

			hr = isStream->SetSize(dNewSize);
			if (FAILED(hr))
				goto end_function;

			// Move to the begining
			LARGE_INTEGER dlibMove;
			dlibMove.QuadPart = 0;

			hr = isStream->Seek(dlibMove, STREAM_SEEK_SET, nullptr);
			if (FAILED(hr))
				goto end_function;

			// Write modified stream
			ULONG cbWritten = 0;
			hr = isStream->Write(lpStreamContent, dwStreamLength, &cbWritten);
			if (FAILED(hr) || !cbWritten)
				goto end_function;

			break;
		}

		// Move on to the next element in the enumeration of this storage.
		CoTaskMemFree(statstg.pwcsName);
		memset(&statstg, 0, sizeof(statstg));
		hr = penum->Next(1, &statstg, 0);
	}

end_function:
	// Free memory
	if (lpStreamContent && ppTabStream)
		delete lpStreamContent;

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

	if (ppTabStream)
	{
		for (i = 0; i < nWayPointNumber; i++)
			delete[] ppTabStream[i];
	}

	delete[] pdwTabLength;
	delete[] ppTabStream;

	return hr;
}

int WriteAXE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	return WriteTemplate(strPathName, cGpsRoute, IDR_AXESRC);
}

int WriteEST(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	return WriteTemplate(strPathName, cGpsRoute, IDR_ESTSRC);
}

int WritePTM(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	return WriteTemplate(strPathName, cGpsRoute, IDR_PTMSRC);
}
