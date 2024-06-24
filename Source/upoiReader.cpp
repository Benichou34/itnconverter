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
 * Purpose : Reader for iGo poi file (.upoi)
 *           igomyway.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"
#include "stdx/bom.h"

int ReadUPOI(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	std::wifstream ifsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ifsFile)
		return S_FALSE;

	stdx::wbom::imbue(ifsFile, stdx::wbom::read(ifsFile));

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	std::wstring strLine;
	while (std::getline(ifsFile, strLine))
	{
		stdx::wstring_helper::vector vecResult;
		stdx::wstring_helper::split(strLine, vecResult, stdx::wfind_first(L"|"));

		if (vecResult.size() > 5)
		{
			CGpsPoint cGpsPoint;

			cGpsPoint.name(stdx::wstring_helper::to_utf8(vecResult[2]));
			cGpsPoint.lat(stdx::wstring_helper::string_to<double>(vecResult[4]));
			cGpsPoint.lng(stdx::wstring_helper::string_to<double>(vecResult[5]));

			if (vecResult.size() > 15)
				cGpsPoint.comment(stdx::wstring_helper::to_utf8(vecResult[14])); // Igo 8.3
			else
				cGpsPoint.comment(stdx::wstring_helper::to_utf8(vecResult[13])); // Igo 8.0

			if (cGpsPoint)
				cGpsRoute.push_back(cGpsPoint);
		}
	}

	cGpsRoute.removeEmpties();

	return S_OK;
}
