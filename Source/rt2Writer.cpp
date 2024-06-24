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
 * Purpose : Writer for OziExplorer CE Route file (.rt2)
 *           www.oziexplorer.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <fstream>

#define RT2_HEADER   "H1,OziExplorer CE Route2 File Version 1.0" CRLF "H2,WGS 84" CRLF
#define RT2_LINE     "H3,%s,,0" CRLF
#define WPT_LINE     "W,%s,%.7f,%.7f,0" CRLF

int WriteRT2(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	ofsFile << RT2_HEADER << stdx::format(RT2_LINE)(stdx::string_helper::from_utf8(cGpsRoute.name()));

	for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
	{
		const CGpsPoint& cGpsPoint = *it;

		// Remove comma
		std::string strText(stdx::string_helper::from_utf8(cGpsPoint.name()));
		stdx::string_helper::replace(strText, ',', ' ');

		ofsFile << stdx::format(WPT_LINE)(strText)(cGpsPoint.lat())(cGpsPoint.lng());
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
