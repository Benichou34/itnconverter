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
 * Purpose : reader for GoClever/Mireo viaGPS file (.txt, .rt)
 *           www.goclever.net / www.mireo.hr
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"
#include "stdx/bom.h"

#define GCL_INFOTAG	'#'
#define GCL_NAME	"NAME="
#define GCL_ADDRESS	"STREET="
#define GCL_POINT	"PT="

static void SearchField(const std::string& strLine, const std::string& strField, std::string& strValue)
{
	strValue.clear();

	size_t pos = strLine.find(strField);
	if (pos != std::string::npos)
	{
		size_t begin = strLine.find('"', pos + strField.size());
		if (begin != std::string::npos)
		{
			begin++;
			size_t end = strLine.find('"', begin);
			if (end != std::string::npos)
			{
				strValue = strLine.substr(begin, end - begin);
			}
		}
	}
}

int ReadGCL(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	std::ifstream ifsFile(strPathName, std::ios_base::binary);
	if (!ifsFile)
		return S_FALSE;

	bool bUtf8 = (stdx::bom::read(ifsFile) == stdx::bom::utf_8); // UTF-8

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	std::string strLine;
	while (std::getline(ifsFile, strLine))
	{
		std::string strValue;

		stdx::string_helper::trimleft(strLine);
		if (strLine[0] == GCL_INFOTAG)
		{
			SearchField(strLine, GCL_NAME, strValue);
			if (!strValue.empty())
				cGpsRoute.name(bUtf8 ? strValue : stdx::string_helper::to_utf8(strValue));
		}

		// Read coords
		SearchField(strLine, GCL_POINT, strValue);
		if (!strValue.empty())
		{
			stdx::string_helper::vector vecCoords;
			stdx::string_helper::split(strValue, vecCoords, stdx::find_first_of(" "), false);

			if (vecCoords.size() > 1)
			{
				CGpsPoint cGpsPoint;

				cGpsPoint.lng(stdx::string_helper::string_to<double>(vecCoords[0]));
				cGpsPoint.lat(stdx::string_helper::string_to<double>(vecCoords[1]));

				// Read address
				SearchField(strLine, GCL_ADDRESS, strValue);
				cGpsPoint.name(bUtf8 ? strValue : stdx::string_helper::to_utf8(strValue));

				cGpsRoute.push_back(cGpsPoint);
			}
		}
	}

	ifsFile.close();
	return ifsFile.bad() ? S_FALSE : S_OK;
}
