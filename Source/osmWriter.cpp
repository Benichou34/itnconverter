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
 * Purpose : Writer for OpenStreetMap XML file (.osm)
 *           http://wiki.openstreetmap.org/wiki/OSM_XML
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "SAXParser/SAXWriter.h"

int WriteOSM(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	double dMinLat = 90;
	double dMaxLat = -90;
	double dMinLon = 180;
	double dMaxLon = -180;

	if (!cGpsRoute.empty())
	{
		// Retrieve bounds informations
		for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
		{
			const geo::CGeoLocation& geoLocation = *it;

			if (geoLocation.lat() < dMinLat)
				dMinLat = geoLocation.lat();

			if (geoLocation.lat() > dMaxLat)
				dMaxLat = geoLocation.lat();

			if (geoLocation.lng() < dMinLon)
				dMinLon = geoLocation.lng();

			if (geoLocation.lng() > dMaxLon)
				dMaxLon = geoLocation.lng();
		}
	}

	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();

		// Write header
		SAXWriter::Tag tagOsm = std::move(xmlWriter.tag("osm").attribute("version", "0.6").attribute("generator", SOFT_FULL_NAME " (" SOFT_URL ")"));

		xmlWriter.tag("bounds")
			.attribute("minlat", stdx::string_helper::to_string(dMinLat))
			.attribute("minlon", stdx::string_helper::to_string(dMinLon))
			.attribute("maxlat", stdx::string_helper::to_string(dMaxLat))
			.attribute("maxlon", stdx::string_helper::to_string(dMaxLon));

		// Write nodes
		size_t id = 1;
		for (CGpsRoute::const_iterator cit = cGpsRoute.begin(); cit != cGpsRoute.end(); ++cit, ++id)
		{
			const geo::CGeoLocation& geoLocation = *cit;

			SAXWriter::Tag tagNode = std::move(xmlWriter.tag("node")
				.attribute("id", "-" + stdx::string_helper::to_string(id))
				.attribute("visible", "true")
				.attribute("lat", stdx::string_helper::to_string(geoLocation.lat()))
				.attribute("lon", stdx::string_helper::to_string(geoLocation.lng())));

			if (geoLocation.alt() != 0)
				xmlWriter.tag("tag").attribute("k", "ele").attribute("v", stdx::string_helper::to_string(geoLocation.alt()));

			if (!geoLocation.name().empty())
				xmlWriter.tag("tag").attribute("k", "name").attribute("v", geoLocation.name());

			if (!geoLocation.comment().empty())
				xmlWriter.tag("tag").attribute("k", "note").attribute("v", geoLocation.comment());
		}

		// Write route (way)
		SAXWriter::Tag tagWay = std::move(xmlWriter.tag("way").attribute("id", "-" + stdx::string_helper::to_string(id)).attribute("visible", "true"));

		if (!cGpsRoute.name().empty())
			xmlWriter.tag("tag").attribute("k", "name").attribute("v", cGpsRoute.name());

		for (id = 0; id < cGpsRoute.size();)
			xmlWriter.tag("nd").attribute("ref", "-" + stdx::string_helper::to_string(++id));
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
