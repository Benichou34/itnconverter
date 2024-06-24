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
 * Purpose : Writer for Falk Navigator file (.tour)
 *           www.gofalk.com
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"

#define TOUR_HEADER "[TOUR]" CRLF "Name = %s" CRLF "Creator = Generated by " SOFT_FULL_NAME " (" SOFT_URL ")" CRLF
#define TOUR_STEP "[%d]" CRLF "Name = %s" CRLF "PositionInList = %d" CRLF "Longitude = %d" CRLF "Latitude = %d" CRLF "Class = FMI.FalkNavigator.PersistentAddress" CRLF "Assembly = FalkNavigator" CRLF "Visited = 0" CRLF

int WriteFLK(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::ofstream ofsFile(strPathName, std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	ofsFile << stdx::format(TOUR_HEADER)(stdx::string_helper::from_utf8(cGpsRoute.name())) << std::endl;

	for (size_t i = 0; i < cGpsRoute.size(); i++)
	{
		const CGpsPoint& cGpsPoint = cGpsRoute[i];

		geo::CGeoMercatorXY gMercatorXY(cGpsPoint);
		ofsFile << stdx::format(TOUR_STEP)(i)(stdx::string_helper::from_utf8(cGpsPoint.name()))(i)(gMercatorXY.x())(gMercatorXY.y()) << std::endl;
	}

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}
