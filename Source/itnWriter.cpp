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
#include "stdx/format.h"
#include "ITN Tools.h"

#define ITN_LINE     "%06d|%07d|%s|%d|\r\n"
#define ITN_FACTOR   100000

#define TT_WAYPOINT             0 // Waypoint
#define TT_WAYPOINT_DISABLED    1 // Waypoint disabled (will be skipped when navigating the itinerary, appears dimmed in the itinerary overview)
#define TT_DESTINATION          2 // Stopover or destination
#define TT_DESTINATION_DISABLED 3 // Stopover disabled (will be skipped when navigating the itinerary, appears dimmed in the itinerary overview)
#define TT_DEPARTURE            4 // Departure point (should only be set for the first item in the itinerary file)

namespace
{
	inline long DoubleToLong(double dDouble)
	{
		return static_cast<long>(floor(dDouble * ITN_FACTOR) + 0.5);
	}
}

int WriteOneITN(const std::wstring& strPathName, const CGpsRoute& cGpsRoute)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	int nFlag;

	for (size_t i = 0; i < cGpsRoute.size(); i++)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];

		if (i == cGpsRoute.upper_bound())
			nFlag = TT_DESTINATION;
		else if (i == 0)
			nFlag = TT_DEPARTURE;
		else
			nFlag = TT_WAYPOINT;

		ofsFile << stdx::format(ITN_LINE)(DoubleToLong(cGpsPoint.lng()))(DoubleToLong(cGpsPoint.lat()))(stdx::string_helper::from_utf8(cGpsPoint.name()))(nFlag);
	}
	return S_OK;
}

int WriteITN(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool)
{
	DWORD dwWayPointNumber = cGpsRoute.size();

	if (!dwFlag || dwWayPointNumber <= dwFlag)
		return WriteOneITN(strPathName, cGpsRoute);

	DWORD dwCurrentPoint = 0;
	CString curPathName;
	CString curNameNb;
	DWORD i = 0;

	while (dwCurrentPoint < dwWayPointNumber)
	{
		i++;

		if (dwCurrentPoint)
			dwCurrentPoint--;

		DWORD dwNbPoint = dwWayPointNumber - dwCurrentPoint;

		if (dwNbPoint > dwFlag)
			dwNbPoint = dwFlag;

		curNameNb.Format(_T("_%d"), i);
		curPathName = strPathName.c_str();
		curPathName.Insert(curPathName.ReverseFind('.'), curNameNb);

		WriteOneITN((LPCWSTR)curPathName, CGpsRoute(cGpsRoute, dwCurrentPoint, dwNbPoint));
		dwCurrentPoint += dwNbPoint;
	}

	return S_OK;
}

