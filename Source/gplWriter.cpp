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
 * Purpose : Writer for Medion GoPal file (.xml)
 *           www.mediongopal.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"

#define XML_XML_HEADER	"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\"?>"
#define XML_TOUR_HEADER	"<tour><options VehicleSpeedNonMotorway=\"27\" VehicleSpeedMotorway=\"33\" VehicleSpeedInPedestrianArea=\"2\" Type=\"3\" Tunnels=\"1\" TollRoad=\"1\" TTIMode=\"0\" PedestrianSpeed=\"1\" MotorWays=\"0\" Mode=\"2\" Ferries=\"1\" CyclistSpeed=\"4\"/>"
#define XML_TOUR_END	"</tour>"
#define XML_STEP		"<dest Longitude=\"%d\" Latitude=\"%d\" Country=\"0\" City=\"%s\"/>"

int WriteGPL(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	ofsFile << XML_XML_HEADER << std::endl;
	ofsFile << XML_TOUR_HEADER << std::endl;

	for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
	{
		const CGpsPoint& cGpsPoint = *it;

		geo::CGeoMercatorXY gMercatorXY(cGpsPoint);
		ofsFile << stdx::format(XML_STEP)(gMercatorXY.x())(gMercatorXY.y())(stdx::string_helper::from_utf8(cGpsPoint.name())) << std::endl;
	}

	ofsFile << XML_TOUR_END << std::endl;

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
