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
 */

#include "GeoGeocoderFactory.h"
#include "GoogleApiGeocoder.h"
#include "BingApiGeocoder.h"
#include "TomtomApiGeocoder.h"
#include "ViaMichelinApiGeocoder.h"
#include "WazeMapGeocoder.h"
#include "HereApiGeocoder.h"

using namespace geo;

std::unique_ptr<IGeoGeocoder> CGeoGeocoderFactory::Get(E_GEO_PROVIDER eGeoProvider)
{
	std::unique_ptr<IGeoGeocoder> pGeocoder = Get(eGeoProvider, std::nothrow);
	if (!pGeocoder)
		throw std::invalid_argument("Bad type");

	return pGeocoder;
}

std::unique_ptr<IGeoGeocoder> CGeoGeocoderFactory::Get(E_GEO_PROVIDER eGeoProvider, const std::nothrow_t&) noexcept
{
	switch (eGeoProvider)
	{
	case E_GEO_PROVIDER_GOOGLE_API:
		return std::make_unique<CGoogleApiGeocoder>();

	case E_GEO_PROVIDER_BING_API:
		return std::make_unique<CBingApiGeocoder>();

	case E_GEO_PROVIDER_TOMTOM_API:
	case E_GEO_PROVIDER_TOMTOM_SEARCH_API:
		return std::make_unique<CTomtomApiGeocoder>();

	case E_GEO_PROVIDER_VIAMICHELIN_API:
		return std::make_unique<CViaMichelinApiGeocoder>();

	case E_GEO_PROVIDER_WAZE_MAP:
		return std::make_unique<CWazeUSAMapGeocoder>();

	case E_GEO_PROVIDER_WAZE_WORLD_MAP:
		return std::make_unique<CWazeWorldMapGeocoder>();

	case E_GEO_PROVIDER_HERE_API:
		return std::make_unique<CHereApiGeocoder>();

	default:
		break;
	}

	return std::unique_ptr<IGeoGeocoder>();
}
