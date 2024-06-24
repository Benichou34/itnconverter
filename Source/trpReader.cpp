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
 * Purpose : Reader for CoPilot 6, 7, 8, 9 & 10 file (.trp)
 *           www.alk.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"
#include "stdx/bom.h"

namespace
{
	const std::wstring START_TRIP(L"Start Trip");
	const std::wstring END_TRIP(L"End Trip");
	const std::wstring START_STOP(L"Start Stop");
	const std::wstring END_STOP(L"End Stop");
	const std::wstring ADDRESS(L"Address");
	const std::wstring ZIP(L"Zip");
	const std::wstring CITY(L"City");
	const std::wstring STATE(L"State");
	const std::wstring LONGITUDE(L"Longitude");
	const std::wstring LATITUDE(L"Latitude");

	constexpr double TRP_FACTOR = 1000000.;
}

int ReadTRP(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	std::wifstream ifsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ifsFile)
		return S_FALSE;

	stdx::wbom::imbue(ifsFile);

	vecGpsArray.push_back(new CGpsRoute());
	CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());

	bool bPoint = false;
	CGpsPoint cGpsPoint;
	std::wstring strLine;
	std::wstring strAddress;
	std::wstring strZip;
	std::wstring strCity;
	std::wstring strState;

	while (std::getline(ifsFile, strLine))
	{
		stdx::wstring_helper::trimright(strLine);

		stdx::wstring_helper::vector vecResult;
		stdx::wstring_helper::split(strLine, vecResult, stdx::wfind_first(L"="));

		if (vecResult.size())
		{
			if (bPoint)
			{
				if (vecResult[0] == END_STOP)
				{
					std::wstring strName = strAddress;

					if (!strZip.empty())
					{
						if (!strName.empty())
							strName += L", ";
						strName += strZip;
					}

					if (!strCity.empty())
					{
						if (!strName.empty())
							strName += L" ";
						strName += strCity;
					}

					if (!strState.empty())
					{
						if (!strName.empty())
							strName += L", ";
						strName += strState;
					}

					if (!strName.empty())
						cGpsPoint.name(stdx::wstring_helper::to_utf8(strName));

					cGpsRoute.push_back(cGpsPoint);

					strAddress.clear();
					strZip.clear();
					strCity.clear();
					strState.clear();
					cGpsPoint.clear();
					bPoint = false;
				}
				else if (vecResult[0] == ADDRESS)
					strAddress = vecResult[1];
				else if (vecResult[0] == ZIP)
					strZip = vecResult[1];
				else if (vecResult[0] == CITY)
					strCity = vecResult[1];
				else if (vecResult[0] == STATE)
					strState = vecResult[1];
				else if (vecResult[0] == LONGITUDE)
					cGpsPoint.lng(stdx::wstring_helper::string_to<int>(vecResult[1]) / TRP_FACTOR);
				else if (vecResult[0] == LATITUDE)
					cGpsPoint.lat(stdx::wstring_helper::string_to<int>(vecResult[1]) / TRP_FACTOR);
			}
			else if (vecResult[0] == START_STOP)
			{
				cGpsPoint.name(stdx::wstring_helper::to_utf8(vecResult[1]));
				bPoint = true;
			}
			else if (vecResult[0] == START_TRIP)
				cGpsRoute.name(stdx::wstring_helper::to_utf8(vecResult[1]));
		}
	}

	return S_OK;
}
