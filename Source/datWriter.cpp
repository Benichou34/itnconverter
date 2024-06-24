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
 * Purpose : Writer for Destinator PN file (.dat)
 *           www.destinatortechnologies.com
 */

#include "stdafx.h"
#include "ITN Tools.h"

 //#define DAT_HEADER  L"City->Street"
#define DAT_HEADER  L"Poi..."
static BYTE lpPattern1[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static BYTE lpPattern2[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

int WriteDAT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	HANDLE pFile = nullptr;
	DWORD dwNumberOfBytesWritten = 0;
	std::wstring wstrTmp;
	double fLong, fLat;

	pFile = CreateFile(strPathName.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (!pFile)
		return GetLastError();

	CGpsRoute::const_iterator it;
	for (it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
	{
		const CGpsPoint& cGpsPoint = *it;

		WriteFile(pFile, DAT_HEADER, (wcslen(DAT_HEADER) + 1) * 2, &dwNumberOfBytesWritten, nullptr);

		stdx::wstring_helper::from_utf8(cGpsPoint.name(), wstrTmp);
		WriteFile(pFile, wstrTmp.c_str(), (wstrTmp.size() + 1) * 2, &dwNumberOfBytesWritten, nullptr);

		stdx::wstring_helper::from_utf8(cGpsPoint.comment(), wstrTmp);
		WriteFile(pFile, wstrTmp.c_str(), (wstrTmp.size() + 1) * 2, &dwNumberOfBytesWritten, nullptr);

		WriteFile(pFile, lpPattern1, sizeof(lpPattern1), &dwNumberOfBytesWritten, nullptr);

		fLong = cGpsPoint.lng();
		fLat = cGpsPoint.lat();
		WriteFile(pFile, &fLong, sizeof(double), &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &fLat, sizeof(double), &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &fLong, sizeof(double), &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &fLat, sizeof(double), &dwNumberOfBytesWritten, nullptr);

		WriteFile(pFile, lpPattern2, sizeof(lpPattern2), &dwNumberOfBytesWritten, nullptr);
	}

	CloseHandle(pFile);

	return S_OK;
}

