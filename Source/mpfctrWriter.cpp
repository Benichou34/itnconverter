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
 * Purpose : Writer for MapFactor GPS Navigation (.xml)
 *           http://navigatorfree.mapfactor.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "SAXParser/SAXWriter.h"
#include "mpfctrHeader.h"

int WriteMPFCTR(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration();
		SAXWriter::Tag tagRoute = xmlWriter.tag(MapFactor::MK_ROUTE);
		SAXWriter::Tag tagSet = xmlWriter.tag(MapFactor::MK_SET);

		xmlWriter.tag(MapFactor::MK_NAME).content(cGpsRoute.name());

		CGpsRoute::const_iterator itFirstElement = cGpsRoute.begin();
		CGpsRoute::const_iterator itLastElement = cGpsRoute.end();
		--itLastElement;

		for (CGpsRoute::const_iterator it = itFirstElement; it != cGpsRoute.end(); ++it)
		{
			const CGpsPoint& cGpsPoint = *it;
			std::string strTag;

			if (it == itFirstElement)
				strTag = MapFactor::MK_DEPARTURE;
			else if (it == itLastElement)
				strTag = MapFactor::MK_DESTINATION;
			else
				strTag = MapFactor::MK_WAYPOINT;

			SAXWriter::Tag tagPoint = xmlWriter.tag(strTag);

			xmlWriter.tag(MapFactor::MK_NAME).content(cGpsPoint.name());
			xmlWriter.tag(MapFactor::MK_LATITUDE).content(stdx::string_helper::to_string<int>(static_cast<int>(cGpsPoint.lat() * MapFactor::COORDS_FACTOR)));
			xmlWriter.tag(MapFactor::MK_LONGITUDE).content(stdx::string_helper::to_string<int>(static_cast<int>(cGpsPoint.lng() * MapFactor::COORDS_FACTOR)));
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
