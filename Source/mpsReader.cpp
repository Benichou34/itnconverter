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
 * Purpose : Reader for Garmin Map Source 5.x file (.mps)
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
	double fAltitude;
} _GPS_POINT, * P_GPS_POINT;

#pragma pack(pop) // packing is 8

#define MPS_SIZE_OF_SIGNATURE    6
#define MPS_DEFAULT_SIGNATURE    "MsRcd\0"

#define MPS_TYPE_FILE_FORMAT     'D'
#define MPS_TYPE_FILE_VERSION    'A'
#define MPS_TYPE_WAYPOINT        'W'
#define MPS_TYPE_TRACK           'T'
#define MPS_TYPE_ROUTE           'R'
#define MPS_TYPE_MAPSEGMENT      'L'
#define MPS_TYPE_MAPSET          'V'

#define MPS_COORD_TO_DEGREE(x)	static_cast<float>((x*180.)/0x80000000UL)

static int ReadTrack(CGpsPointArray& cGpsTrack, LPBYTE lpTrack)
{
	P_GPS_POINT pGpsPoint;
	unsigned int unNumber = 0;
	unsigned int i;

	/* Read Name */
	cGpsTrack.name(stdx::string_helper::to_utf8(reinterpret_cast<char*>(lpTrack)));
	lpTrack += strlen(reinterpret_cast<char*>(lpTrack)) + 1;

	/* Bypass display Flag */
	lpTrack += sizeof(unsigned char);

	/* Bypass Color */
	lpTrack += sizeof(unsigned int);

	/* Read waypoint number */
	unNumber = *reinterpret_cast<unsigned int*>(lpTrack);
	lpTrack += sizeof(unsigned int);

	for (i = 0; i < unNumber; i++)
	{
		CGpsPoint cGpsPoint;

		/* Read coordinates */
		pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpTrack);
		lpTrack += sizeof(_GPS_POINT);

		cGpsPoint.coords(MPS_COORD_TO_DEGREE(pGpsPoint->nLatitude), MPS_COORD_TO_DEGREE(pGpsPoint->nLongitude), pGpsPoint->fAltitude);

		lpTrack += sizeof(unsigned char) + sizeof(unsigned int); // Heure
		lpTrack += sizeof(unsigned char) + sizeof(double); // Profondeur

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
	unsigned int i;

	/* Read Name */
	pszName = reinterpret_cast<char*>(lpRoute);
	lpRoute += strlen(pszName) + 1;

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

		/* Bypass min coordinates */
		pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpRoute);
		lpRoute += sizeof(_GPS_POINT);
	}

	/* Read waypoint number */
	unNumber = *reinterpret_cast<unsigned int*>(lpRoute);
	lpRoute += sizeof(unsigned int);

	for (i = 0; i < unNumber; i++)
	{
		/* Read Name */
		pszName = reinterpret_cast<char*>(lpRoute);
		lpRoute += strlen(pszName) + 1;

		vecRoute.push_back(stdx::string_helper::to_utf8(pszName));

		/* Bypass class */
		lpRoute += sizeof(int);

		/* Bypass Country */
		lpRoute += strlen(reinterpret_cast<char*>(lpRoute)) + 1;

		/* Bypass subclass */
		lpRoute += 17;
		if (nVersion > 3)
		{
			lpRoute += 5;

			/* Bypass unkown string */
			lpRoute += strlen(reinterpret_cast<char*>(lpRoute)) + 1;
		}

		/* Bypass unkown data */
		lpRoute += 18;

		/* Read Link number */
		unLinkNumber = *reinterpret_cast<unsigned int*>(lpRoute);
		lpRoute += sizeof(unsigned int);

		/* Bypass links waypoints */
		lpRoute += unLinkNumber * sizeof(_GPS_POINT);

		/* End Flag */
		bFlag = *static_cast<unsigned char*>(lpRoute);
		lpRoute += sizeof(unsigned char);

		if (!bFlag)
		{
			/* Bypass max coordinates */
			pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpRoute);
			lpRoute += sizeof(_GPS_POINT);

			/* Bypass min coordinates */
			pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpRoute);
			lpRoute += sizeof(_GPS_POINT);
		}
	}

	return S_OK;
}

static int ReadWayPoint(CGpsPointArray& cGpsWaypointsArray, CGpsPointArray& cGpsRoutePoints, LPBYTE lpWaypoint, int nVersion)
{
	P_GPS_POINT pGpsPoint;
	CGpsPoint cGpsPoint;
	int nClass;

	/* Read Name*/
	cGpsPoint.name(stdx::string_helper::to_utf8(reinterpret_cast<char*>(lpWaypoint)));
	lpWaypoint += strlen(reinterpret_cast<char*>(lpWaypoint)) + 1;

	/* Read class */
	nClass = *reinterpret_cast<int*>(lpWaypoint);
	lpWaypoint += sizeof(int);

	/* Bypass Country */
	lpWaypoint += strlen(reinterpret_cast<char*>(lpWaypoint)) + 1;

	/* Bypass subclass */
	lpWaypoint += 17;
	if (nVersion > 3)
		lpWaypoint += 5;

	/* Read coordinates */
	pGpsPoint = reinterpret_cast<P_GPS_POINT>(lpWaypoint);
	lpWaypoint += sizeof(_GPS_POINT);

	/* Read text */
	cGpsPoint.comment(stdx::string_helper::to_utf8(reinterpret_cast<char*>(lpWaypoint)));
	lpWaypoint += strlen(reinterpret_cast<char*>(lpWaypoint)) + 1;

	cGpsPoint.coords(MPS_COORD_TO_DEGREE(pGpsPoint->nLatitude), MPS_COORD_TO_DEGREE(pGpsPoint->nLongitude), pGpsPoint->fAltitude);

	if (!nClass)
		cGpsWaypointsArray.push_back(cGpsPoint);
	else
		cGpsRoutePoints.push_back(cGpsPoint);

	return S_OK;
}

static int _ReadMPS(LPBYTE lpFile, DWORD dwFileSize, std::vector<CGpsPointArray*>& vecGpsArray)
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
	if (memcmp(lpFile, MPS_DEFAULT_SIGNATURE, MPS_SIZE_OF_SIGNATURE))
		return E_INVALIDARG;

	vecGpsArray.push_back(new CGpsWaypointArray);
	CGpsPointArray& cGpsWaypointsArray = *vecGpsArray.back();

	pRecordHeader = (P_RECORD_HEADER)(lpPos + MPS_SIZE_OF_SIGNATURE);

	while ((LPBYTE)pRecordHeader < lpEnd)
	{
		lpPos = (LPBYTE)pRecordHeader + sizeof(_RECORD_HEADER);
		switch (pRecordHeader->ucType)
		{
		case MPS_TYPE_FILE_FORMAT:    // File format version
			if (*lpPos == 'd')
				nVersion = 3;
			else if (*lpPos > 'd' && *lpPos < 'i')
				nVersion = 4;
			else if (*lpPos == 'i')
				nVersion = 5;
			else
				return E_NOTIMPL;
			break;

		case MPS_TYPE_FILE_VERSION:   // File version
			break;

		case MPS_TYPE_WAYPOINT: // Waypoint
			ReadWayPoint(cGpsWaypointsArray, cGpsRoutePoints, lpPos, nVersion);
			break;

		case MPS_TYPE_TRACK:    // Track
			vecGpsArray.push_back(new CGpsTrack());
			ReadTrack(*vecGpsArray.back(), lpPos);
			break;

		case MPS_TYPE_ROUTE:    // Route
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

int ReadMPS(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
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

	hr = _ReadMPS(pMapFile, dwFileSize, vecGpsArray);

end_function:
	if (pMapFile) UnmapViewOfFile(pMapFile);
	if (pFileMapping) CloseHandle(pFileMapping);
	if (pFile) CloseHandle(pFile);

	return hr;
}
