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
 * Purpose : Writer for Magellan Route file (.rte)
 *           www.magellangps.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <fstream>

#define PMGNWPL      "$PMGNWPL,%08.03f,%c,%09.03f,%c,%07.f,M,WPT%03d,%s,a"
#define PMGNRTE_1    "$PMGNRTE,%d,%d,c,1,WPT%03d,a"
#define PMGNRTE_2    "$PMGNRTE,%d,%d,c,1,WPT%03d,a,WPT%03d,a"
#define PMGNRTE_EX_1 "$PMGNRTE,%d,%d,c,1,%s,WPT%03d,a"
#define PMGNRTE_EX_2 "$PMGNRTE,%d,%d,c,1,%s,WPT%03d,a,WPT%03d,a"
#define CHECKSUM     "%s*%02X\r\n"

#define NORTH  'N'   // +
#define SOUTH  'S'   // -
#define EAST   'E'   // +
#define WEAST  'W'   // -

namespace
{
	double DegToMag(double dDeg)
	{
		double dMag = ((dDeg - static_cast<int>(dDeg)) * 60) + (static_cast<int>(dDeg) * 100);
		return fabs(dMag);
	}

	unsigned char getChecksum(const std::string& strBuffer)
	{
		unsigned char ucChecksum = 0;

		for (std::string::const_iterator cit = strBuffer.begin() + 1; cit != strBuffer.end(); ++cit)
			ucChecksum ^= *cit;

		return ucChecksum & 0xFF;
	}
}

int WriteMAG(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	// Write Waypoints
	for (size_t i = 0; i < cGpsRoute.size(); i++)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];

		// Remove commas
		std::string strTmpText = stdx::string_helper::from_utf8(cGpsPoint.name());
		stdx::string_helper::remove(strTmpText, ',');

		std::string strline = stdx::format(PMGNWPL)
			(DegToMag(cGpsPoint.lat()))
			((cGpsPoint.lat() < 0) ? SOUTH : NORTH)
			(DegToMag(cGpsPoint.lng()))
			((cGpsPoint.lng() < 0) ? WEAST : EAST)
			(cGpsPoint.alt())
			(i + 1)
			(strTmpText);

		ofsFile << stdx::format(CHECKSUM)(strline)(getChecksum(strline));
	}

	size_t nRouteNumber = (cGpsRoute.size() / 2) + (cGpsRoute.size() % 2);

	std::string strTmpText = stdx::string_helper::narrow(CWToolsString::FileTitle(strPathName));
	if (dwFlag)
	{
		// eXplorist format
		// Remove commas
		stdx::string_helper::remove(strTmpText, ',');
	}

	// Write Route
	for (size_t i = 0; i < nRouteNumber; i++)
	{
		std::string strline;

		if (i + 1 == nRouteNumber && cGpsRoute.size() % 2)
		{
			if (dwFlag)
				// eXplorist format
				strline = stdx::format(PMGNRTE_EX_1)(nRouteNumber)(i + 1)(strTmpText)((i * 2) + 1);
			else
				strline = stdx::format(PMGNRTE_1)(nRouteNumber)(i + 1)((i * 2) + 1);
		}
		else
		{
			if (dwFlag)
				// eXplorist format
				strline = stdx::format(PMGNRTE_EX_2)(nRouteNumber)(i + 1)(strTmpText)((i * 2) + 1)((i * 2) + 2);
			else
				strline = stdx::format(PMGNRTE_2)(nRouteNumber)(i + 1)((i * 2) + 1)((i * 2) + 2);
		}

		ofsFile << stdx::format(CHECKSUM)(strline)(getChecksum(strline));
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
