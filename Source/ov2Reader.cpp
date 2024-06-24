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
 * Purpose : Reader for TomTom POI files (.ov2)
 *           www.tomtom.com
 */

#include "stdafx.h"
#include "ToolsLibrary/fmstream.h"
#include "ITN Tools.h"

#define POI_DELETED     0x00
#define POI_TYPE_1      0x01
#define POI_SIMPLE      0x02
#define POI_EXTENDED    0x03

#define POI_OFFSET_LEN     1
#define POI_OFFSET_LON     5
#define POI_OFFSET_LAT     9
#define POI_OFFSET_NAME   13
#define POI_TYPE_1_LEN    21

#define POI_FACTOR    100000.

int ReadOV2(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	ifmstream ifmsFile(strPathName.c_str());
	if (!ifmsFile)
		return S_FALSE;

	const char* pBuffer = static_cast<const char*>(ifmsFile.data());
	const void* pEnd = pBuffer + ifmsFile.size();

	uint32_t unRecordLength = 0;
	int32_t nCoordinate = 0;

	vecGpsArray.push_back(new CGpsPoiArray());

	while (pBuffer < pEnd)
	{
		// Read Type
		switch (*pBuffer)
		{
		case POI_DELETED:
			memcpy(&unRecordLength, pBuffer + POI_OFFSET_LEN, sizeof(uint32_t));
			break;
		case POI_SIMPLE:
		case POI_EXTENDED:
		{
			memcpy(&unRecordLength, pBuffer + POI_OFFSET_LEN, sizeof(uint32_t));

			CGpsPoint cGpsPoint;

			memcpy(&nCoordinate, pBuffer + POI_OFFSET_LON, sizeof(int32_t));
			cGpsPoint.lng(nCoordinate / POI_FACTOR);

			memcpy(&nCoordinate, pBuffer + POI_OFFSET_LAT, sizeof(int32_t));
			cGpsPoint.lat(nCoordinate / POI_FACTOR);

			cGpsPoint.name(stdx::string_helper::to_utf8(pBuffer + POI_OFFSET_NAME));

			vecGpsArray.back()->push_back(cGpsPoint);
		}
		break;

		case POI_TYPE_1:
			unRecordLength = POI_TYPE_1_LEN;
			break;

		default:
			return S_FALSE;
		}

		pBuffer += unRecordLength;
	}

	return S_OK;
}
