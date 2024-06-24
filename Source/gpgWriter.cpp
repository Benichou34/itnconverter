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
 * Purpose : Writer for MapSource GPS eXchange file (.gpx)
 *           www.topografix.com/gpx.asp / www.garmin.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "SAXParser/SAXWriter.h"

int WriteGPG(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool)
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

		std::string strTime = stdx::format("%04d-%02d-%02dT%02d:%02d:%02dZ")
			(sSystemTime.wYear)
			(sSystemTime.wMonth)
			(sSystemTime.wDay)
			(sSystemTime.wHour)
			(sSystemTime.wMinute)
			(sSystemTime.wSecond);

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

		SAXWriter::Tag tagGpx = std::move(xmlWriter.tag("gpx")
			.attribute("xmlns", "http://www.topografix.com/GPX/1/1")
			.attribute("xmlns:gpxx", "http://www.garmin.com/xmlschemas/GpxExtensions/v3")
			.attribute("xmlns:trp", "http://www.garmin.com/xmlschemas/TripExtensions/v1")
			.attribute("creator", SOFT_FULL_NAME " (" SOFT_URL ")")
			.attribute("version", "1.1")
			.attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
			.attribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd http://www.garmin.com/xmlschemas/GpxExtensions/v3 http://www.garmin.com/xmlschemas/GpxExtensionsv3.xsd http://www.garmin.com/xmlschemas/TripExtensions/v1 http://www.garmin.com/xmlschemas/TripExtensionsv1.xsd"));

		// Metadata
		{
			SAXWriter::Tag tagMetadata = xmlWriter.tag("metadata");

			{
				SAXWriter::Tag tagAuthor = xmlWriter.tag("author");
				xmlWriter.tag("name").content(SOFT_COMPAGNY);

				SAXWriter::Tag tagLink = std::move(xmlWriter.tag("link").attribute("href", SOFT_URL));
				xmlWriter.tag("text").content(SOFT_COMPAGNY);
			}

			xmlWriter.tag("time").content(strTime);

			xmlWriter.tag("bounds")
				.attribute("minlat", stdx::string_helper::to_string(fMinLat))
				.attribute("minlon", stdx::string_helper::to_string(fMinLon))
				.attribute("maxlat", stdx::string_helper::to_string(fMaxLat))
				.attribute("maxlon", stdx::string_helper::to_string(fMaxLon));
		}

		auto writeGarminExtension = [&](const std::string& prefix, const std::string& extension, const std::string& property = std::string(), const std::string& value = std::string())
		{
			// Garmin Extensions
			SAXWriter::Tag tagExt = xmlWriter.tag(prefix + ":" + extension);
			if (!property.empty())
			{
				SAXWriter::Tag tagProperty = xmlWriter.tag(prefix + ":" + property);
				if (!value.empty())
					tagProperty.content(value);
			}
		};

		auto writePoint = [&](
			const CGpsPoint& cGpsPoint,
			const std::string& type,
			const std::function<void()>& callbackExtension)
		{
			SAXWriter::Tag tagWpt = std::move(xmlWriter.tag(type)
				.attribute("lat", stdx::string_helper::to_string(cGpsPoint.lat()))
				.attribute("lon", stdx::string_helper::to_string(cGpsPoint.lng())));

			xmlWriter.tag("ele").content(stdx::string_helper::to_string(cGpsPoint.alt())); // Units: meters
			xmlWriter.tag("time").content(strTime);

			if (!cGpsPoint.name().empty())
				xmlWriter.tag("name").content(cGpsPoint.name());

			if (!cGpsPoint.comment().empty())
				xmlWriter.tag("desc").cdata(cGpsPoint.comment());

			xmlWriter.tag("sym").content("Waypoint");

			if (callbackExtension)
			{
				// Garmin Extensions
				SAXWriter::Tag tagExtensions = xmlWriter.tag("extensions");
				callbackExtension();
			}
		};

		// Waypoints
		for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
			writePoint(*it, "wpt", [&] { writeGarminExtension("gpxx", "WaypointExtension", "DisplayMode", "SymbolAndName"); });

		// Route
		SAXWriter::Tag tagRte = xmlWriter.tag("rte");
		xmlWriter.tag("name").content(cGpsRoute.name());

		// Garmin Extensions
		writeGarminExtension("gpxx", "RouteExtension", "IsAutoNamed", "true");

		for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
		{
			writePoint(*it, "rtept", [&]
				{
					writeGarminExtension("gpxx", "RoutePointExtension", "Subclass", "000000000000ffffffffffffffffffffffff");
					if (dwFlag && it != cGpsRoute.begin() && it != (cGpsRoute.end() - 1))
						writeGarminExtension("trp", "ViaPoint");
				});
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
