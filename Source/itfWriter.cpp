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
 * Purpose : Writer for Sygic McGuider file (.itf)
 *           www.sygic.com / www.mcguider.com
 *           https://developers.sygic.com/documentation.php?action=navifiles_ITF
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "itfHeader.h"

namespace
{
	enum
	{
		ITF_WAYPOINT = 1, // Waypoint
		ITF_DESTINATION = 2, // Stopover or destination
		ITF_DEPARTURE = 3,  // Departure point (should only be set for the first item in the itinerary file)
		ITF_INVISIBLE = 4
	};

	const uint8_t raw2TSR[0x47] =
	{
		0x83, 0x73, 0x5F, 0x50, 0x3C, 0x28, 0x14, 0x08, 0x00, 0x00, 0x80, 0x3F,
		0x8F, 0xC2, 0x75, 0x3F, 0xC3, 0xF5, 0x68, 0x3F, 0xAE, 0x47, 0x61, 0x3F,
		0xCD, 0xCC, 0x4C, 0x3F, 0x33, 0x33, 0x33, 0x3F, 0x00, 0x00, 0x80, 0x3F,
		0x77, 0xBE, 0x7F, 0x3F, 0xEE, 0x7C, 0x7F, 0x3F, 0xEE, 0x7C, 0x7F, 0x3F,
		0x64, 0x3B, 0x7F, 0x3F, 0x64, 0x3B, 0x7F, 0x3F, 0x02, 0x00, 0x01, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	const uint8_t raw2OVA[0x1C] =
	{
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};

	const uint8_t rawData11[0x5F] =
	{
		0x83, 0x73, 0x5F, 0x50, 0x3C, 0x28, 0x14, 0x08, 0x00, 0x00, 0x80, 0x3F,
		0x8F, 0xC2, 0x75, 0x3F, 0xC3, 0xF5, 0x68, 0x3F, 0xAE, 0x47, 0x61, 0x3F,
		0xCD, 0xCC, 0x4C, 0x3F, 0x33, 0x33, 0x33, 0x3F, 0x00, 0x00, 0x80, 0x3F,
		0x77, 0xBE, 0x7F, 0x3F, 0xEE, 0x7C, 0x7F, 0x3F, 0xEE, 0x7C, 0x7F, 0x3F,
		0x64, 0x3B, 0x7F, 0x3F, 0x64, 0x3B, 0x7F, 0x3F, 0x02, 0x00, 0x01, 0x01,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	void writeString(std::ostream& os, const std::wstring& wstr)
	{
		uint16_t usLength = static_cast<uint16_t>(wstr.size());
		os.write(reinterpret_cast<const char*>(&usLength), sizeof(uint16_t));
		os.write(reinterpret_cast<const char*>(wstr.data()), wstr.size() * 2);
	}
}

int WriteOldITF(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	std::wstring wstrLine;

	SygicMcGuider::ITF_HEADER itfHeader;
	itfHeader.uiSignature = SygicMcGuider::HDR_SIGNATURE;
	itfHeader.uiPadding = SygicMcGuider::HDR_PADDING;
	itfHeader.usPointNumber2 = itfHeader.usPointNumber1 = static_cast<uint16_t>(cGpsRoute.size());

	// Write header
	ofsFile.write(reinterpret_cast<const char*>(&itfHeader), sizeof(SygicMcGuider::ITF_HEADER));

	// Write Points
	for (size_t i = 0; i < cGpsRoute.size(); ++i)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];
		SygicMcGuider::ITF_POINT itfPoint;

		if (i == cGpsRoute.upper_bound())
			itfPoint.usType = ITF_DESTINATION;
		else if (i == 0)
			itfPoint.usType = ITF_DEPARTURE;
		else
			itfPoint.usType = ITF_WAYPOINT;

		itfPoint.nLatitude = static_cast<int32_t>(cGpsPoint.lat() * SygicMcGuider::COORDS_FACTOR);
		itfPoint.nLongitude = static_cast<int32_t>(cGpsPoint.lng() * SygicMcGuider::COORDS_FACTOR);

		stdx::wstring_helper::from_utf8(cGpsPoint.name(), wstrLine);
		itfPoint.usNameLength = static_cast<uint16_t>((wstrLine.size() + 1) * 2);

		ofsFile.write(reinterpret_cast<const char*>(&itfPoint), sizeof(SygicMcGuider::ITF_POINT));
		ofsFile.write(reinterpret_cast<const char*>(wstrLine.c_str()), itfPoint.usNameLength);
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}

int WriteITF11(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	SygicGpsNavigation::ITF_HEADER itfHeader;
	itfHeader.uiSignature = SygicGpsNavigation::HDR_SIGNATURE;
	itfHeader.uiPadding = SygicGpsNavigation::HDR_PADDING;

	// Write header
	ofsFile.write(reinterpret_cast<const char*>(&itfHeader), sizeof(SygicGpsNavigation::ITF_HEADER));

	// Name
	writeString(ofsFile, stdx::wstring_helper::from_utf8(cGpsRoute.name()));

	// Write Points
	auto writePoint = [&ofsFile](const CGpsPoint& cGpsPoint1, const CGpsPoint& cGpsPoint2)
	{
		SygicGpsNavigation::ITF_POINT_11 itfPoint;
		memset(&itfPoint, 0x00, sizeof(SygicGpsNavigation::ITF_POINT_11));

		itfPoint.nLatitude = static_cast<int32_t>(cGpsPoint1.lat() * SygicGpsNavigation::COORDS_FACTOR);
		itfPoint.nLongitude = static_cast<int32_t>(cGpsPoint1.lng() * SygicGpsNavigation::COORDS_FACTOR);

		itfPoint.nNextLatitude = static_cast<int32_t>(cGpsPoint2.lat() * SygicGpsNavigation::COORDS_FACTOR);
		itfPoint.nNextLongitude = static_cast<int32_t>(cGpsPoint2.lng() * SygicGpsNavigation::COORDS_FACTOR);

		writeString(ofsFile, SygicGpsNavigation::CAR);
		ofsFile.write(reinterpret_cast<const char*>(&itfPoint), sizeof(SygicGpsNavigation::ITF_POINT_11));
		ofsFile.write(reinterpret_cast<const char*>(rawData11), sizeof(rawData11));
	};

	if (cGpsRoute.size() == 1)
	{
		writePoint(cGpsRoute.front(), CGpsPoint());
	}
	else
	{
		CGpsRoute::const_iterator itPrev = cGpsRoute.begin();
		CGpsRoute::const_iterator it = itPrev + 1;

		for (; it != cGpsRoute.end(); ++it)
		{
			writePoint(*itPrev, *it);
			itPrev = it;
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}

int WriteITF13(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	SygicGpsNavigation::ITF_HEADER itfHeader;
	itfHeader.uiSignature = SygicGpsNavigation::HDR_SIGNATURE;
	itfHeader.uiPadding = SygicGpsNavigation::HDR_PADDING;

	// Write header
	ofsFile.write(reinterpret_cast<const char*>(&itfHeader), sizeof(SygicGpsNavigation::ITF_HEADER));

	// Name
	writeString(ofsFile, stdx::wstring_helper::from_utf8(cGpsRoute.name()));

	if (cGpsRoute.empty())
		return S_OK;

	// Write Points
	auto writePoint = [&ofsFile](const CGpsPoint& cGpsPoint1, const CGpsPoint& cGpsPoint2)
	{
		SygicGpsNavigation::ITF_POINT_13 itfPoint;
		memset(&itfPoint, 0x00, sizeof(SygicGpsNavigation::ITF_POINT_13));

		itfPoint.nLatitude = static_cast<int32_t>(cGpsPoint1.lat() * SygicGpsNavigation::COORDS_FACTOR);
		itfPoint.nLongitude = static_cast<int32_t>(cGpsPoint1.lng() * SygicGpsNavigation::COORDS_FACTOR);

		itfPoint.nNextLatitude = static_cast<int32_t>(cGpsPoint2.lat() * SygicGpsNavigation::COORDS_FACTOR);
		itfPoint.nNextLongitude = static_cast<int32_t>(cGpsPoint2.lng() * SygicGpsNavigation::COORDS_FACTOR);

		writeString(ofsFile, SygicGpsNavigation::CAR);
		ofsFile.write(reinterpret_cast<const char*>(&SygicGpsNavigation::FTI), sizeof(uint32_t));
		ofsFile.write(reinterpret_cast<const char*>(&itfPoint), sizeof(SygicGpsNavigation::ITF_POINT_13));
		ofsFile.write(reinterpret_cast<const char*>(&SygicGpsNavigation::TSR), sizeof(uint32_t));
		ofsFile.write(reinterpret_cast<const char*>(raw2TSR), sizeof(raw2TSR));
		ofsFile.write(reinterpret_cast<const char*>(&SygicGpsNavigation::OVA), sizeof(uint32_t));
		ofsFile.write(reinterpret_cast<const char*>(raw2OVA), sizeof(raw2OVA));
	};

	if (cGpsRoute.size() == 1)
	{
		writePoint(cGpsRoute.front(), CGpsPoint());
	}
	else
	{
		CGpsRoute::const_iterator itPrev = cGpsRoute.begin();
		CGpsRoute::const_iterator it = itPrev + 1;

		for (; it != cGpsRoute.end(); ++it)
		{
			writePoint(*itPrev, *it);
			itPrev = it;
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
