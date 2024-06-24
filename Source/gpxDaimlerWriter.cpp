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

int WriteDaimler50GPX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();

		// Header
		SAXWriter::Tag tagGpx = std::move(xmlWriter.tag("gpx:gpx")
			.attribute("version", "1.1")
			.attribute("creator", SOFT_FULL_NAME " (" SOFT_URL ")")
			.attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
			.attribute("xmlns:gpx", "http://www.topografix.com/GPX/1/1")
			.attribute("xmlns:gpxd", "http://www.daimler.com/DaimlerGPXExtensions/V2.7.4")
			.attribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd")
		);

		SAXWriter::Tag tagRte = xmlWriter.tag("gpx:rte");
		xmlWriter.tag("gpx:name").content(cGpsRoute.name());

		for (const CGpsPoint& cGpsPoint : cGpsRoute)
		{
			SAXWriter::Tag tagRtept = std::move(xmlWriter.tag("gpx:rtept")
				.attribute("lat", stdx::string_helper::to_string(cGpsPoint.lat()))
				.attribute("lon", stdx::string_helper::to_string(cGpsPoint.lng()))
				.content(std::string())
			);

			if (!cGpsPoint.name().empty())
				xmlWriter.tag("gpx:name").content(cGpsPoint.name());

			if (!cGpsPoint.comment().empty())
				xmlWriter.tag("gpx:desc").cdata(cGpsPoint.comment());
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}

int WriteDaimler55GPX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();

		// Header
		SAXWriter::Tag tagGpx = std::move(xmlWriter.tag("gpx")
			.attribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 GPX_Schema_Topografix.xsd http://www.daimler.com/DaimlerGPXExtensions/V2.7.4 Daimler_GPX_Extension_V2_7_4.xsd")
			.attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
			.attribute("xmlns:Gpx", "http://www.topografix.com/GPX/1/1")
			.attribute("xmlns:Gpxd", "http://www.daimler.com/DaimlerGPXExtensions/V2.7.4")
			.attribute("xmlns", "http://www.topografix.com/GPX/1/1")
			.attribute("creator", SOFT_FULL_NAME " (" SOFT_URL ")")
			.attribute("version", "1.1")
		);

		SAXWriter::Tag tagRte = xmlWriter.tag("rte");
		xmlWriter.tag("name").content(cGpsRoute.name());

		for (auto it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
		{
			const CGpsPoint& cGpsPoint = *it;

			SAXWriter::Tag tagRtept = std::move(xmlWriter.tag("gpx:rtept")
				.attribute("lat", stdx::string_helper::to_string(cGpsPoint.lat()))
				.attribute("lon", stdx::string_helper::to_string(cGpsPoint.lng()))
			);

			if (!cGpsPoint.name().empty())
				xmlWriter.tag("gpx:name").content(cGpsPoint.name());

			if (!cGpsPoint.comment().empty())
				xmlWriter.tag("gpx:desc").cdata(cGpsPoint.comment());

			SAXWriter::Tag tagExtensions = xmlWriter.tag("gpx:extensions");
			SAXWriter::Tag tagWptExtension = xmlWriter.tag("gpxd:WptExtension");
			xmlWriter.tag("gpxd:WptUse").attribute("Use", it == (cGpsRoute.end() - 1) ? "destination" : "attractor").attribute("LastWpt", "false").content(std::string());
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}

int WriteDaimlerUbxGPX(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();

		// Header
		SAXWriter::Tag tagGpx = std::move(xmlWriter.tag("gpx:gpx")
			.attribute("version", "1.1")
			.attribute("creator", SOFT_FULL_NAME " (" SOFT_URL ")")
			.attribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance")
			.attribute("xmlns:gpx", "http://www.topografix.com/GPX/1/1")
			.attribute("xmlns:gpxd", "http://www.daimler.com/DaimlerGPXExtensions/V2.4")
			.attribute("xsi:schemaLocation", "http://www.topografix.com/GPX/1/1 GPX-Schema_Topografix.xsd")
		);

		SAXWriter::Tag tagRte = xmlWriter.tag("gpx:rte");
		xmlWriter.tag("gpx:name").content(cGpsRoute.name());

		xmlWriter.tag("gpx:extensions").tag("gpxd:RteExtension").tag("gpxd:RouteIconId").attribute("IconId", "13");

		for (const CGpsPoint& cGpsPoint : cGpsRoute)
		{
			SAXWriter::Tag tagRtept = std::move(xmlWriter.tag("gpx:rtept")
				.attribute("lat", stdx::string_helper::to_string(cGpsPoint.lat()))
				.attribute("lon", stdx::string_helper::to_string(cGpsPoint.lng()))
				.content(std::string())
			);

			if (!cGpsPoint.name().empty())
				xmlWriter.tag("gpx:name").content(cGpsPoint.name());

			if (!cGpsPoint.comment().empty())
				xmlWriter.tag("gpx:desc").cdata(cGpsPoint.comment());
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
