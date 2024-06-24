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
 * Purpose : Writer for iGo poi file (.upoi)
 *           igomyway.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/bom.h"
#include "stdx/format.h"
#include "stdx/string_helper.h"

#define UPOI_FORMAT_80 "%d|" SOFT_NAME "|%s||%.6f|%.6f||||||||%s|" CRLF
#define UPOI_FORMAT_83 "%d|" SOFT_NAME "|%s||%.6f|%.6f|||||||||%s||" CRLF

static int WriteUPOI(const std::wstring& strPathName, const std::string& strFormat, const CGpsRoute& cGpsRoute)
{
	std::wofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	stdx::wbom::write(ofsFile, stdx::wbom::utf_16le);
	stdx::wbom::imbue(ofsFile, stdx::wbom::utf_16le);

	for (size_t i = 0; i < cGpsRoute.size(); ++i)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];

		ofsFile << stdx::wstring_helper::from_utf8(stdx::format(strFormat)(i)
			(cGpsPoint.name())
			(cGpsPoint.lat())
			(cGpsPoint.lng())
			(cGpsPoint.comment()));
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}

int WriteUPOI80(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	return WriteUPOI(strPathName, UPOI_FORMAT_80, cGpsRoute);
}

int WriteUPOI83(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	return WriteUPOI(strPathName, UPOI_FORMAT_83, cGpsRoute);
}
