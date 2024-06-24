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
 * Purpose : Reader for Navigon MN|7 file (.route, .store)
 *           www.navigon.com
 */

#include "stdafx.h"
#include "ToolsLibrary/fmstream.h"
#include "ITN Tools.h"
#include "routeHeader.h"

#define GET_HIGH(msb, lsb) ((msb << 4) | (lsb >> 28))

namespace
{
	double NavigonToCoord(const _GPS_COORD& _Coord)
	{
		double dCoord;

		// Construction de la partie entière
		_GPS_COORD _LocalCoord = _Coord;
		bool bNegative = false;

		_LocalCoord.unMSB &= 0xFF;
		if (_LocalCoord.unMSB & COORD_NEGATIVE)
		{
			bNegative = true;
			_LocalCoord.unMSB &= ~COORD_NEGATIVE;
		}

		dCoord = COORD_REF_VAL / static_cast<double>(pow(2., static_cast<int>(GET_HIGH(COORD_REF_MSB, COORD_REF_LSB) - GET_HIGH(_LocalCoord.unMSB, _LocalCoord.unLSB))));
		_LocalCoord.unLSB &= ~COORD_MAX_LSB;

		// On en deduit les facteurs de multiplication
		double dFactor = dCoord / 16;
		for (int i = 0; i < 32; i += 8)
		{
			dCoord += ((_LocalCoord.unLSB >> (24 - i)) & 0xFF) * dFactor;
			dFactor /= 256;
		}

		dCoord /= COORD_FACTOR;
		if (bNegative)
			dCoord = -dCoord;

		return dCoord;
	}

	uint32_t ReadField(const char* pBuffer, CGpsPoint& cGpsPoint)
	{
		const _GPS_COORDS* pGpsCoords = nullptr;
		const _STRING* pString = nullptr;
		const _DATA* pData = reinterpret_cast<const _DATA*>(pBuffer);
		pBuffer += sizeof(_DATA);

		switch (pData->uiId)
		{
		case UID_STREET_GPS_CITY_CODE:
		case UID_STREET_GPS_CODE_CITY:
		case UID_NAME_GPS_CODE:
			// Name
			pString = reinterpret_cast<const _STRING*>(pBuffer);
			pBuffer += sizeof(_STRING);

			if (pString->uiLength && cGpsPoint.name().empty())
				cGpsPoint.name(std::string(reinterpret_cast<const char*>(pBuffer), pString->uiLength));
			pBuffer += pString->uiLength;

			if (pData->uiType == TYPE_DECAL || pData->uiType == TYPE_DECAL2)
				pBuffer += 4;

			// Coordinates
			pBuffer += 3;
			pGpsCoords = reinterpret_cast<const _GPS_COORDS*>(pBuffer);

			if (!cGpsPoint)
			{
				cGpsPoint.lng(NavigonToCoord(pGpsCoords->Longitude));
				cGpsPoint.lat(NavigonToCoord(pGpsCoords->Latitude));
			}
			break;

		default:
			break;
		}

		return pData->uiLength + sizeof(_DATA);
	}

	uint32_t ReadPoint(const char* pBuffer, CGpsPointArray& cGpsPointArray)
	{
		uint32_t unSize = *reinterpret_cast<const uint32_t*>(pBuffer);
		pBuffer += sizeof(uint32_t);

		// Lecture du nom
		const _DATA* pName = reinterpret_cast<const _DATA*>(pBuffer);
		pBuffer += sizeof(_DATA);

		CGpsPoint cGpsPoint;

		if (pName->uiLength)
		{
			cGpsPoint.name(std::string(reinterpret_cast<const char*>(pBuffer), pName->uiLength));
			pBuffer += pName->uiLength;
		}

		// Lecture du nombre de champs
		const _PT_FIELD* pPtFields = reinterpret_cast<const _PT_FIELD*>(pBuffer);
		pBuffer += sizeof(_PT_FIELD);

		for (uint32_t i = 0; i < pPtFields->uiFieldsNb; i++)
			pBuffer += ReadField(pBuffer, cGpsPoint);

		if (cGpsPoint)
			cGpsPointArray.push_back(cGpsPoint);

		return unSize + sizeof(uint32_t);
	}
}

int ReadROUTE(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	ifmstream ifmsFile(strPathName.c_str());
	if (!ifmsFile)
		return S_FALSE;

	const char* pBuffer = static_cast<const char*>(ifmsFile.data());
	uint32_t fileSize = static_cast<uint32_t>(ifmsFile.size());

	//	Verification de la version
	const _VERSION* pVersion = reinterpret_cast<const _VERSION*>(pBuffer);
	pBuffer += sizeof(_VERSION);

	if (pVersion->unMajor != gs_version.unMajor || pVersion->unMinor != gs_version.unMinor || pVersion->unBuild != gs_version.unBuild)
		return S_FALSE; // Error must return

	// Verification de la taille du fichier
	uint32_t unTotalLength = *reinterpret_cast<const uint32_t*>(pBuffer);
	pBuffer += sizeof(uint32_t);

	if (fileSize != (unTotalLength + sizeof(_VERSION)))
		return S_FALSE; // Error must return

	const _STRING* pDateTime = reinterpret_cast<const _STRING*>(pBuffer);
	pBuffer += sizeof(_STRING) + pDateTime->uiLength;

	// Lecture du header
	const _HEADER* pHeader = reinterpret_cast<const _HEADER*>(pBuffer);
	pBuffer += sizeof(_HEADER);

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	// Lecture des points
	for (uint32_t i = 0; i < pHeader->unPointNumber; i++)
		pBuffer += ReadPoint(pBuffer, cGpsRoute);

	return S_OK;
}
