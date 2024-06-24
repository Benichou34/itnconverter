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
 * Purpose : Writer for CoPilot 10 file (.trp)
 *           www.alk.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/bom.h"
#include "stdx/format.h"
#include "stdx/string_helper.h"

#define DATA_VERSION "Data Version:3.4.1.2" CRLF 
#define CREATOR      "Creator=" SOFT_FULL_NAME " (" SOFT_URL ")" CRLF
#define START_TRIP   "Start Trip=%s" CRLF
#define END_TRIP     "End Trip" CRLF CRLF

#define START_STOP   "Start Stop=Stop %d" CRLF
#define LONGITUDE    "Longitude=%d" CRLF
#define LATITUDE     "Latitude=%d" CRLF
#define ADDRESS      "Address=%s" CRLF
#define SHOW         "Show=%d" CRLF
#define END_STOP     "End Stop" CRLF CRLF

#define HEADER		DATA_VERSION START_TRIP END_TRIP
#define STOP		START_STOP LONGITUDE LATITUDE ADDRESS SHOW END_STOP
#define TRP_FACTOR	1000000
#define SHOW_STEP	0
#define SHOW_STOP	1

int WriteCP10(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::wofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	stdx::wbom::write(ofsFile, stdx::wbom::utf_16le);
	stdx::wbom::imbue(ofsFile, stdx::wbom::utf_16le);

	ofsFile << stdx::wstring_helper::from_utf8(stdx::format(HEADER)(cGpsRoute.name()));

	for (size_t i = 0; i < cGpsRoute.size(); i++)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];

		ofsFile << stdx::wstring_helper::from_utf8(stdx::format(STOP)(i)
			(static_cast<int>(cGpsPoint.lng() * TRP_FACTOR))
			(static_cast<int>(cGpsPoint.lat() * TRP_FACTOR))
			(cGpsPoint.name())
			((i == 0 || i == cGpsRoute.upper_bound()) ? SHOW_STOP : SHOW_STEP));
	}

	ofsFile.close();
	return ofsFile.good()?S_OK:S_FALSE;
}
