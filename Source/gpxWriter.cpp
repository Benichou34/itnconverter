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
 * Purpose : Writer for GPS exchange file (.gpx)
 *           www.topografix.com/gpx.asp
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "SAXParser/SAXWriter.h"

int WriteGPX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	{
		SYSTEMTIME sSystemTime;
		double fMinLat = 0;
		double fMaxLat = 0;
		double fMinLon = 0;
		double fMaxLon = 0;

		GetSystemTime(&sSystemTime);

		if (!cGpsRoute.empty())
		{
			// Retrieve bounds informations
			fMinLat = fMaxLat = cGpsRoute.front().lat();
			fMinLon = fMaxLon = cGpsRoute.front().lng();

			for (size_t i = 1; i < cGpsRoute.size(); i++)
			{
				const CGpsPoint& cGpsPoint = cGpsRoute[i];

				if (cGpsPoint.lat() < fMinLat)
					fMinLat = cGpsPoint.lat();

				if (cGpsPoint.lat() > fMaxLat)
					fMaxLat = cGpsPoint.lat();

				if (cGpsPoint.lng() < fMinLon)
					fMinLon = cGpsPoint.lng();

				if (cGpsPoint.lng() > fMaxLon)
					fMaxLon = cGpsPoint.lng();
			}
		}

		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();

		// Header
		SAXWriter::Tag tagGpx = std::move(xmlWriter.tag("gpx")
			.attribute("version", "1.0")
			.attribute("creator", SOFT_FULL_NAME " (" SOFT_URL ")")
			.attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
			.attribute("xmlns", "http://www.topografix.com/GPX/1/0")
			.attribute("xmlns:topografix", "http://www.topografix.com/GPX/Private/TopoGrafix/0/2")
			.attribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/0 http://www.topografix.com/GPX/1/0/gpx.xsd http://www.topografix.com/GPX/Private/TopoGrafix/0/2 http://www.topografix.com/GPX/Private/TopoGrafix/0/2/topografix.xsd"));

		xmlWriter.tag("time").content(stdx::format("%04d-%02d-%02dT%02d:%02d:%02dZ")
			(sSystemTime.wYear)
			(sSystemTime.wMonth)
			(sSystemTime.wDay)
			(sSystemTime.wHour)
			(sSystemTime.wMinute)
			(sSystemTime.wSecond));

		xmlWriter.tag("bounds")
			.attribute("minlat", stdx::string_helper::to_string(fMinLat))
			.attribute("minlon", stdx::string_helper::to_string(fMinLon))
			.attribute("maxlat", stdx::string_helper::to_string(fMaxLat))
			.attribute("maxlon", stdx::string_helper::to_string(fMaxLon));

		SAXWriter::Tag tagRte = xmlWriter.tag("rte");
		xmlWriter.tag("name").content(cGpsRoute.name());

		for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
		{
			const CGpsPoint& cGpsPoint = *it;

			SAXWriter::Tag tagRtept = std::move(xmlWriter.tag("rtept")
				.attribute("lat", stdx::string_helper::to_string(cGpsPoint.lat()))
				.attribute("lon", stdx::string_helper::to_string(cGpsPoint.lng())));

			if (cGpsPoint.alt() != 0)
				xmlWriter.tag("ele").content(stdx::string_helper::to_string(cGpsPoint.alt()));

			if (!cGpsPoint.name().empty())
			{
				if (dwFlag)
				{
					std::string strTmp = cGpsPoint.name();
					stdx::string_helper::remove(strTmp, ','); // Remove comma
					xmlWriter.tag("name").content(strTmp);
				}
				else
					xmlWriter.tag("name").content(cGpsPoint.name());
			}

			if (!cGpsPoint.comment().empty())
			{
				if (dwFlag)
				{
					std::string strTmp = cGpsPoint.comment();
					stdx::string_helper::remove(strTmp, ','); // Remove comma
					xmlWriter.tag("desc").cdata(strTmp);
				}
				else
					xmlWriter.tag("desc").cdata(cGpsPoint.comment());
			}
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
