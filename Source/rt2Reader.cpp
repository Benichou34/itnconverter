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
 * Purpose : Reader for OziExplorer CE Route file (.rt2)
 *           www.oziexplorer.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <fstream>

#define RT2_HEADER   "H1" //,OziExplorer CE Route2 File Version 1.0" // Header
#define RT2_ROUTE    "H3" // Route
#define RT2_WAYPOINT "W" // Waypoint

static void ParseRoute(const stdx::string_helper::vector& vecResults, CGpsRoute& cGpsRoute)
{
	if (vecResults.size() > 2)
		cGpsRoute.name(stdx::string_helper::to_utf8(vecResults[1]));
}

static int ParseWaypoint(const stdx::string_helper::vector& vecResults, CGpsPointArray& cGpsPointArray)
{
	if (vecResults.size() > 4)
	{
		CGpsPoint cGpsPoint;

		cGpsPoint.name(stdx::string_helper::to_utf8(vecResults[1]));
		cGpsPoint.lat(stdx::string_helper::string_to<double>(vecResults[2]));
		cGpsPoint.lng(stdx::string_helper::string_to<double>(vecResults[3]));

		cGpsPointArray.push_back(cGpsPoint);
	}

	return S_OK;
}

int ReadRT2(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	HRESULT hr = S_FALSE;

	std::ifstream ifsFile(strPathName.c_str());
	if (!ifsFile)
		return S_FALSE;

	std::string strLine;
	while (std::getline(ifsFile, strLine))
	{
		stdx::string_helper::vector vecStrResult;

		stdx::string_helper::split(strLine, vecStrResult, stdx::find_first(","));
		if (vecStrResult.size() > 0)
		{
			if (hr == S_FALSE)
			{
				if (!vecStrResult.front().compare(RT2_HEADER))
					hr = S_OK;
			}
			else
			{
				if (!vecStrResult.front().compare(RT2_ROUTE))
				{
					vecGpsArray.push_back(new CGpsRoute());
					ParseRoute(vecStrResult, *static_cast<CGpsRoute*>(vecGpsArray.back()));
				}
				else if (!vecStrResult.front().compare(RT2_WAYPOINT) && !vecGpsArray.empty())
				{
					ParseWaypoint(vecStrResult, *vecGpsArray.back());
				}
			}
		}
	}

	return hr;
}
