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
 * Purpose : Writer for OziExplorer Track file (.plt)
 *           http://www.oziexplorer3.com/eng/help/fileformats.html
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <fstream>

#define PLT_HEAD     "OziExplorer Track Point File Version 2.1" CRLF "WGS 84" CRLF "Altitude is in Feet" CRLF "Reserved 3" CRLF
#define PLT_NAME     "0, 2, 65280, %s, 1, 0, 0, 65280" CRLF
#define PLT_PTNB     " %d" CRLF
#define PLT_RECORD   "%+011.7f, %+012.7f, 0, %.5f, , ," CRLF

int WritePLT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	ofsFile << PLT_HEAD << stdx::format(PLT_NAME)(stdx::string_helper::from_utf8(cGpsRoute.name())) << stdx::format(PLT_PTNB)(cGpsRoute.size());

	for (CGpsRoute::const_iterator cit = cGpsRoute.begin(); cit != cGpsRoute.end(); ++cit)
	{
		const geo::CGeoLatLng& cGpsLatlng = *cit;
		ofsFile << stdx::format(PLT_RECORD)(cGpsLatlng.lat())(cGpsLatlng.lng())(MTR2FEET(cGpsLatlng.alt()));
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
