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
 * Purpose : Reader for Falk Navigator file (.tour)
 *           www.gofalk.com
 */

#include "stdafx.h"
#include "ITN Tools.h"

#define SECTION_TOUR  _T("TOUR")

#define KEY_NAME      _T("Name")
#define KEY_POSITION  _T("PositionInList")
#define KEY_LONGITUDE _T("Longitude")
#define KEY_LATITUDE  _T("Latitude")

#define VALUE_ERROR   _T("R_ERROR")

int ReadFLK(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	int i = 0;
	std::wstring strReadString;

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	// Read Route name
	GetPrivateProfileStdString(SECTION_TOUR, KEY_NAME, strPathName, strReadString, VALUE_ERROR);
	if (strReadString.compare(VALUE_ERROR))
		cGpsRoute.name(stdx::wstring_helper::to_utf8(strReadString));

	// Read Steps
	do
	{
		std::wstring strSection(stdx::wstring_helper::to_string(i));
		i++;

		GetPrivateProfileStdString(strSection.c_str(), KEY_POSITION, strPathName, strReadString, VALUE_ERROR);
		if (strReadString.compare(VALUE_ERROR))
		{
			CGpsPoint cGpsPoint;
			geo::CGeoMercatorXY gMercatorXY;

			GetPrivateProfileStdString(strSection.c_str(), KEY_NAME, strPathName, strReadString, VALUE_ERROR);
			if (strReadString.compare(VALUE_ERROR))
				cGpsPoint.name(stdx::wstring_helper::to_utf8(strReadString));

			GetPrivateProfileStdString(strSection.c_str(), KEY_LONGITUDE, strPathName, strReadString, VALUE_ERROR);
			if (strReadString.compare(VALUE_ERROR))
				gMercatorXY.x(stdx::wstring_helper::string_to<int>(strReadString));

			GetPrivateProfileStdString(strSection.c_str(), KEY_LATITUDE, strPathName, strReadString, VALUE_ERROR);
			if (strReadString.compare(VALUE_ERROR))
				gMercatorXY.y(stdx::wstring_helper::string_to<int>(strReadString));

			if (gMercatorXY)
			{
				cGpsPoint = gMercatorXY;
				cGpsRoute.push_back(cGpsPoint);
			}
		}
	} while (strReadString.compare(VALUE_ERROR));

	return S_OK;
}
