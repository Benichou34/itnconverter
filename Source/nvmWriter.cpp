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
 * Purpose : Writer for Navman XML file (.xml)
 *           www.navman.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "SAXParser/SAXWriter.h"
#include <fstream>

namespace
{
	inline long nvmDoubleToLong(double dDouble)
	{
		return static_cast<long>(dDouble * 100000);
	}

	void writeLocation(SAXWriter& xmlWriter, const CGpsPoint& cGpsPoint, const std::string& strType)
	{
		SAXWriter::Tag tagItem = std::move(xmlWriter.tag("Item").attribute("class", "Location"));

		xmlWriter.tag("name").content(cGpsPoint.name());
		xmlWriter.tag("name2").content("");
		xmlWriter.tag("lat").content(stdx::string_helper::to_string(nvmDoubleToLong(cGpsPoint.lat())));
		xmlWriter.tag("long").content(stdx::string_helper::to_string(nvmDoubleToLong(cGpsPoint.lng())));
		xmlWriter.tag("entryName").content("");
		xmlWriter.tag("entryLat").content(stdx::string_helper::to_string(nvmDoubleToLong(cGpsPoint.lat())));
		xmlWriter.tag("entryLong").content(stdx::string_helper::to_string(nvmDoubleToLong(cGpsPoint.lng())));
		xmlWriter.tag("entryHeading").content("-1");
		xmlWriter.tag("entrySideOfLine").content("0");
		xmlWriter.tag("entryHouseNo").content("0");
		xmlWriter.tag("entryPostcode").content("");
		xmlWriter.tag("entryResolved").content("1");
		xmlWriter.tag("entryValid").content("1");
		xmlWriter.tag("entryMaxRoad").content("0");
		xmlWriter.tag("entryHalfSize").content("0");
		xmlWriter.tag("source").content("5");
		xmlWriter.tag("resultType").content("9");
		xmlWriter.tag("photo").content("");
		xmlWriter.tag("typeName").content("");
		xmlWriter.tag("phoneNum").content("");
		xmlWriter.tag("type").content(strType);
	}
}

int WriteNVM(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();

		SAXWriter::Tag tagMultistopLocations1 = xmlWriter.tag("MultistopLocations");
		SAXWriter::Tag tagMultistopLocations2 = std::move(xmlWriter.tag("MultistopLocations").attribute("class", "LocationList"));

		writeLocation(xmlWriter, CGpsPoint(), "road");
		for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
			writeLocation(xmlWriter, *it, "area");
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
