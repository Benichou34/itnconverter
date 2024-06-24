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
 * Purpose : Send coordinates to MioMore Desktop, if available
 */

#include "stdafx.h"
#include "Storage/Registry.h"
#include "stdx/uri_helper.h"
#include "miomore.h"
#include "ITN Tools.h"

namespace
{
	void getLocation(CSendToGps::E_TYPE eType, std::wstring& strLocation)
	{
		strLocation.clear();

		if (eType == CSendToGps::E_TYPE_POINT)
		{
			try
			{
				const storage::Registry regStorage(HKEY_CLASSES_ROOT);
				strLocation = regStorage(_T("miomoredesktop\\shell\\open\\command"))[_T("")];
			}
			catch (storage::Exception&)
			{
				try
				{
					const storage::Registry regStorage(HKEY_LOCAL_MACHINE);
					strLocation = regStorage(_T("SOFTWARE\\Classes\\miomoredesktop\\shell\\open\\command"))[_T("")];
				}
				catch (storage::Exception&)
				{
				}
			}
		}
	}

	void sendTo(CSendToGps::E_TYPE eType, const std::wstring& strParams)
	{
		std::wstring strLocation;
		getLocation(eType, strLocation);

#if defined(_DEBUG) || defined(FORCE_LOG)
		CITNConverterApp::wlog() << clock() << L": sendTo(" << eType << L", " << strParams << L")" << std::endl;
#endif //_DEBUG

		if (!strLocation.empty())
		{
			size_t pos = strLocation.find(_T("%1"));
			strLocation.replace(pos, 2, strParams);

			STARTUPINFO startupInfo = { 0 };
			PROCESS_INFORMATION processInformation;

			CreateProcess(nullptr,
				const_cast<wchar_t*>(strLocation.c_str()),
				nullptr,
				nullptr,
				false,
				0,
				nullptr,
				nullptr,
				&startupInfo,
				&processInformation);
		}
	}
}

bool CMioMoreDesktop::IsAvailabled(CSendToGps::E_TYPE eType)
{
	std::wstring strLocation;
	getLocation(eType, strLocation);
	return !strLocation.empty();
}

void CMioMoreDesktop::SendTo(const CGpsPoint& cGpsPoint)
{
	std::ostringstream ssParam;
	ssParam << "miomoredesktop:poi?name=" << stdx::uri_helper::escape(cGpsPoint.name()) << "&province=&city=&street=&streetnum=&postalcode=&country=&phone=&notes=" << stdx::uri_helper::escape(cGpsPoint.comment()) << "&lat=" << cGpsPoint.lat() << "&lon=" << cGpsPoint.lng();

	sendTo(CSendToGps::E_TYPE_POINT, stdx::string_helper::widen(ssParam.str()));
}
