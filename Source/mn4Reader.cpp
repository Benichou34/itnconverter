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
 * Purpose : Reader for Navigon MN|4, MN|5, MN|6 and MN|7 file (.rte)
 *           www.navigon.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "GeoServices/GeoServices.h"
#include "stdx/string_helper.h"
#include "stdx/bom.h"

namespace
{
	int ParseMN4(stdx::string_helper::vector& vecStrResult, CGpsRoute& cGpsRoute, bool bUtf8)
	{
		HRESULT hr = S_OK;
		std::string strPostalCode;

		CGpsPoint cGpsPoint;

		if (vecStrResult.size() > 15)
		{
			strPostalCode = vecStrResult[4];
			strPostalCode += " ";
			strPostalCode += vecStrResult[5];

			cGpsPoint.name(bUtf8 ? vecStrResult[5] : stdx::string_helper::to_utf8(vecStrResult[5]));
			cGpsPoint.lng(stdx::string_helper::string_to<double>(vecStrResult[11]));
			cGpsPoint.lat(stdx::string_helper::string_to<double>(vecStrResult[12]));
		}
		else
		{
			strPostalCode = vecStrResult[3];
			strPostalCode += " ";
			strPostalCode += vecStrResult[4];

			cGpsPoint.name(bUtf8 ? vecStrResult[4] : stdx::string_helper::to_utf8(vecStrResult[4]));
			cGpsPoint.lng(stdx::string_helper::string_to<double>(vecStrResult[10]));
			cGpsPoint.lat(stdx::string_helper::string_to<double>(vecStrResult[11]));
		}

		// If coordinates are empty, find on Google
		if (!cGpsPoint)
		{
			geo::CGeoGeocoder gGeocoder(geo::E_GEO_PROVIDER_GOOGLE_API);

			gGeocoder->Load(strPostalCode);
			if (gGeocoder->getStatus() == geo::E_GEO_OK)
			{
				const geo::CGeoLocations& geoResults = gGeocoder->getResults();
				if (geoResults.size())
					cGpsPoint = geoResults.front();
			}
		}

		cGpsRoute.push_back(cGpsPoint);

		return hr;
	}

	int ParseMN6(stdx::string_helper::vector& vecStrResult, CGpsRoute& cGpsRoute, bool bUtf8)
	{
		if (vecStrResult.size() > 5)
		{
			CGpsPoint cGpsPoint;

			std::string strName;
			if (vecStrResult[0].size() > 1)
				strName = vecStrResult[0].c_str() + 1;
			else
				strName = vecStrResult[2];

			cGpsPoint.name(bUtf8 ? strName : stdx::string_helper::to_utf8(strName));
			cGpsPoint.lng(stdx::string_helper::string_to<double>(vecStrResult[4]));
			cGpsPoint.lat(stdx::string_helper::string_to<double>(vecStrResult[5]));

			cGpsRoute.push_back(cGpsPoint);
		}

		return S_OK;
	}

	int ParseMN7(stdx::string_helper::vector& vecStrResult, CGpsRoute& cGpsRoute, bool bUtf8)
	{
		if (vecStrResult.size() > 6)
		{
			CGpsPoint cGpsPoint;

			std::string strName;
			if (vecStrResult[0].size() > 1)
				strName = vecStrResult[0].c_str() + 1;
			else
				strName = vecStrResult[3];

			cGpsPoint.name(bUtf8 ? strName : stdx::string_helper::to_utf8(strName));
			cGpsPoint.lng(stdx::string_helper::string_to<double>(vecStrResult[5]));
			cGpsPoint.lat(stdx::string_helper::string_to<double>(vecStrResult[6]));

			cGpsRoute.push_back(cGpsPoint);
		}

		return S_OK;
	}
}

int ReadMNX(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	std::ifstream ifsFile(strPathName.c_str());
	if (!ifsFile)
		return S_FALSE;

	bool bUtf8 = (stdx::bom::read(ifsFile) == stdx::bom::utf_8); // UTF-8

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	std::string strLine;
	while (std::getline(ifsFile, strLine))
	{
		stdx::string_helper::vector vecStrResult;
		stdx::string_helper::split(strLine, vecStrResult, stdx::find_first("|"));

		if (vecStrResult.size() > 1)
		{
			if (vecStrResult[0][0] == '[')
				if (vecStrResult[1][0] == ']')
					ParseMN6(vecStrResult, cGpsRoute, bUtf8);
				else
					ParseMN7(vecStrResult, cGpsRoute, bUtf8);
			else
				ParseMN4(vecStrResult, cGpsRoute, bUtf8);
		}
	}

	cGpsRoute.removeEmpties();

	return S_OK;
}
