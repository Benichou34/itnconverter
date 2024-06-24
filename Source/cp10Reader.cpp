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
 * Purpose : Reader for CoPilot 10 json file (.trp)
 *           www.alk.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"
#include "jsonParser/JsonParser.h"

namespace
{
	constexpr double TRP_FACTOR = 1000000.;
}

int ReadCP10(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	std::ifstream ifsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ifsFile)
		return S_FALSE;

	CJsonParser jsParser;
	try
	{
		ifsFile >> jsParser;
		const CJsonObject& jsonTrip = jsParser("Trip");

		vecGpsArray.push_back(new CGpsRoute());
		CGpsRoute& cGpsRoute = *static_cast<CGpsRoute*>(vecGpsArray.back());
		cGpsRoute.name(jsonTrip("Name"));

		const CJsonArray& jsonStops = jsonTrip("Stops");
		for (size_t i = 0; i < jsonStops.size(); i++)
		{
			const CJsonObject& jsonStopInfo = jsonStops[i]("StopInfo");
			CGpsPoint cGpsPoint;

			std::string strName = jsonStopInfo("Name");
			if (strName.empty())
				strName = jsonStopInfo("Address");

			cGpsPoint.name(strName);
			cGpsPoint.lng(static_cast<int>(jsonStopInfo("Longitude")) / TRP_FACTOR);
			cGpsPoint.lat(static_cast<int>(jsonStopInfo("Latitude")) / TRP_FACTOR);

			cGpsRoute.push_back(cGpsPoint);
		}
	}
	catch (CJsonException&)
	{
		return S_FALSE;
	}

	return S_OK;
}
