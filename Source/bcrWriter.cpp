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
 * Purpose : Writer for Marco Polo and Motorrad Routenplaner file (.bcr)
 *           www.marcopolo.de
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "stdx/string_helper.h"

#define SECTION_CLIENT        "CLIENT"
#define SECTION_COORDINATES   "COORDINATES"
#define SECTION_DESCRIPTION   "DESCRIPTION"
#define SECTION_ROUTE         "ROUTE"

#define KEY_REQUEST           "REQUEST"
#define KEY_ROUTENAME         "ROUTENAME"
#define KEY_DESCRIPTIONLINES  "DESCRIPTIONLINES"
#define KEY_DESCRIPTION       "DESCRIPTION%d"
#define KEY_STATION           "STATION%d"
#define KEY_ROUTERECT         "ROUTERECT"

#define VALUE_REQUEST         "TRUE"
#define VALUE_STATION_CLIENT  "Standort,999999999"
#define VALUE_STATION_COORDS  "%d,%d"
#define VALUE_ROUTERECT       "%d,%d,%d,%d"

int WriteBCR(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	size_t i;
	std::string szPathName = stdx::string_helper::narrow(strPathName);

	// Write request
	if (!WritePrivateProfileStringA(SECTION_CLIENT, KEY_REQUEST, VALUE_REQUEST, szPathName.c_str()))
		return GetLastError();

	// Write route name
	if (!WritePrivateProfileStringA(SECTION_CLIENT, KEY_ROUTENAME, stdx::string_helper::from_utf8(cGpsRoute.name()).c_str(), szPathName.c_str()))
		return GetLastError();

	// Write Client Station
	for (i = 0; i < cGpsRoute.size(); i++)
	{
		if (!WritePrivateProfileStringA(SECTION_CLIENT, stdx::format(KEY_STATION)(i + 1).str().c_str(), VALUE_STATION_CLIENT, szPathName.c_str()))
			return GetLastError();
	}

	// Write Coordinates
	for (i = 0; i < cGpsRoute.size(); i++)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];

		geo::CGeoMercatorXY gMercatorXY(cGpsPoint);

		if (!WritePrivateProfileStringA(SECTION_COORDINATES, stdx::format(KEY_STATION)(i + 1).str().c_str(), stdx::format(VALUE_STATION_COORDS)(gMercatorXY.x())(gMercatorXY.y()).str().c_str(), szPathName.c_str()))
			return GetLastError();
	}

	// Write Descriptions
	for (i = 0; i < cGpsRoute.size(); i++)
	{
		if (!WritePrivateProfileStringA(SECTION_DESCRIPTION, stdx::format(KEY_STATION)(i + 1).str().c_str(), stdx::string_helper::from_utf8(cGpsRoute[i].name()).c_str(), szPathName.c_str()))
			return GetLastError();
	}

	return S_OK;
}