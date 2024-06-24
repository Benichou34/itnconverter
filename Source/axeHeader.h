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
 * Purpose : Header for Microsoft Autoroute and MapPoint files (.axe, .ptm)
 */

#include "stdafx.h"
#include "ITN Tools.h"

#pragma pack(push, 1) // packing is now 1

typedef struct PROPERTY_HEADER
{
	WORD    wByteOrder;   // Always 0xFFFE
	WORD    wFormat;      // Always 0
	DWORD   dwOSVer;      // System version
	BYTE    clsid[16];    // Application CLSID
	DWORD   dwReserved;   // Should be 1
	BYTE    id[16];      // semantic name of a section
	DWORD   dwOffset;    // offset from start of whole property set
} sPropertyHeader;

typedef struct SECTION_OFFSET
{
	DWORD dwId;       // name of a property
	DWORD dwOffset;   // offset from the start of the section to that
} sSectionOffset;

typedef struct SECTION_HEADER
{
	DWORD   dwSectionSize;         // Size of section
	DWORD   dwPropertiesNumber;      // Count of properties in section
	sSectionOffset pSectionOffset;
} sSectionHeader;

typedef struct PROPERTY_CONTENT
{
	DWORD dwType;     // type of a property
	DWORD dwValue;    // value of property
} sPropertyContent;

typedef struct JOURNEY_HEADER
{
	short nField0;
	BYTE id[4];
	BYTE padding1[6];
	int nField1;
	DWORD dwPointsNumber;
	int nField2;
	BYTE padding2[8];
} sJourneyHeader;

typedef struct WAYPOINT_HEADER
{
	BYTE padding[32];
	int nLongitude;
	int nLatitude;
	BYTE cbCommentLength;
} sJourneyPointHeader;

#define AE2007_PADDING  7
#define AE2009_PADDING  23

typedef struct WAYPOINT_MIDDLE
{
	BYTE padding[3]; // Always FF FE FF
	BYTE cbCommentLength;
} sJourneyPointMiddle;

typedef struct WAYPOINT_DATA
{
	int nRouteType;
	int nField1;
	int nField2;
	int nField3;
	int nField4;
	int nIdentifier;
	double dDistance;
	int nStopLatitude;
	int nStopLongitude;
	int nStartLatitude;
	int nStartLongitude;
	int nField5;
	int nField6;
	int nField7;
	int nField8;
	int nField9;
	int nField10;
	int nField11;
} sJourneyPointData;

#pragma pack(pop) // packing is 8

#define MAX_DIFF_SC_RD     1000
#define AXE_TO_DEG(axe)    (float)(((double)axe * 360)/0x100000000)
#define DEG_TO_AXE(deg)    (int)(((double)deg * 0x100000000)/360)

#define JOURNEY_ID         "MASM"

#define STREAM_JOURNEY     L"Journey"
#define STREAM_PROPERTIES  L"\005Olhud5yvVwudb10uAaq5ezn4Ac"

#define PROP_VERSION_ID    0x00060002
#define PROP_COUNTRY_ID    0x00010000

#define PROP_TYPE_DWORD    0x00000003
#define PROP_TYPE_STRING   0x00000008

// Version history
#define MAP_VERSION_2002   09
#define MAP_VERSION_2003   10
#define MAP_VERSION_2004   11
#define MAP_VERSION_2005   12
#define MAP_VERSION_2006   13
#define MAP_VERSION_2007   14
#define MAP_VERSION_2008   15
#define MAP_VERSION_2009   16
#define MAP_VERSION_2010   17
#define MAP_VERSION_2011   18
#define MAP_VERSION_2013   19

HRESULT GetStream(const std::wstring& wstrFileName, const std::wstring& wstrStreamName, BYTE** ppBuffer, DWORD* pdwLength);
DWORD ReadVersion(LPBYTE pBuffer, DWORD dwLength);

