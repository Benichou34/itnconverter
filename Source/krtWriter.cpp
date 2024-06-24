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
 * Purpose : Writer for klickTel Routenplaner file (.krt)
 *           www.klicktel.de
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "SAXParser/SAXWriter.h"

int WriteKRT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();

		// Route
		SAXWriter::Tag tagRoute = xmlWriter.tag("pkDRoute");
		{
			SAXWriter::Tag tagStations = xmlWriter.tag("Stations");

			for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
			{
				const CGpsPoint& cGpsPoint = *it;

				std::string strLatitude(stdx::string_helper::to_string(cGpsPoint.lat()));
				std::string strLongitude(stdx::string_helper::to_string(cGpsPoint.lng()));

				stdx::string_helper::replace(strLatitude, '.', ',');
				stdx::string_helper::replace(strLongitude, '.', ',');

				SAXWriter::Tag tagStation = xmlWriter.tag("Station");
				xmlWriter.tag("City").content(cGpsPoint.name());

				SAXWriter::Tag tagPoint = xmlWriter.tag("Point");
				xmlWriter.tag("Latitude").content(strLatitude);
				xmlWriter.tag("Longitude").content(strLongitude);
			}
		}

		xmlWriter.tag("RouteOptions");
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
