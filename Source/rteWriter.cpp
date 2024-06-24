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
 * Purpose : Writer for PCX5 file (.rte)
 *           www.bayo.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <fstream>

#define RTE_NAME     "H  SOFTWARE NAME & VERSION\r\nI  PCX5 2.09 by " SOFT_FULL_NAME " (" SOFT_URL ")\r\n"
#define RTE_HEAD1    "\r\nH  R DATUM                IDX DA            DF            DX            DY            DZ\r\n"
#define RTE_HEAD2    "M  G WGS 84               121 +0.000000e+00 +0.000000e+00 +0.000000e+00 +0.000000e+00 +0.000000e+00\r\n"
#define RTE_HEAD3    "\r\nH  COORDINATE SYSTEM\r\nU  LAT LON DEG\r\n"
#define RTE_INDEX    "\r\nR  01 %s\r\n"
#define RTE_HEADER   "\r\nH  IDNT     LATITUDE    LONGITUDE    DATE      TIME     ALT   DESCRIPTION                              PROXIMITY     SYMBOL ;waypts\r\n"
#define RTE_RECORD   "W  WPT%05d %s %s %s %s %s %s 0.00000e+000  00018\r\n"
#define CRD_LAT      "%+011.7f"
#define CRD_LONG     "%+012.7f"
#define CRD_ALT      "%05.f"

#define NORTH  'N'   // +
#define SOUTH  'S'   // -
#define EAST   'E'   // +
#define WEAST  'W'   // -

#define DEF_ALT   "-9999" // Default altitude value

#define MAX_DESC_LEN 40

static char* szMonth[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC", nullptr };

int WriteRTE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::string strDate;
	std::string strTime;
	char szLatitude[12];
	char szLongitude[13];
	char szAltitude[6];
	SYSTEMTIME sSystemTime;

	GetSystemTime(&sSystemTime);

	strDate = stdx::format("%02d-%s-%02d")
		(sSystemTime.wDay)
		(szMonth[sSystemTime.wMonth - 1])
		(sSystemTime.wYear % 100);

	strTime = stdx::format("%02d:%02d:%02d")
		(sSystemTime.wHour)
		(sSystemTime.wMinute)
		(sSystemTime.wSecond);

	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	ofsFile << RTE_NAME << RTE_HEAD1 << RTE_HEAD2 << RTE_HEAD3 << stdx::format(RTE_INDEX)(stdx::string_helper::from_utf8(cGpsRoute.name())) << RTE_HEADER;

	for (size_t i = 0; i < cGpsRoute.size(); i++)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];

		sprintf(szLatitude, CRD_LAT, cGpsPoint.lat());
		if (szLatitude[0] == '-')
			szLatitude[0] = SOUTH;
		else
			szLatitude[0] = NORTH;

		sprintf(szLongitude, CRD_LONG, cGpsPoint.lng());
		if (szLongitude[0] == '-')
			szLongitude[0] = WEAST;
		else
			szLongitude[0] = EAST;

		if (cGpsPoint.alt() != 0)
			sprintf(szAltitude, CRD_ALT, cGpsPoint.alt());
		else
			strcpy(szAltitude, DEF_ALT);

		std::string strAddress = stdx::string_helper::from_utf8(cGpsPoint.name());
		strAddress.resize(MAX_DESC_LEN, 0x20);

		ofsFile << stdx::format(RTE_RECORD)(i + 1)(szLatitude)(szLongitude)(strDate)(strTime)(szAltitude)(strAddress);
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}