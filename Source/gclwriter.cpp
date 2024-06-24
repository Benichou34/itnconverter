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
 * Purpose : Writer for GoClever/Mireo viaGPS 3.0 file (.txt)
 *           www.goclever.net / www.mireo.hr
 */

#include "stdafx.h"
#include <fstream>
#include "ITN Tools.h"
#include "stdx/string_helper.h"
#include "stdx/bom.h"
#include "stdx/format.h"

namespace
{
	const std::string GCL_NAME("#NAME=\"%s\"\n");
	const std::string GCL_LINE("STREET=\"%s\" PT=\"%.5f %.5f\"\n");
	const std::wstring GCL_PREFIX(L"Route ");

	int WriteGoClever(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, bool bName)
	{
		std::ofstream ofsFile(strPathName, std::ios_base::binary);
		if (!ofsFile)
			return S_FALSE;

		stdx::bom::write(ofsFile, stdx::bom::utf_8); // BOM

		// Write Name
		if (bName)
			ofsFile << stdx::format(GCL_NAME)(cGpsRoute.name());

		// Write Points
		for (CGpsRoute::const_iterator it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
		{
			const CGpsPoint& cGpsPoint = *it;
			ofsFile << stdx::format(GCL_LINE)(cGpsPoint.name())(cGpsPoint.lng())(cGpsPoint.lat());
		}

		ofsFile.close();
		return ofsFile.good() ? S_OK : S_FALSE;
	}
}

int WriteGCL(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::wstring strFileName = CWToolsString::FileName(strPathName);
	std::wstring newFileName = strPathName;

	if (strFileName.compare(0, GCL_PREFIX.size(), GCL_PREFIX) != 0)
		newFileName = CWToolsString::FileDirectory(strPathName) + GCL_PREFIX + strFileName;

	return WriteGoClever(newFileName, cGpsRoute, false);
}

int WriteRT(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	return WriteGoClever(strPathName, cGpsRoute, true);
}