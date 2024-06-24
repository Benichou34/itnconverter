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
 * Purpose : Writer for Garmin Map Source 5.x file (.mps)
 *           www.garmin.com
 */

#include "stdafx.h"
#include "ITN Tools.h"

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
	double fAltitude;
} _GPS_POINT, * P_GPS_POINT;

#pragma pack(pop) // packing is 8

#define MPS_SIZE_OF_SIGNATURE    6
#define MPS_DEFAULT_SIGNATURE    "MsRcd\0"

#define MPS_SIZE_OF_FORMAT       2
#define MPS_DEFAULT_FORMAT       "i\0"

#define MPS_SIZE_OF_VERSION      27
static unsigned char MPS_DEFAULT_VERSION[] = { 0xF4, 0x01, 0x53, 0x51, 0x41, 0x00, 0x4A, 0x75, 0x6C, 0x20, 0x20, 0x33, 0x20, 0x32, 0x30, 0x30, 0x33, 0x00, 0x30, 0x38, 0x3A, 0x33, 0x35, 0x3A, 0x33, 0x33, 0x00 };

#define MPS_DEFAULT_WPT_CLASS    8
#define MPS_SIZE_OF_WPT_SUBCLASS 23 // + empty country
static unsigned char MPS_DEFAULT_WPT_SUBCLASS[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

#define MPS_SIZE_OF_WPT_END      42
static unsigned char MPS_DEFAULT_WPT_END[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define MPS_SIZE_OF_UNKNOWN      19
static unsigned char MPS_DEFAULT_UNKNOWN[] = { 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#define MPS_SIZE_OF_MAPSET       2
#define MPS_DEFAULT_MAPSET       "\0\001"

#define MPS_TYPE_FILE_FORMAT     'D'
#define MPS_TYPE_FILE_VERSION    'A'
#define MPS_TYPE_WAYPOINT        'W'
#define MPS_TYPE_TRACK           'T'
#define MPS_TYPE_ROUTE           'R'
#define MPS_TYPE_MAPSEGMENT      'L'
#define MPS_TYPE_MAPSET          'V'

#define DEGREE_COORD_TO_MPS(x) ((int)((x*0x80000000UL)/180.))

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

	sRecordHeader.dwLength = stdx::string_helper::from_utf8(cGpsRoute.name()).size() + 1 + // Route name
		sizeof(short) + // Flags
		(sizeof(_GPS_POINT) * 2) + // min/max coordinates
		sizeof(int); // Point number
	sRecordHeader.bType = MPS_TYPE_ROUTE;

	/* Search min and max coordinates */
	if (nWayPointNumber)
	{
		const CGpsPoint& cGpsPoint0 = cGpsRoute[0];

		sRecordHeader.dwLength += stdx::string_helper::from_utf8(cGpsPoint0.name()).size() + 1 + // waypoint name
			sizeof(int) + // class
			MPS_SIZE_OF_WPT_SUBCLASS + // country + subclass
			MPS_SIZE_OF_UNKNOWN + // unkown data
			sizeof(int) + // Link number
			sizeof(BYTE); // End flag
		fMinLatitude = fMaxLatitude = cGpsPoint0.lat();
		fMinLongitude = fMaxLongitude = cGpsPoint0.lng();
		fMinAltitude = fMaxAltitude = cGpsPoint0.alt();

		for (i = 1; i < nWayPointNumber; i++)
		{
			const CGpsPoint& cGpsPoint = cGpsRoute[i];

			sRecordHeader.dwLength += stdx::string_helper::from_utf8(cGpsPoint.name()).size() + 1 + // waypoint name
				sizeof(int) + // class
				MPS_SIZE_OF_WPT_SUBCLASS + // country + subclass
				MPS_SIZE_OF_UNKNOWN + // unkown data
				sizeof(int) + // Link number
				(sizeof(_GPS_POINT) * 2) + // Linked waypoints
				(sizeof(_GPS_POINT) * 2) + // min/max waypoints
				sizeof(BYTE); // End flag

			fLatitude = cGpsPoint.lat();
			fLongitude = cGpsPoint.lng();
			fAltitude = cGpsPoint.alt();

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
		WriteFile(pFile, stdx::string_helper::from_utf8(cGpsRoute.name()).c_str(), stdx::string_helper::from_utf8(cGpsRoute.name()).size() + 1, &dwNumberOfBytesWritten, nullptr);
		WriteFile(pFile, &sFlag, sizeof(short), &dwNumberOfBytesWritten, nullptr);

		sGpsPoint.nLatitude = DEGREE_COORD_TO_MPS(fMaxLatitude);
		sGpsPoint.nLongitude = DEGREE_COORD_TO_MPS(fMaxLongitude);
		sGpsPoint.fAltitude = fMaxAltitude;
		sGpsPoint.bAltitude = (BYTE)((fMaxAltitude) ? 0x01 : 0x00);
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);

		sGpsPoint.nLatitude = DEGREE_COORD_TO_MPS(fMinLatitude);
		sGpsPoint.nLongitude = DEGREE_COORD_TO_MPS(fMinLongitude);
		sGpsPoint.fAltitude = fMinAltitude;
		sGpsPoint.bAltitude = (BYTE)((fMinAltitude) ? 0x01 : 0x00);
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);

		WriteFile(pFile, &nWayPointNumber, sizeof(int), &dwNumberOfBytesWritten, nullptr);
	}

	return S_OK;
}

static int WriteRoute(const CGpsRoute& cGpsRoute, size_t nIndex, HANDLE pFile)
{
	_GPS_POINT sGpsPoint;
	DWORD dwNumberOfBytesWritten = 0;
	int nClass = MPS_DEFAULT_WPT_CLASS;
	int nLink = 2;
	BYTE bFlag = 0;
	int nMinLatitude = 0;
	int nMaxLatitude = 0;
	int nMinLongitude = 0;
	int nMaxLongitude = 0;
	double fMinAltitude = 0;
	double fMaxAltitude = 0;

	if (nIndex == cGpsRoute.upper_bound())
	{
		nLink = 0;
		bFlag = 1;
	}

	const CGpsPoint& cGpsPoint = cGpsRoute[nIndex];

	WriteFile(pFile, stdx::string_helper::from_utf8(cGpsPoint.name()).c_str(), stdx::string_helper::from_utf8(cGpsPoint.name()).size() + 1, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, &nClass, sizeof(int), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, MPS_DEFAULT_WPT_SUBCLASS, MPS_SIZE_OF_WPT_SUBCLASS, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, MPS_DEFAULT_UNKNOWN, MPS_SIZE_OF_UNKNOWN, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, &nLink, sizeof(int), &dwNumberOfBytesWritten, nullptr);

	if (nLink)
	{
		// Point 1
		const geo::CGeoLatLng& gLatLng1 = cGpsRoute[nIndex];
		nMinLatitude = nMaxLatitude = sGpsPoint.nLatitude = DEGREE_COORD_TO_MPS(gLatLng1.lat());
		nMinLongitude = nMaxLongitude = sGpsPoint.nLongitude = DEGREE_COORD_TO_MPS(gLatLng1.lng());
		fMinAltitude = fMaxAltitude = sGpsPoint.fAltitude = gLatLng1.alt();
		sGpsPoint.bAltitude = (BYTE)((sGpsPoint.fAltitude) ? 0x01 : 0x00);
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);

		// Point 2
		const geo::CGeoLatLng& gLatLng2 = cGpsRoute[nIndex + 1];
		sGpsPoint.nLatitude = DEGREE_COORD_TO_MPS(gLatLng2.lat());
		sGpsPoint.nLongitude = DEGREE_COORD_TO_MPS(gLatLng2.lng());
		sGpsPoint.fAltitude = gLatLng2.alt();
		sGpsPoint.bAltitude = (BYTE)((sGpsPoint.fAltitude) ? 0x01 : 0x00);
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);

		if (sGpsPoint.nLatitude < nMinLatitude)
			nMinLatitude = sGpsPoint.nLatitude;

		if (sGpsPoint.nLatitude > nMaxLatitude)
			nMaxLatitude = sGpsPoint.nLatitude;

		if (sGpsPoint.nLongitude < nMinLongitude)
			nMinLongitude = sGpsPoint.nLongitude;

		if (sGpsPoint.nLongitude > nMaxLongitude)
			nMaxLongitude = sGpsPoint.nLongitude;

		if (sGpsPoint.fAltitude < fMinAltitude)
			fMinAltitude = sGpsPoint.fAltitude;

		if (sGpsPoint.fAltitude > fMaxAltitude)
			fMaxAltitude = sGpsPoint.fAltitude;
	}

	WriteFile(pFile, &bFlag, sizeof(BYTE), &dwNumberOfBytesWritten, nullptr);

	if (!bFlag)
	{
		// Max
		sGpsPoint.nLatitude = nMaxLatitude;
		sGpsPoint.nLongitude = nMaxLongitude;
		sGpsPoint.fAltitude = fMaxAltitude;
		sGpsPoint.bAltitude = (BYTE)((sGpsPoint.fAltitude) ? 0x01 : 0x00);
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);

		// Min
		sGpsPoint.nLatitude = nMinLatitude;
		sGpsPoint.nLongitude = nMinLongitude;
		sGpsPoint.fAltitude = fMinAltitude;
		sGpsPoint.bAltitude = (BYTE)((sGpsPoint.fAltitude) ? 0x01 : 0x00);
		WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
	}

	return S_OK;
}

static int WriteWayPoint(const CGpsPointArray& cGpsPointArray, size_t nIndex, HANDLE pFile)
{
	_RECORD_HEADER sRecordHeader;
	_GPS_POINT sGpsPoint;
	DWORD dwNumberOfBytesWritten = 0;
	int nClass = MPS_DEFAULT_WPT_CLASS;

	const CGpsPoint& cGpsPoint = cGpsPointArray[nIndex];

	sRecordHeader.dwLength = stdx::string_helper::from_utf8(cGpsPoint.name()).size() + 1 + sizeof(int) + MPS_SIZE_OF_WPT_SUBCLASS + sizeof(_GPS_POINT) + MPS_SIZE_OF_WPT_END;
	sRecordHeader.bType = MPS_TYPE_WAYPOINT;

	sGpsPoint.nLatitude = DEGREE_COORD_TO_MPS(cGpsPoint.lat());
	sGpsPoint.nLongitude = DEGREE_COORD_TO_MPS(cGpsPoint.lng());
	sGpsPoint.fAltitude = cGpsPoint.alt();
	sGpsPoint.bAltitude = (BYTE)((sGpsPoint.fAltitude) ? 0x01 : 0x00);

	WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, stdx::string_helper::from_utf8(cGpsPoint.name()).c_str(), stdx::string_helper::from_utf8(cGpsPoint.name()).size() + 1, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, &nClass, sizeof(int), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, MPS_DEFAULT_WPT_SUBCLASS, MPS_SIZE_OF_WPT_SUBCLASS, &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, &sGpsPoint, sizeof(_GPS_POINT), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, MPS_DEFAULT_WPT_END, MPS_SIZE_OF_WPT_END, &dwNumberOfBytesWritten, nullptr);

	return S_OK;
}

int WriteMPS(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	size_t i;
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
	WriteFile(pFile, MPS_DEFAULT_SIGNATURE, MPS_SIZE_OF_SIGNATURE, &dwNumberOfBytesWritten, nullptr);

	sRecordHeader.dwLength = MPS_SIZE_OF_FORMAT;
	sRecordHeader.bType = MPS_TYPE_FILE_FORMAT;
	WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, MPS_DEFAULT_FORMAT, MPS_SIZE_OF_FORMAT, &dwNumberOfBytesWritten, nullptr);

	sRecordHeader.dwLength = MPS_SIZE_OF_VERSION;
	sRecordHeader.bType = MPS_TYPE_FILE_VERSION;
	WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, MPS_DEFAULT_VERSION, MPS_SIZE_OF_VERSION, &dwNumberOfBytesWritten, nullptr);

	cGpsWaypoints = cGpsRoute;
	// remove duplicated points
	cGpsWaypoints.removeDuplicates();

	for (i = 0; i < cGpsWaypoints.size(); i++)
		WriteWayPoint(cGpsWaypoints, i, pFile);

	WriteRouteHeader(cGpsRoute, pFile);
	for (i = 0; i < cGpsRoute.size(); i++)
		WriteRoute(cGpsRoute, i, pFile);

	/* Write Mapset */
	sRecordHeader.dwLength = MPS_SIZE_OF_MAPSET;
	sRecordHeader.bType = MPS_TYPE_MAPSET;
	WriteFile(pFile, &sRecordHeader, sizeof(_RECORD_HEADER), &dwNumberOfBytesWritten, nullptr);
	WriteFile(pFile, MPS_DEFAULT_MAPSET, MPS_SIZE_OF_MAPSET, &dwNumberOfBytesWritten, nullptr);

	CloseHandle(pFile);

	return S_OK;
}

