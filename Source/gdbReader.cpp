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
 * Purpose : Reader for Garmin Map Source 6.x file (.gdb)
 *           www.garmin.com
 */

#include "stdafx.h"
#include "ITN Tools.h"

#pragma pack(push, 1) // packing is now 1
typedef struct
{
	unsigned int unLength;   // Record length (without this header)
	unsigned char ucType;    // Record type
} _RECORD_HEADER, * P_RECORD_HEADER;

typedef struct
{
	// Coords in (180/2^31) degrees
	int nLatitude;
	int nLongitude;
	unsigned char bAltitude;
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

#define GDB_COORD_TO_DEGREE(x)	static_cast<float>((x*180.)/0x80000000UL)

static int ReadTrack(CGpsPointArray& cGpsTrack, LPBYTE lpTrack, int nVersion)
{
	/* Read Name */
	char* pszName = reinterpret_cast<char*>(lpTrack);
	lpTrack += strlen(pszName) + 1;

	if (nVersion > 2)
		cGpsTrack.name(pszName);
	else
		cGpsTrack.name(stdx::string_helper::to_utf8(pszName));

	/* Bypass display Flag */
	lpTrack += sizeof(unsigned char);

	/* Bypass Color */
	lpTrack += sizeof(unsigned int);

	/* Read waypoint number */
	unsigned int unNumber = *reinterpret_cast<unsigned int*>(lpTrack);
	lpTrack += sizeof(unsigned int);

	for (unsigned int i = 0; i < unNumber; i++)
	{
		CGpsPoint cGpsPoint;

		/* Read coordinates */
		P_GPS_POINT pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpTrack);
		lpTrack += sizeof(_GPS_POINT);

		cGpsPoint.lat(GDB_COORD_TO_DEGREE(pGpsPoint->nLatitude));
		cGpsPoint.lng(GDB_COORD_TO_DEGREE(pGpsPoint->nLongitude));

		if (pGpsPoint->bAltitude)
		{
			cGpsPoint.alt(*reinterpret_cast<double*>(lpTrack));
			lpTrack += sizeof(double);
		}

		unsigned char bFlag = *static_cast<unsigned char*>(lpTrack);
		lpTrack += sizeof(unsigned char);
		if (bFlag)
			lpTrack += sizeof(unsigned int); // Heure

		bFlag = *static_cast<unsigned char*>(lpTrack);
		lpTrack += sizeof(unsigned char);
		if (bFlag)
			lpTrack += sizeof(double); // Profondeur

		bFlag = *static_cast<unsigned char*>(lpTrack);
		lpTrack += sizeof(unsigned char);
		if (bFlag)
			lpTrack += sizeof(double); // Temperature

		cGpsTrack.push_back(cGpsPoint);
	}

	return S_OK;
}

static int ReadRoute(std::vector<std::string>& vecRoute, LPBYTE lpRoute, int nVersion)
{
	P_GPS_POINT pGpsPoint;
	char* pszName;
	unsigned int unNumber = 0;
	unsigned int unLinkNumber = 0;
	unsigned char bFlag;
	unsigned int i, j;

	/* Read Name */
	pszName = reinterpret_cast<char*>(lpRoute);
	lpRoute += strlen(pszName) + 1;

	if (nVersion > 2)
		vecRoute.push_back(pszName);
	else
		vecRoute.push_back(stdx::string_helper::to_utf8(pszName));

	/* Bypass Automate Flag */
	lpRoute += sizeof(unsigned char);

	/* Min/Max Flag */
	bFlag = *static_cast<unsigned char*>(lpRoute);
	lpRoute += sizeof(unsigned char);

	if (!bFlag)
	{
		/* Bypass max coordinates */
		pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpRoute);
		lpRoute += sizeof(_GPS_POINT);

		if (pGpsPoint->bAltitude)
			lpRoute += sizeof(double);

		/* Bypass min coordinates */
		pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpRoute);
		lpRoute += sizeof(_GPS_POINT);

		if (pGpsPoint->bAltitude)
			lpRoute += sizeof(double);
	}

	/* Read waypoint number */
	unNumber = *reinterpret_cast<unsigned int*>(lpRoute);
	lpRoute += sizeof(unsigned int);

	for (i = 0; i < unNumber; i++)
	{
		/* Read Name */
		pszName = reinterpret_cast<char*>(lpRoute);
		lpRoute += strlen(pszName) + 1;

		if (nVersion > 2)
			vecRoute.push_back(pszName);
		else
			vecRoute.push_back(stdx::string_helper::to_utf8(pszName));

		/* Bypass class */
		lpRoute += sizeof(int);

		/* Bypass Country */
		lpRoute += strlen(reinterpret_cast<char*>(lpRoute)) + 1;

		/* Bypass subclass */
		lpRoute += 22;

		if (*lpRoute)
		{
			lpRoute += 8;
			if (nVersion > 2)
				lpRoute += 8;
		}
		lpRoute++;

		/* Bypass unkown data */
		lpRoute += 18;

		/* Read Link number */
		unLinkNumber = *reinterpret_cast<unsigned int*>(lpRoute);
		lpRoute += sizeof(unsigned int);

		/* Bypass links waypoints */
		for (j = 0; j < unLinkNumber; j++)
		{
			pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpRoute);
			lpRoute += sizeof(_GPS_POINT);

			if (pGpsPoint->bAltitude)
				lpRoute += sizeof(double);
		}

		/* End Flag */
		bFlag = *static_cast<unsigned char*>(lpRoute);
		lpRoute += sizeof(unsigned char);

		if (!bFlag)
		{
			/* Bypass max coordinates */
			pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpRoute);
			lpRoute += sizeof(_GPS_POINT);

			if (pGpsPoint->bAltitude)
				lpRoute += sizeof(double);

			/* Bypass min coordinates */
			pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpRoute);
			lpRoute += sizeof(_GPS_POINT);

			if (pGpsPoint->bAltitude)
				lpRoute += sizeof(double);
		}

		if (nVersion > 1)
		{
			lpRoute += 8;
			if (nVersion > 2)
				lpRoute += 2;
		}
	}

	return S_OK;
}

static int ReadWayPoint(CGpsPointArray& cGpsWaypointsArray, CGpsPointArray& cGpsRoutePoints, LPBYTE lpWaypoint, int nVersion)
{
	P_GPS_POINT pGpsPoint;
	char* pszName;
	char* pszText;
	double fAltitude = 0;
	int nClass;

	/* Read Name*/
	pszName = reinterpret_cast<char*>(lpWaypoint);
	lpWaypoint += strlen(pszName) + 1;

	/* Read class */
	nClass = *reinterpret_cast<int*>(lpWaypoint);
	lpWaypoint += sizeof(int);

	/* Bypass Country */
	lpWaypoint += strlen(reinterpret_cast<char*>(lpWaypoint)) + 1;

	/* Bypass subclass */
	lpWaypoint += 22;

	/* Read coordinates */
	pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpWaypoint);
	lpWaypoint += sizeof(_GPS_POINT);

	if (pGpsPoint->bAltitude)
	{
		fAltitude = *reinterpret_cast<double*>(lpWaypoint);
		lpWaypoint += sizeof(double);
	}

	/* Read text */
	pszText = reinterpret_cast<char*>(lpWaypoint);
	lpWaypoint += strlen(pszText) + 1;

	CGpsPoint cGpsPoint;

	if (nVersion > 2)
	{
		cGpsPoint.name(pszName);
		cGpsPoint.comment(pszText);
	}
	else
	{
		cGpsPoint.name(stdx::string_helper::to_utf8(pszName));
		cGpsPoint.comment(stdx::string_helper::to_utf8(pszText));
	}

	cGpsPoint.coords(GDB_COORD_TO_DEGREE(pGpsPoint->nLatitude), GDB_COORD_TO_DEGREE(pGpsPoint->nLongitude), fAltitude);

	if (!nClass)
		cGpsWaypointsArray.push_back(cGpsPoint);
	else
		cGpsRoutePoints.push_back(cGpsPoint);

	return S_OK;
}

static int _ReadGDB(LPBYTE lpFile, DWORD dwFileSize, std::vector<CGpsPointArray*>& vecGpsArray)
{
	P_RECORD_HEADER pRecordHeader;
	LPBYTE lpPos = lpFile;
	LPBYTE lpEnd = lpFile + dwFileSize;
	int nVersion = 0;
	CGpsWaypointArray cGpsRoutePoints;
	std::vector<std::string> vecRoute;
	CGpsWaypointArray::iterator it;
	size_t i;

	// Check signature
	if (memcmp(lpFile, GDB_DEFAULT_SIGNATURE, GDB_SIZE_OF_SIGNATURE))
		return E_INVALIDARG;

	vecGpsArray.push_back(new CGpsWaypointArray);
	CGpsPointArray& cGpsWaypointsArray = *vecGpsArray.back();

	pRecordHeader = (P_RECORD_HEADER)(lpPos + GDB_SIZE_OF_SIGNATURE);

	while ((LPBYTE)pRecordHeader < lpEnd)
	{
		lpPos = (LPBYTE)pRecordHeader + sizeof(_RECORD_HEADER);
		switch (pRecordHeader->ucType)
		{
		case GDB_TYPE_FILE_FORMAT:    // File format version
			nVersion = *lpPos - 'k' + 1;
			break;

		case GDB_TYPE_FILE_VERSION:   // File version
			lpPos += 10; // MapSource\0
			break;

		case GDB_TYPE_WAYPOINT: // Waypoint
			ReadWayPoint(cGpsWaypointsArray, cGpsRoutePoints, lpPos, nVersion);
			break;

		case GDB_TYPE_TRACK:    // Track
			vecGpsArray.push_back(new CGpsTrack());
			ReadTrack(*vecGpsArray.back(), lpPos, nVersion);
			break;

		case GDB_TYPE_ROUTE:    // Route
			vecRoute.clear();
			ReadRoute(vecRoute, lpPos, nVersion);
			if (vecRoute.size())
			{
				vecGpsArray.push_back(new CGpsRoute());
				CGpsPointArray& cGpsPointArray = *vecGpsArray.back();

				cGpsPointArray.name(vecRoute[0]);

				for (i = 1; i < vecRoute.size(); i++)
				{
					std::string& strAddress = vecRoute[i];
					for (it = cGpsRoutePoints.begin(); it != cGpsRoutePoints.end(); ++it)
					{
						if (strAddress == it->name())
						{
							cGpsPointArray.push_back(*it);
							break;
						}
					}
				}
			}
			break;

		default:
			break;
		}

		pRecordHeader = (P_RECORD_HEADER)(lpPos + pRecordHeader->unLength);
	}


	return S_OK;
}

int ReadGDB(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	HRESULT hr = S_FALSE;
	DWORD dwFileSize = 0;
	HANDLE pFile = nullptr;
	HANDLE pFileMapping = nullptr;
	LPBYTE pMapFile = nullptr;

	pFile = CreateFile(strPathName.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (pFile == INVALID_HANDLE_VALUE)
	{
		// Error must return
		hr = GetLastError();
		goto end_function;
	}

	pFileMapping = CreateFileMapping(pFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (!pFileMapping)
	{
		// Error must return
		goto end_function;
	}

	pMapFile = (LPBYTE)MapViewOfFile(pFileMapping, FILE_MAP_READ, 0, 0, 0);
	if (!pMapFile)
	{
		// Error must return
		goto end_function;
	}

	dwFileSize = GetFileSize(pFile, nullptr);
	if (!dwFileSize)
		goto end_function;

	hr = _ReadGDB(pMapFile, dwFileSize, vecGpsArray);

end_function:
	if (pMapFile) UnmapViewOfFile(pMapFile);
	if (pFileMapping) CloseHandle(pFileMapping);
	if (pFile) CloseHandle(pFile);

	return hr;
}

