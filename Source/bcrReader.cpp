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
 * Purpose : Reader for Marco Polo and Motorrad Routenplaner file (.bcr)
 *           www.marcopolo.de
 */

#include "stdafx.h"
#include "ITN Tools.h"

#define SECTION_CLIENT        _T("CLIENT")
#define SECTION_COORDINATES   _T("COORDINATES")
#define SECTION_DESCRIPTION   _T("DESCRIPTION")
#define SECTION_ROUTE         _T("ROUTE")

#define KEY_REQUEST           _T("REQUEST")
#define KEY_ROUTENAME         _T("ROUTENAME")
#define KEY_DESCRIPTIONLINES  _T("DESCRIPTIONLINES")
#define KEY_DESCRIPTION       _T("DESCRIPTION%d")
#define KEY_STATION           _T("STATION%d")
#define KEY_ROUTERECT         _T("ROUTERECT")

#define VALUE_REQUEST         _T("TRUE")
#define VALUE_STATION_CLIENT  _T("Standort,999999999")
#define VALUE_STATION_COORDS  _T("%d,%d")
#define VALUE_ROUTERECT       _T("%d,%d,%d,%d")

#define VALUE_ERROR           _T("R_ERROR")

int ReadBCR(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	int i;
	std::wstring strReadString;
	int nX, nY;

	// Read request
	GetPrivateProfileStdString(SECTION_CLIENT, KEY_REQUEST, strPathName, strReadString, VALUE_ERROR);
	if (!strReadString.compare(VALUE_ERROR))
		return GetLastError();

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	// Read Route name
	GetPrivateProfileStdString(SECTION_CLIENT, KEY_ROUTENAME, strPathName, strReadString, VALUE_ERROR);
	if (strReadString.compare(VALUE_ERROR))
		cGpsRoute.name(stdx::wstring_helper::to_utf8(strReadString));

	// Read station coordinates
	i = 0;
	do
	{
		i++;

		GetPrivateProfileStdString(SECTION_COORDINATES, stdx::wformat(KEY_STATION)(i), strPathName, strReadString, VALUE_ERROR);
		if (strReadString.compare(VALUE_ERROR))
		{
			swscanf(strReadString.c_str(), VALUE_STATION_COORDS, &nX, &nY);

			CGpsPoint cGpsPoint;

			cGpsPoint = geo::CGeoMercatorXY(nX, nY);
			cGpsRoute.push_back(cGpsPoint);
		}
	} while (strReadString.compare(VALUE_ERROR));


	// Read station description
	i = 0;
	do
	{
		i++;

		GetPrivateProfileStdString(SECTION_DESCRIPTION, stdx::wformat(KEY_STATION)(i), strPathName, strReadString, VALUE_ERROR);
		if (strReadString.compare(VALUE_ERROR))
			cGpsRoute[i - 1].name(stdx::wstring_helper::to_utf8(strReadString));
	} while (strReadString.compare(VALUE_ERROR));

	return S_OK;
}
