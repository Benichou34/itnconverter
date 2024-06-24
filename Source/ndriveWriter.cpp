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
 * Purpose : Writer for NDrive file (.kml)
 *           http://www.ndrive.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"

#define XML_XML_HEADER     "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" CRLF
#define XML_KML_HEADER     "<kml xmlns=\"http://earth.google.com/kml/2.2\">" CRLF
#define XML_KML_END        "</kml>" CRLF

#define XML_NAME           "<name><![CDATA[%s]]></name>" CRLF
#define XML_DESC           "<description><![CDATA[Generated by <a href=\"" SOFT_URL "\">" SOFT_FULL_NAME "</a>.]]></description>" CRLF
#define XML_VISIBILTY      "<visibility minScale=\"20\" maxScale=\"50000\">1</visibility>" CRLF

#define XML_DOCU_HEADER    "<Document timestamp=\"%d\">" CRLF
#define XML_DOCU_END       "</Document>" CRLF
#define XML_FOLDER_HEADER  "<Folder icon=\"my_itinerary\">" CRLF
#define XML_FOLDER_END     "</Folder>" CRLF
#define XML_PCMK_HEADER    "<Placemark>" CRLF
#define XML_PCMK_END       "</Placemark>" CRLF

#define XML_POINT_STYLE    "<Style>" CRLF "<IconStyle>" CRLF "<Icon>" CRLF "<href>itinerary_waypoint</href>" CRLF "</Icon>" CRLF "</IconStyle>" CRLF "</Style>" CRLF
#define XML_POINT          "<Point>" CRLF "<coordinates>%f,%f</coordinates>" CRLF "</Point>" CRLF

#define XML_PLACEMARK      XML_PCMK_HEADER XML_NAME XML_POINT_STYLE XML_POINT XML_PCMK_END

int WriteNDRIVE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	CGpsRoute::const_iterator it;

	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	// Write header
	ofsFile << XML_XML_HEADER << XML_KML_HEADER << stdx::format(XML_DOCU_HEADER)(time(nullptr)) << XML_FOLDER_HEADER;
	ofsFile << stdx::format(XML_NAME)(cGpsRoute.name()) << XML_DESC << XML_VISIBILTY;

	// Write points
	for (it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
		ofsFile << stdx::format(XML_PLACEMARK)(it->name())(it->lng())(it->lat());

	// Write end of tag
	ofsFile << XML_FOLDER_END << XML_DOCU_END << XML_KML_END;

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}