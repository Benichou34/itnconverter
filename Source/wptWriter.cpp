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
 * Purpose : Writer for OziExplorer Waypoint file (.wpt)
 *           http://www.oziexplorer3.com/eng/help/fileformats.html
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <fstream>

#define WPT_HEAD     "OziExplorer Waypoint File Version 1.0" CRLF "WGS 84" CRLF "Reserved 2" CRLF "Reserved 3" CRLF
#define WPT_RECORD   "%d, %s, %+011.7f, %+012.7f,, 18, 1, 3, 0, 65535, %s, 0, 0, 0, %.5f" CRLF

int WriteWPT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	ofsFile << WPT_HEAD;

	size_t i = 1;
	for (CGpsRoute::const_iterator cit = cGpsRoute.begin(); cit != cGpsRoute.end(); ++cit, ++i)
	{
		const CGpsPoint& cGpsPoint = *cit;

		// Remove comma
		std::string strText(stdx::string_helper::from_utf8(cGpsPoint.name()));
		std::string strDesc(stdx::string_helper::from_utf8(cGpsPoint.comment()));

		stdx::string_helper::replace(strText, ',', ' ');
		stdx::string_helper::replace(strDesc, ',', ' ');

		ofsFile << stdx::format(WPT_RECORD)(i)(strText)(cGpsPoint.lat())(cGpsPoint.lng())(strDesc.substr(0, 40))(MTR2FEET(cGpsPoint.alt()));
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
