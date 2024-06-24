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
 * Purpose : Reader for Polstar PolNav trail file (.trl)
 *           www.polstargps.com
 */

#include "stdafx.h"
#include <fstream>
#include "trlHeader.h"
#include "ITN Tools.h"

int ReadTRL(const std::wstring& strPathName, std::vector<CGpsPointArray*>& vecGpsArray, bool)
{
	std::ifstream ifsFile(strPathName.c_str());
	if (!ifsFile)
		return S_FALSE;

	vecGpsArray.push_back(new CGpsRoute());
	TRL_POINT sTrlPoint;

	while (!ifsFile.eof())
	{
		ifsFile.read(reinterpret_cast<char*>(&sTrlPoint), sizeof(TRL_POINT));
		if (ifsFile.gcount() == sizeof(TRL_POINT))
		{
			swap_endian(sTrlPoint.dSpeed);
			swap_endian(sTrlPoint.dALtitude);

			CGpsPoint cGpsPoint;

			cGpsPoint.lng(sTrlPoint.nLongitude / TRL_FACTOR);
			cGpsPoint.lat(sTrlPoint.nLatitude / TRL_FACTOR);
			cGpsPoint.alt(sTrlPoint.dALtitude);
			cGpsPoint.name(stdx::string_helper::to_string(vecGpsArray.back()->size()));

			vecGpsArray.back()->push_back(cGpsPoint);
		}
	}

	return S_OK;
}
