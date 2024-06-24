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
 * Purpose : Writer for Here Maps
 *           https://www.here.com
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include "GeoServices/GeoServices.h"

namespace
{
	constexpr size_t MAX_HERE_STEPS = 10;
}

int WriteHERE(const std::wstring&, const CGpsRoute& cGpsRoute, DWORD dwFlag, bool)
{
	std::string strUrl;

	std::unique_ptr<geo::CGeoRouteOptions> cgOptions;
	geo::GeoVehicleType::type_t vehicleType = FillRouteOptionsWithSettings(cgOptions);

	CGpsRoute::const_iterator itEnd = cGpsRoute.begin();
	if (cGpsRoute.size() > MAX_HERE_STEPS)
		std::advance(itEnd, MAX_HERE_STEPS);
	else
		itEnd = cGpsRoute.end();

	geo::CHereUrl::LocationsToUrl(geo::CGeoLocations(cGpsRoute.begin(), itEnd), strUrl, vehicleType, *cgOptions, (dwFlag != 0));

	if (strUrl.size())
		ShellExecuteA(nullptr, "open", strUrl.c_str(), nullptr, nullptr, SW_SHOWNORMAL);

	return S_OK;
}
