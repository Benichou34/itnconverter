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
 * Purpose : Header for Navigon MN|7 file (.route, .store)
 */

#if !defined(AFX_ROUTE_H_INCLUDED_)
#define AFX_ROUTE_H_INCLUDED_

#include <stdint.h>

#pragma pack(push, 1) // packing is now 1
typedef struct
{
	uint32_t unMajor;
	uint32_t unMinor;
	uint32_t unBuild;
} _VERSION, * P_VERSION;

typedef struct
{
	uint32_t unPointNumber;
	uint32_t unZero;
} _HEADER, * P_HEADER;

typedef struct
{
	uint32_t uiId;
	uint32_t uiType;
	uint32_t uiLength;
} _DATA, * P_DATA;

typedef struct
{
	uint32_t uiType;
	uint32_t uiFieldsNb;
	uint32_t uiLastField;
} _PT_FIELD, * P_PT_FIELD;

typedef struct
{
	uint32_t unLSB;
	uint32_t unMSB;
} _GPS_COORD, * P_GPS_COORD;

typedef struct
{
	_GPS_COORD Longitude;
	_GPS_COORD Latitude;
	uint8_t ucRFU;
} _GPS_COORDS, * P_GPS_COORDS;

typedef struct
{
	uint32_t uiType;
	uint32_t uiLength;
} _STRING, * P_STRING;

#define COORD_FACTOR	10000000
#define COORD_REF_VAL	(256. * COORD_FACTOR)
#define COORD_REF_LSB	0x70000000
#define COORD_MAX_LSB	0xF0000000
#define COORD_REF_MSB 	0x40
#define COORD_NEGATIVE 	0x80

#define TYPE_NAME		0x00000000
#define TYPE_FIELD		0x00000001
#define TYPE_DECAL		0x00000002
#define TYPE_DECAL2		0x00000004

#define UID_NAME						0x00000000
#define UID_STREET_GPS_CITY_CODE		0x0000008C
#define UID_STREET_GPS_CODE_CITY		0x0000003C
#define UID_STREET						0x00000032
#define UID_NUMBER_CODE					0x00000064
#define UID_CITY						0x00000011
#define UID_STATE						0x00000014
#define UID_NAME_GPS_CODE				0x00000028
#define UID_COUNTRY						0x0000000A

#define STRING_NAME						0x00000000
#define STRING_CITY						0x00000008
#define STRING_CODE						0x00000032
#define STRING_EMPTY					0x00000002

static _VERSION gs_version = { 0x0000FFFF, 0x00000001, 0x00000000 };

#pragma pack(pop) // packing is 8

#endif // !defined(AFX_ROUTE_H_INCLUDED_)
