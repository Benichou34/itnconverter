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
 * Purpose : Reader for Magellan Route file (.rte)
 *           www.magellangps.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"

#define PMGNRTE  "$PMGNRTE"  // Route Information.
#define PMGNTRK  "$PMGNTRK"  // Track information
#define PMGNWPL  "$PMGNWPL"  // Description of a single waypoint

#define NORTH  'N'   // +
#define SOUTH  'S'   // -
#define EAST   'E'   // +
#define WEAST  'W'   // -

#define FEET   'F'   // Altitude in Feet

static double MagToDeg(double dMag)
{
	int nDeg = (int)(dMag / 100);
	return ((dMag - (nDeg * 100)) / 60.) + nDeg;
}

static int ParseRoute(const stdx::string_helper::vector& vecResults, CGpsRoute& cGpsRoute, const CGpsPointArray& cGpsPointArray, int& nExplorist)
{
	CGpsPointArray::const_iterator it;

	if (vecResults.size() > 5)
	{
		if (stdx::string_helper::string_to<int>(vecResults[2]) == 1)
			nExplorist = vecResults.size() % 2;

		if (vecResults[3][0] != 'c')
			return S_OK;

		if (nExplorist && cGpsRoute.name().empty())
			cGpsRoute.name(vecResults[5]);

		for (size_t i = 5 + nExplorist; i < (vecResults.size() - 1); i++)
		{
			if ((i - nExplorist) % 2)
			{
				for (it = cGpsPointArray.begin(); it != cGpsPointArray.end(); ++it)
				{
					if (!vecResults[i].compare(it->comment()))
					{
						cGpsRoute.push_back(*it);
						break;
					}
				}
			}
		}
	}

	return S_OK;
}

static int ParseWaypoint(const stdx::string_helper::vector& vecResults, CGpsPointArray& cGpsPointArray)
{
	if (vecResults.size() > 9)
	{
		CGpsPoint cGpsPoint;

		double dLatitude = MagToDeg(stdx::string_helper::string_to<double>(vecResults[1]));
		if (vecResults[2][0] == SOUTH)
			dLatitude = -dLatitude;

		double dLongitude = MagToDeg(stdx::string_helper::string_to<double>(vecResults[3]));
		if (vecResults[4][0] == WEAST)
			dLongitude = -dLongitude;

		double dAltitude = stdx::string_helper::string_to<double>(vecResults[5]);
		if (vecResults[6][0] == FEET)
			dAltitude = FEET2MTR(dAltitude);

		cGpsPoint.coords(dLatitude, dLongitude, dAltitude);

		cGpsPoint.comment(vecResults[7]);
		if (vecResults[8].empty())
			cGpsPoint.name(vecResults[7]);
		else
			cGpsPoint.name(vecResults[8]);

		cGpsPointArray.push_back(cGpsPoint);
	}

	return S_OK;
}

int ReadMAG(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	CGpsWaypointArray cGpsPointArray;
	int nExplorist = 0;

	std::ifstream ifsFile(strPathName.c_str());
	if (!ifsFile)
		return S_FALSE;

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	std::string strLine;
	while (std::getline(ifsFile, strLine))
	{
		stdx::string_helper::vector vecResults;
		stdx::string_helper::split(strLine, vecResults, stdx::find_first_of(",*"));

		if (vecResults.size() > 0)
		{
			if (!vecResults.front().compare(PMGNWPL))
				ParseWaypoint(vecResults, cGpsPointArray);

			if (!vecResults.front().compare(PMGNRTE))
				ParseRoute(vecResults, cGpsRoute, cGpsPointArray, nExplorist);
		}
	}

	if (cGpsRoute.empty())
		cGpsRoute = cGpsPointArray; // No route, keep waypoints

	return S_OK;
}
