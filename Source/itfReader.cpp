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
 * Purpose : Reader for Sygic McGuider file (.itf)
 *           www.sygic.com / www.mcguider.com
 */

#include "stdafx.h"
#include "ToolsLibrary/fmstream.h"
#include "ITN Tools.h"
#include "itfHeader.h"

const uint32_t SygicMcGuider::HDR_SIGNATURE = 0x03C32B55;
const uint32_t SygicMcGuider::HDR_PADDING = 0xFFFF0000;
const int SygicMcGuider::COORDS_FACTOR = 100000;

static HRESULT ReadMcGuiderFormat(const ifmstream& ifmsFile, std::vector<CGpsPointArray*>& vecGpsArray)
{
	const SygicMcGuider::ITF_HEADER* pHeader = reinterpret_cast<const SygicMcGuider::ITF_HEADER*>(ifmsFile.data());

	if (pHeader->uiSignature != SygicMcGuider::HDR_SIGNATURE || !pHeader->usPointNumber2)
		return S_FALSE;

	std::unique_ptr<CGpsRoute> pRoute(new CGpsRoute);
	const SygicMcGuider::ITF_POINT* pPoint = reinterpret_cast<const SygicMcGuider::ITF_POINT*>(reinterpret_cast<const char*>(pHeader) + sizeof(SygicMcGuider::ITF_HEADER));

	for (int i = 0; i < pHeader->usPointNumber2; ++i)
	{
		CGpsPoint cGpsPoint;

		const wchar_t* pName = reinterpret_cast<const wchar_t*>(reinterpret_cast<const char*>(pPoint) + sizeof(SygicMcGuider::ITF_POINT));
		cGpsPoint.name(stdx::wstring_helper::to_utf8(std::wstring(pName, pPoint->usNameLength / 2)));

		cGpsPoint.lng(static_cast<double>(pPoint->nLongitude) / SygicMcGuider::COORDS_FACTOR);
		cGpsPoint.lat(static_cast<double>(pPoint->nLatitude) / SygicMcGuider::COORDS_FACTOR);

		pRoute->push_back(cGpsPoint);
		pPoint = reinterpret_cast<const SygicMcGuider::ITF_POINT*>(reinterpret_cast<const char*>(pName) + pPoint->usNameLength);
	}

	if (!pRoute->empty())
		vecGpsArray.push_back(pRoute.release());

	return S_OK;
}

const std::wstring SygicGpsNavigation::CAR(L"car");
const uint32_t SygicGpsNavigation::FTI = 0x49544633;
const uint32_t SygicGpsNavigation::TSR = 0x52535432;
const uint32_t SygicGpsNavigation::OVA = 0x41564F32;

const uint32_t SygicGpsNavigation::HDR_SIGNATURE = 0x00400003;
const uint32_t SygicGpsNavigation::HDR_PADDING = 0x00370076;
const int SygicGpsNavigation::COORDS_FACTOR = 100000;

#define SIZEOF_STRING(pStr) (sizeof(uint16_t) + (pStr->usLength) * 2)

static HRESULT ReadGpsNavigationFormat(const ifmstream& ifmsFile, std::vector<CGpsPointArray*>& vecGpsArray)
{
	const void* pEnd = reinterpret_cast<const char*>(ifmsFile.data()) + ifmsFile.size();
	const SygicGpsNavigation::ITF_HEADER* pHeader = reinterpret_cast<const SygicGpsNavigation::ITF_HEADER*>(ifmsFile.data());

	if (pHeader->uiSignature != SygicGpsNavigation::HDR_SIGNATURE)
		return S_FALSE;

	std::unique_ptr<CGpsRoute> pRoute(new CGpsRoute);

	const SygicGpsNavigation::ITF_STRING* pString = reinterpret_cast<const SygicGpsNavigation::ITF_STRING*>(reinterpret_cast<const char*>(pHeader) + sizeof(SygicGpsNavigation::ITF_HEADER));
	pRoute->name(stdx::wstring_helper::to_utf8(std::wstring(&(pString->wString), pString->usLength)));

	const char* pCurrent = reinterpret_cast<const char*>(pString) + SIZEOF_STRING(pString);

	while (pCurrent < pEnd)
	{
		CGpsPoint cGpsPoint;

		pString = reinterpret_cast<const SygicGpsNavigation::ITF_STRING*>(pCurrent);
		if (SygicGpsNavigation::CAR != std::wstring(&(pString->wString), pString->usLength))
			break;

		pCurrent += SIZEOF_STRING(pString);
		if (*reinterpret_cast<const uint32_t*>(pCurrent) == SygicGpsNavigation::FTI)
		{
			pCurrent += sizeof(uint32_t);
			const SygicGpsNavigation::ITF_POINT_13* pPoint = reinterpret_cast<const SygicGpsNavigation::ITF_POINT_13*>(pCurrent);

			if (pRoute->empty())
			{
				cGpsPoint.lng(static_cast<double>(pPoint->nLongitude) / SygicGpsNavigation::COORDS_FACTOR);
				cGpsPoint.lat(static_cast<double>(pPoint->nLatitude) / SygicGpsNavigation::COORDS_FACTOR);
				cGpsPoint.name(stdx::string_helper::to_string(pRoute->size() + 1));
				pRoute->push_back(cGpsPoint);
			}

			cGpsPoint.lng(static_cast<double>(pPoint->nNextLongitude) / SygicGpsNavigation::COORDS_FACTOR);
			cGpsPoint.lat(static_cast<double>(pPoint->nNextLatitude) / SygicGpsNavigation::COORDS_FACTOR);

			pCurrent += sizeof(SygicGpsNavigation::ITF_POINT_13) + sizeof(uint32_t) + 0x47 + sizeof(uint32_t) + 0x1C; // + 2TSR + 2OVA
		}
		else
		{
			const SygicGpsNavigation::ITF_POINT_11* pPoint = reinterpret_cast<const SygicGpsNavigation::ITF_POINT_11*>(pCurrent);

			if (pRoute->empty())
			{
				cGpsPoint.lng(static_cast<double>(pPoint->nLongitude) / SygicGpsNavigation::COORDS_FACTOR);
				cGpsPoint.lat(static_cast<double>(pPoint->nLatitude) / SygicGpsNavigation::COORDS_FACTOR);
				cGpsPoint.name(stdx::string_helper::to_string(pRoute->size() + 1));
				pRoute->push_back(cGpsPoint);
			}

			cGpsPoint.lng(static_cast<double>(pPoint->nNextLongitude) / SygicGpsNavigation::COORDS_FACTOR);
			cGpsPoint.lat(static_cast<double>(pPoint->nNextLatitude) / SygicGpsNavigation::COORDS_FACTOR);

			pCurrent += sizeof(SygicGpsNavigation::ITF_POINT_11) + 0x5F;
		}

		cGpsPoint.name(stdx::string_helper::to_string(pRoute->size() + 1));
		pRoute->push_back(cGpsPoint);
	}

	if (!pRoute->empty())
		vecGpsArray.push_back(pRoute.release());

	return S_OK;
}

int ReadITF(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	ifmstream ifmsFile(strPathName.c_str());
	if (!ifmsFile)
		return S_FALSE;

	if (*reinterpret_cast<const uint32_t*>(ifmsFile.data()) == SygicMcGuider::HDR_SIGNATURE)
		ReadMcGuiderFormat(ifmsFile, vecGpsArray);
	else
		ReadGpsNavigationFormat(ifmsFile, vecGpsArray);

	return S_OK;
}
