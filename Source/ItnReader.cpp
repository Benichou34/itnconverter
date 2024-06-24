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
 * Purpose : Reader for TomTom file (.itn)
 *           www.tomtom.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"
#include "stdx/bom.h"

#define ITN_FACTOR   100000.

int ReadITN(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	std::ifstream ifsFile(strPathName);
	if (!ifsFile)
		return S_FALSE;

	bool bUtf8 = (stdx::bom::read(ifsFile) == stdx::bom::utf_8); // UTF-8

	vecGpsArray.push_back(new CGpsRoute);
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	std::string strLine;
	while (std::getline(ifsFile, strLine))
	{
		stdx::string_helper::vector vecResult;

		stdx::string_helper::split(strLine, vecResult, stdx::find_first("|"));
		if (vecResult.size() > 3)
		{
			CGpsPoint cGpsPoint;

			cGpsPoint.lng(stdx::string_helper::string_to<int>(vecResult[0]) / ITN_FACTOR);
			cGpsPoint.lat(stdx::string_helper::string_to<int>(vecResult[1]) / ITN_FACTOR);
			cGpsPoint.name(bUtf8 ? vecResult[2] : stdx::string_helper::to_utf8(vecResult[2]));

			cGpsRoute.push_back(cGpsPoint);
		}
	}

	return S_OK;
}
