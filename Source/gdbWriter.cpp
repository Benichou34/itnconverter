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
 * Purpose : Writer for Garmin Map Source 6.x file (.gdb)
 *           www.garmin.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "stdx/string_helper.h"

#pragma pack(push, 1) // packing is now 1
typedef struct
{
	DWORD dwLength;   // Record length (without this header)
	BYTE  bType;      // Record type
} _RECORD_HEADER, * P_RECORD_HEADER;

typedef struct
{
	// Coords in (180/2^31) degrees
	int nLatitude;
	int nLongitude;
	BYTE bAltitude;
} _GPS_POINT, * P_GPS_POINT;

#pragma pack(pop) // packing is 8

#define GDB_SIZE_OF_SIGNATURE    6
#define GDB_DEFAULT_SIGNATURE    "MsRcf\0"

#define GDB_TYPE_FILE_FORMAT     'D'
#define GDB_TYPE_FILE_VERSION    'A'
#define GDB_TYPE_WAYPOINT        'W'
#define GDB_TYPE_TRACK           'T'
#define GDB_TYPE_ROUTE           'R'
#define GDB_TYPE_MAPSEGMENT      'L'
#define GDB_TYPE_MAPSET          'V'

#define GDB_SIZE_OF_FORMAT       2
#define GDB_DEFAULT_FORMAT       "l\0"

#define GDB_DEFAULT_VERSION      "]\002" SOFT_FULL_NAME "\0" __DATE__"\0" __TIME__ "\0MapSource\0"
#define GDB_SIZE_OF_VERSION      strlen(GDB_DEFAULT_VERSION) + 22

#define GDB_DEFAULT_WPT_CLASS    8
#define GDB_SIZE_OF_WPT_SUBCLASS 23 // + empty country
static unsigned char GDB_DEFAULT_WPT_SUBCLASS[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

#define GDB_SIZE_OF_WPT_END      31
static unsigned char GDB_DEFAULT_WPT_END[] = { 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define GDB_SIZE_OF_UNKNOWN      19
static unsigned char GDB_DEFAULT_UNKNOWN[] = { 0x00, 0x03, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define GDB_SIZE_OF_ROUTE_END    8
static unsigned char GDB_DEFAULT_ROUTE_END[] = { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };

#define GDB_SIZE_OF_MAPSET       2
#define GDB_DEFAULT_MAPSET       "\0\001"

#define DEGREE_COORD_TO_GDB(x) (int)((x*0x80000000UL)/180)

static int WriteRouteHeader(const CGpsRoute& cGpsRoute, HANDLE pFile)
{
	_RECORD_HEADER sRecordHeader;
	_GPS_POINT sGpsPoint;
	DWORD dwNumberOfBytesWritten = 0;
	int nWayPointNumber = cGpsRoute.size();
	double fLatitude, fMinLatitude, fMaxLatitude;
	double fLongitude, fMinLongitude, fMaxLongitude;
	double fAltitude, fMinAltitude, fMaxAltitude;
	short sFlag = 0;
	int i;

	std::string strRouteName = stdx::string_helper::from_utf8(cGpsRoute.name());

	sRecordHeader.dwLength = strRouteName.size() + 1 + // Route name
		sizeof(short) + // Flags
		((sizeof(_GPS_POINT) + sizeof(double)) * 2) + // min/max coordinates
		sizeof(int) + // Point number
		sizeof(BYTE);// Url
	sRecordHeader.bType = GDB_TYPE_ROUTE;

	/* Search min and max coordinates */
	if (nWayPointNumber)
	{
		const CGpsPoint& cGpsPoint0 = cGpsRoute[0];

		sRecordHeader.dwLength += stdx::string_helper::from_utf8(cGpsPoint0.name()).size() + 1 + // waypoint name
			sizeof(int) + // class
			GDB_SIZE_OF_WPT_SUBCLASS + // country + subclass
			GDB_SIZE_OF_UNKNOWN + // unkown data
			sizeof(int) + // Link number
			sizeof(BYTE) + // End flag
			GDB_SIZE_OF_ROUTE_END;
		fMinLatitude = fMaxLatitude = cGpsPoint0.lat();
		fMinLongitude = fMaxLongitude = cGpsPoint0.lng();
		fMinAltitude = fMaxAltitude = cGpsPoint0.alt();

		for (i = 1; i < nWayPointNumber; i++)
		{
			const CGpsPoint& cGpsPoint = cGpsRoute[i];

			sRecordHeader.dwLength += stdx::string_helper::from_utf8(cGpsPoint.name()).size() + 1 + // waypoint name
				sizeof(int) + // class
				GDB_SIZE_OF_WPT_SUBCLASS + // country + subclass
				GDB_SIZE_OF_UNKNOWN + // unkown data
				sizeof(int) + // Link number
				(sizeof(_GPS_POINT) * 2) + // Linked waypoints
				((sizeof(_GPS_POINT) + sizeof(double)) * 2) + // min/max waypoints
				sizeof(BYTE) + // End flag
				GDB_SIZE_OF_ROUTE_END;

			fLatitude = cGpsPoint.lat();
			fLongitude = cGpsPoint.lng();
			fAltitude = cGpsPoint.alt();
			if (fAltitude)
				sRecordHeader.dwLength += sizeof(double) * 2;

			if (fLatitude < fMinLatitude)
				fMinLatitude = fLatitude;

			if (fLatitude > fMaxLatitude)
				fMaxLatitude = fLatitude;

			if (fLongitude < fMinLongitude)
				fMinLongitude = fLongitude;

			if (fLongitude > fMaxLongitude)
				fMaxLongitude = fLongitude;

			if (fAltitude < fMinAltitude)
				fMinAltitude = fAltitude;

			if (fAltitude > fMaxAltitude)
				fMaxAltitude = fAltitude;
		}

		WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, strRouteName.c_str(), strRouteName.size() + 1, &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &sFlag, sizeof(short), &dwNumberOfBytesWritten, nullptr);

		sGpsPoint.nLatitude = DEGREE_COORD_TO_GDB(fMaxLatitude);
		sGpsPoint.nLongitude = DEGREE_COORD_TO_GDB(fMaxLongitude);
		sGpsPoint.bAltitude = (BYTE)0x01;
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &fMaxAltitude, sizeof(double), &dwNumberOfBytesWritten, nullptr);

		sGpsPoint.nLatitude = DEGREE_COORD_TO_GDB(fMinLatitude);
		sGpsPoint.nLongitude = DEGREE_COORD_TO_GDB(fMinLongitude);
		sGpsPoint.bAltitude = (BYTE)0x01;
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &fMinAltitude, sizeof(double), &dwNumberOfBytesWritten, nullptr);

		WriteFile(pFile, &nWayPointNumber, sizeof(int), &dwNumberOfBytesWritten, nullptr);
	}

	return S_OK;
}

static int WriteRoute(const CGpsRoute& cGpsRoute, size_t nIndex, HANDLE pFile)
{
	_GPS_POINT sGpsPoint;
	DWORD dwNumberOfBytesWritten = 0;
	int nClass = 0;
	int nLink = 2;
	BYTE bFlag = 0;
	int nMinLatitude = 0, nMaxLatitude = 0;
	int nMinLongitude = 0, nMaxLongitude = 0;
	double fAltitude = 0, fMinAltitude = 0, fMaxAltitude = 0;

	if (nIndex == cGpsRoute.upper_bound())
	{
		nLink = 0;
		bFlag = 1;
	}

	const CGpsPoint& cGpsPoint = cGpsRoute[nIndex];
	std::string strPointName = stdx::string_helper::from_utf8(cGpsPoint.name());

	WriteFile(pFile, strPointName.c_str(), strPointName.size() + 1, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, &nClass, sizeof(int), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, GDB_DEFAULT_WPT_SUBCLASS, GDB_SIZE_OF_WPT_SUBCLASS, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, GDB_DEFAULT_UNKNOWN, GDB_SIZE_OF_UNKNOWN, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, &nLink, sizeof(int), &dwNumberOfBytesWritten, nullptr);

	if (nLink)
	{
		// Point 1
		const geo::CGeoLatLng& gLatLng1 = cGpsRoute[nIndex];
		nMinLatitude = nMaxLatitude = sGpsPoint.nLatitude = DEGREE_COORD_TO_GDB(gLatLng1.lat());
		nMinLongitude = nMaxLongitude = sGpsPoint.nLongitude = DEGREE_COORD_TO_GDB(gLatLng1.lng());
		fMinAltitude = fMaxAltitude = fAltitude = gLatLng1.alt();
		sGpsPoint.bAltitude = (BYTE)((fAltitude) ? 0x01 : 0x00);
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
		if (sGpsPoint.bAltitude)
			WriteFile(pFile, &fAltitude, sizeof(double), &dwNumberOfBytesWritten, nullptr);

		// Point 2
		const geo::CGeoLatLng& gLatLng2 = cGpsRoute[nIndex + 1];
		sGpsPoint.nLatitude = DEGREE_COORD_TO_GDB(gLatLng2.lat());
		sGpsPoint.nLongitude = DEGREE_COORD_TO_GDB(gLatLng2.lng());
		fAltitude = gLatLng2.alt();
		sGpsPoint.bAltitude = (BYTE)((fAltitude) ? 0x01 : 0x00);
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
		if (sGpsPoint.bAltitude)
			WriteFile(pFile, &fAltitude, sizeof(double), &dwNumberOfBytesWritten, nullptr);

		if (sGpsPoint.nLatitude < nMinLatitude)
			nMinLatitude = sGpsPoint.nLatitude;

		if (sGpsPoint.nLatitude > nMaxLatitude)
			nMaxLatitude = sGpsPoint.nLatitude;

		if (sGpsPoint.nLongitude < nMinLongitude)
			nMinLongitude = sGpsPoint.nLongitude;

		if (sGpsPoint.nLongitude > nMaxLongitude)
			nMaxLongitude = sGpsPoint.nLongitude;

		if (fAltitude < fMinAltitude)
			fMinAltitude = fAltitude;

		if (fAltitude > fMaxAltitude)
			fMaxAltitude = fAltitude;
	}

	WriteFile(pFile, &bFlag, sizeof(BYTE), &dwNumberOfBytesWritten, nullptr);

	if (!bFlag)
	{
		/*
		if(!fMinAltitude)
		fMinAltitude = -99999999;

		if(!fMaxAltitude)
		fMaxAltitude = 99999999;
		*/
		// Max
		sGpsPoint.nLatitude = nMaxLatitude;
		sGpsPoint.nLongitude = nMaxLongitude;
		sGpsPoint.bAltitude = (BYTE)0x01;
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &fMaxAltitude, sizeof(double), &dwNumberOfBytesWritten, nullptr);

		// Min
		sGpsPoint.nLatitude = nMinLatitude;
		sGpsPoint.nLongitude = nMinLongitude;
		sGpsPoint.bAltitude = (BYTE)0x01;
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &fMinAltitude, sizeof(double), &dwNumberOfBytesWritten, nullptr);
	}

	WriteFile(pFile, GDB_DEFAULT_ROUTE_END, GDB_SIZE_OF_ROUTE_END, &dwNumberOfBytesWritten, nullptr);

	return S_OK;
}

static int WriteWayPoint(const CGpsPointArray& cGpsPointArray, size_t nIndex, HANDLE pFile)
{
	_RECORD_HEADER sRecordHeader;
	_GPS_POINT sGpsPoint;
	DWORD dwNumberOfBytesWritten = 0;
	int nClass = GDB_DEFAULT_WPT_CLASS;
	double fAltitude = 0;

	const CGpsPoint& cGpsPoint = cGpsPointArray[nIndex];
	std::string strPointName = stdx::string_helper::from_utf8(cGpsPoint.name());

	sRecordHeader.dwLength = strPointName.size() + 1 + sizeof(int) + GDB_SIZE_OF_WPT_SUBCLASS + sizeof(_GPS_POINT) + GDB_SIZE_OF_WPT_END;
	sRecordHeader.bType = GDB_TYPE_WAYPOINT;

	sGpsPoint.nLatitude = DEGREE_COORD_TO_GDB(cGpsPoint.lat());
	sGpsPoint.nLongitude = DEGREE_COORD_TO_GDB(cGpsPoint.lng());
	fAltitude = cGpsPoint.alt();
	sGpsPoint.bAltitude = (BYTE)((fAltitude) ? 0x01 : 0x00);
	sRecordHeader.dwLength += sGpsPoint.bAltitude * sizeof(double);

	WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, strPointName.c_str(), strPointName.size() + 1, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, &nClass, sizeof(int), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, GDB_DEFAULT_WPT_SUBCLASS, GDB_SIZE_OF_WPT_SUBCLASS, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
	if (sGpsPoint.bAltitude)
		WriteFile(pFile, &fAltitude, sizeof(double), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, GDB_DEFAULT_WPT_END, GDB_SIZE_OF_WPT_END, &dwNumberOfBytesWritten, nullptr);

	return S_OK;
}

int WriteGDB(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	HANDLE pFile = nullptr;
	DWORD dwNumberOfBytesWritten = 0;
	_RECORD_HEADER sRecordHeader;
	CGpsWaypointArray cGpsWaypoints;

	pFile = CreateFile(strPathName.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr);

	if (!pFile)
		return GetLastError();

	/* Write header and file format */
	WriteFile(pFile, GDB_DEFAULT_SIGNATURE, GDB_SIZE_OF_SIGNATURE, &dwNumberOfBytesWritten, nullptr);

	sRecordHeader.dwLength = GDB_SIZE_OF_FORMAT;
	sRecordHeader.bType = GDB_TYPE_FILE_FORMAT;
	WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, GDB_DEFAULT_FORMAT, GDB_SIZE_OF_FORMAT, &dwNumberOfBytesWritten, nullptr);

	/* Write file version */
	sRecordHeader.dwLength = GDB_SIZE_OF_VERSION;
	sRecordHeader.bType = GDB_TYPE_FILE_VERSION;
	WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, GDB_DEFAULT_VERSION, GDB_SIZE_OF_VERSION + 10, &dwNumberOfBytesWritten, nullptr);

	cGpsWaypoints = cGpsRoute;
	// remove duplicated points
	cGpsWaypoints.removeDuplicates();

	for (size_t i = 0; i < cGpsWaypoints.size(); i++)
		WriteWayPoint(cGpsWaypoints, i, pFile);

	WriteRouteHeader(cGpsRoute, pFile);
	for (size_t i = 0; i < cGpsRoute.size(); i++)
		WriteRoute(cGpsRoute, i, pFile);

	WriteFile(pFile, "\0", sizeof(BYTE), &dwNumberOfBytesWritten, nullptr); // Url

	/* Write Mapset */
	sRecordHeader.dwLength = GDB_SIZE_OF_MAPSET;
	sRecordHeader.bType = GDB_TYPE_MAPSET;
	WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, GDB_DEFAULT_MAPSET, GDB_SIZE_OF_MAPSET, &dwNumberOfBytesWritten, nullptr);

	CloseHandle(pFile);

	return S_OK;
}

