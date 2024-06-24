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
 * Purpose : Writer for IGN Rando file (.rdn)
 *           www.ignrando.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "SAXParser/SAXWriter.h"

int WriteRDN(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	SYSTEMTIME sSystemTime;
	GetSystemTime(&sSystemTime);

	{
		SAXWriter xmlWriter(ofsFile);
		xmlWriter.declaration("windows-1252");

		// Header
		SAXWriter::Tag tagRando = xmlWriter.tag("RANDONNEE");
		{
			SAXWriter::Tag tagEntete = xmlWriter.tag("ENTETE");
			xmlWriter.tag("VERSION_XML").content("1.1");
			xmlWriter.tag("VERSION_BASE").content("IHA03AA");
			xmlWriter.tag("DATE").content(stdx::format("%02d/%02d/%04d")(sSystemTime.wDay)(sSystemTime.wMonth)(sSystemTime.wYear));
			xmlWriter.tag("HEURE").content(stdx::format("%02d:%02d:%02d")(sSystemTime.wHour)(sSystemTime.wMinute)(sSystemTime.wSecond));
		}
		{
			SAXWriter::Tag tagInfos = xmlWriter.tag("INFORMATIONS");
			xmlWriter.tag("NB_ETAPES").content(stdx::string_helper::to_string(cGpsRoute.size()));
			xmlWriter.tag("DESCRIPTION").content(stdx::string_helper::from_utf8(cGpsRoute.name()));
		}

		for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
		{
			const CGpsPoint& cGpsPoint = *it;

			SAXWriter::Tag tagEtape = xmlWriter.tag("ETAPE");

			xmlWriter.tag("POSITION").content(stdx::format("%f,%f")(cGpsPoint.lat())(cGpsPoint.lng()));
			xmlWriter.tag("ALTITUDE").content(stdx::string_helper::to_string(cGpsPoint.alt()));

			if (!cGpsPoint.name().empty())
				xmlWriter.tag("DESCRIPTION_ETAPE").content(stdx::string_helper::from_utf8(cGpsPoint.name()));
		}
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
