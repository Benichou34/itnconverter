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
 * Purpose : Reader for PCX5 file (.rte)
 *           www.bayo.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <fstream>

typedef struct _OFFSET
{
	DWORD dwOffset;
	DWORD dwLenght;
} sOffset;

#define TYPE_IDNT          0
#define TYPE_LATITUDE      1
#define TYPE_LONGITUDE     2
#define TYPE_DATE          3
#define TYPE_TIME          4
#define TYPE_ALT           5
#define TYPE_DESCRIPTION   6
#define TYPE_PROXIMITY     7
#define TYPE_SYMBOL        8
#define TYPE_NUMBER        9

#define TRK_TYPE_HEADER    'H' // Header
#define TRK_TYPE_INFO      'I' // Software identification
#define TRK_TYPE_MAP       'M' // Map datum
#define TRK_TYPE_COORD     'U' // Coordinate system
#define TRK_TYPE_TRACK     'T' // Track
#define TRK_TYPE_ROAD      'R' // Route
#define TRK_TYPE_WAYPT     'W' // Waypoint

#define TRK_HDR_SOFT       "SOFTWARE NAME & VERSION"
#define TRK_HDR_COORD      "COORDINATE SYSTEM"

#define TRK_COORD_LL       "LAT LON"

#define TRK_UNIT_DEG       "DEG" // DEG format (dddd.ddddd)  deg.deg
#define TRK_UNIT_DMS       "DMS" // DMS format (DDDMMss.ss)  deg min sec. sec
#define TRK_UNIT_DM        "DM"  // DMM format (DDDmm.mmmm) deg min.min

#define UNIT_DEG           0     // DEG
#define UNIT_DMS           1     // DMS
#define UNIT_DM            2     // DMM

#define TRK_NORTH          'N'   // +
#define TRK_SOUTH          'S'   // -
#define TRK_EAST           'E'   // +
#define TRK_WEAST          'W'   // -

#define TRK_POS            '+'   // +
#define TRK_NEG            '-'   // -

#define TRK_SPACE          ' '
#define TRK_NULL_ALT       -9999

static char* szMonth[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC", nullptr };
static char* szUnit[] = { TRK_UNIT_DEG, TRK_UNIT_DMS, TRK_UNIT_DM, nullptr };

static char* szHeader[TYPE_NUMBER] = { "IDNT", "LATITUDE", "LONGITUDE", "DATE", "TIME", "ALT", "DESCRIPTION", "PROXIMITY", "SYMBOL" };

#define DM2DEG(D, m) (D + (m/60))
#define DMS2DEG(D, m, s) (D + (m/60) + (s/3600))

int ParseRoute(std::string& strLine, CGpsPointArray& cGpsPointArray)
{
	char* pPos = const_cast<char*>(strLine.data()) + 1;

	while (*pPos == TRK_SPACE)
		pPos++;

	pPos += 2;

	while (*pPos == TRK_SPACE)
		pPos++;

	std::string str = pPos;
	stdx::string_helper::trimleft(str);
	stdx::string_helper::trimright(str);

	cGpsPointArray.name(stdx::string_helper::to_utf8(str));

	return S_OK;
}

int ParseUnit(std::string& strLine, char* pnUnit)
{
	char* pPos = const_cast<char*>(strLine.data()) + 1;
	char i = 0;

	pPos = strstr(pPos, TRK_COORD_LL);
	if (!pPos)
		return S_FALSE;

	pPos += strlen(TRK_COORD_LL);

	while (*pPos == TRK_SPACE)
		pPos++;

	while (szUnit[i])
	{
		if (!_strnicmp(szUnit[i], pPos, strlen(szUnit[i])))
		{
			*pnUnit = i;
			return S_OK;
		}

		i++;
	}

	return S_FALSE;
}

void ParseHeader(const std::string& strLine, sOffset* pTabOffset)
{
	for (int i = 0; i < TYPE_NUMBER; i++)
	{
		size_t pos = strLine.find(szHeader[i]);
		if (pos == std::string::npos)
			break;
		pTabOffset[i].dwOffset = pos;
	}

	for (int i = 0; i < TYPE_NUMBER - 1; i++)
	{
		if (pTabOffset[i].dwOffset)
			pTabOffset[i].dwLenght = pTabOffset[i + 1].dwOffset - pTabOffset[i].dwOffset - 1;
	}
}

int ParseLineLL(std::string& strLine, CGpsPointArray& cGpsPointArray, sOffset* pTabOffset, char nUnit)
{
	char* pPos = const_cast<char*>(strLine.data()) + 1;
	BOOL bPositive = TRUE;
	float fLatitude, fLongitude, fAltitude;
	int nTmpInt1;
	int nTmpInt2;
	float fTmpFloat;

	CGpsPoint cGpsPoint;

	// Read LATITUDE
	pPos = const_cast<char*>(strLine.data()) + pTabOffset[TYPE_LATITUDE].dwOffset;
	*(pPos + pTabOffset[TYPE_LATITUDE].dwLenght) = 0x00;

	bPositive = TRUE;

	switch (*pPos)
	{
	case TRK_POS:
	case TRK_NORTH:
		bPositive = TRUE;
		pPos++;
		break;
	case TRK_NEG:
	case TRK_SOUTH:
		bPositive = FALSE;
		pPos++;
		break;
	default:
		break;
	}

	switch (nUnit)
	{
	case UNIT_DMS:
		sscanf(pPos, "%2d%2d%f", &nTmpInt1, &nTmpInt2, &fTmpFloat);
		fLatitude = DMS2DEG(nTmpInt1, nTmpInt2, fTmpFloat);
		break;

	case UNIT_DM:
		sscanf(pPos, "%2d%f", &nTmpInt1, &fTmpFloat);
		fLatitude = DM2DEG(nTmpInt1, fTmpFloat);
		break;

	case UNIT_DEG:
	default:
		fLatitude = (float)atof(pPos);
		break;
	}

	if (!bPositive)
		fLatitude = -fLatitude;

	cGpsPoint.lat(fLatitude);

	// Read LONGITUDE
	pPos = const_cast<char*>(strLine.data()) + pTabOffset[TYPE_LONGITUDE].dwOffset;
	*(pPos + pTabOffset[TYPE_LONGITUDE].dwLenght) = 0x00;

	bPositive = TRUE;

	switch (*pPos)
	{
	case TRK_POS:
	case TRK_EAST:
		bPositive = TRUE;
		pPos++;
		break;
	case TRK_NEG:
	case TRK_WEAST:
		bPositive = FALSE;
		pPos++;
		break;
	default:
		break;
	}

	switch (nUnit)
	{
	case UNIT_DMS:
		sscanf(pPos, "%3d%2d%f", &nTmpInt1, &nTmpInt2, &fTmpFloat);
		fLongitude = DMS2DEG(nTmpInt1, nTmpInt2, fTmpFloat);
		break;

	case UNIT_DM:
		sscanf(pPos, "%3d%f", &nTmpInt1, &fTmpFloat);
		fLongitude = DM2DEG(nTmpInt1, fTmpFloat);
		break;

	case UNIT_DEG:
	default:
		fLongitude = (float)atof(pPos);
		break;
	}

	if (!bPositive)
		fLongitude = -fLongitude;

	cGpsPoint.lng(fLongitude);

	// Read altitude
	fAltitude = (float)atof(strLine.data() + pTabOffset[TYPE_ALT].dwOffset);
	if (fAltitude == TRK_NULL_ALT)
		fAltitude = 0;

	cGpsPoint.alt(fAltitude);

	// Read description
	std::string str(strLine.data() + pTabOffset[TYPE_DESCRIPTION].dwOffset, pTabOffset[TYPE_DESCRIPTION].dwLenght);;
	stdx::string_helper::trimleft(str);
	stdx::string_helper::trimright(str);

	if (str.empty())
	{
		str.assign(strLine.data() + pTabOffset[TYPE_IDNT].dwOffset, pTabOffset[TYPE_IDNT].dwLenght);
		stdx::string_helper::trimleft(str);
		stdx::string_helper::trimright(str);
	}

	cGpsPoint.name(stdx::string_helper::to_utf8(str));
	cGpsPointArray.push_back(cGpsPoint);

	return S_OK;
}

int ReadRTE(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	HRESULT hr = S_FALSE;
	char nUnit = UNIT_DEG;
	sOffset sTabOffset[TYPE_NUMBER];

	memset(sTabOffset, 0x00, sizeof(sTabOffset));

	std::ifstream ifsFile(strPathName.c_str());
	if (!ifsFile)
		return S_FALSE;

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	std::string strLine;
	while (std::getline(ifsFile, strLine))
	{
		switch (strLine[0])
		{
		case TRK_TYPE_HEADER:
			// Check header
			if (strLine.find(TRK_HDR_SOFT) != std::string::npos)
				hr = S_OK;
			else if (hr == S_OK)
				ParseHeader(strLine, sTabOffset);
			break;

		case TRK_TYPE_INFO:
			// Ignore software information
			break;

		case TRK_TYPE_MAP:
			// Ignore Map datum
			break;

		case TRK_TYPE_COORD:
			// Check coordinates
			if (hr == S_OK && ParseUnit(strLine, &nUnit) == S_FALSE)
				return S_FALSE;
			break;

		case TRK_TYPE_ROAD:
			// Read road name
			if (hr == S_OK)
				ParseRoute(strLine, cGpsRoute);
			break;

		case TRK_TYPE_WAYPT:
			// Read track
			if (hr == S_OK)
				ParseLineLL(strLine, cGpsRoute, sTabOffset, nUnit);
			break;

		default:
			break;
		}
	}

	return hr;
}
