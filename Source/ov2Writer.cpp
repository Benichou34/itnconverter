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
 * Purpose : Writer for TomTom POI files (.ov2)
 *           www.tomtom.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"

#define POI_SIMPLE     0x02
#define POI_HEADER_LEN 13
#define POI_FACTOR     100000

static inline int32_t DoubleToInt32(double dDouble)
{
	return static_cast<int32_t>(floor(dDouble * POI_FACTOR) + 0.5);
}

int WriteOV2(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName, std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
	{
		const CGpsPoint& cGpsPoint = *it;
		std::string strName = stdx::string_helper::from_utf8(cGpsPoint.name());

		uint8_t uType = POI_SIMPLE;
		uint32_t uRecordLength = POI_HEADER_LEN + strName.size() + 1;
		int32_t nCoords;

		ofsFile.write(reinterpret_cast<char*>(&uType), sizeof(uType));
		ofsFile.write(reinterpret_cast<char*>(&uRecordLength), sizeof(uRecordLength));

		nCoords = DoubleToInt32(cGpsPoint.lng());
		ofsFile.write(reinterpret_cast<char*>(&nCoords), sizeof(nCoords));

		nCoords = DoubleToInt32(cGpsPoint.lat());
		ofsFile.write(reinterpret_cast<char*>(&nCoords), sizeof(nCoords));

		ofsFile.write(strName.c_str(), strName.size() + 1);
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
