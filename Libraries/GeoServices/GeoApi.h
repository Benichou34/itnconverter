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

#ifndef _GEO_API_H_INCLUDED_
#define _GEO_API_H_INCLUDED_

#include <cstddef>
#include <set>
#include <map>
#include <string>
#include <memory>

namespace geo
{
	typedef enum
	{
		E_GEO_OK,                     // indicates that no errors occurred; the address was successfully parsed and at least one geocode was returned.
		E_GEO_ZERO_RESULTS,           // indicates that the search was successful but returned no results.
		E_GEO_OVER_QUERY_LIMIT,       // indicates that you are over your quota.
		E_GEO_REQUEST_DENIED,         // indicates that your request was denied, generally because of lack of a sensor parameter.
		E_GEO_INVALID_REQUEST,        // generally indicates that the query is missing.
		E_GEO_NOT_FOUND,              // indicates at least one of the locations specified in the requests's origin, destination, or waypoints could not be geocoded.
		E_GEO_MAX_WAYPOINTS_EXCEEDED, // indicates that too many waypointss were provided in the request The maximum allowed waypoints is 8, plus the origin, and destination.
		E_GEO_UNKNOWN_ERROR,          // indicates a directions request could not be processed due to a server error. The request may succeed if you try again.
		E_GEO_BAD_ARGUMENTS,
		E_GEO_TIMEOUT,                // 

		E_HTTP_ERROR = 100            // Windows HTTP errors
	} E_GEO_STATUS_CODE;

	typedef enum
	{
		E_GEO_PROVIDER_INTERNAL,
		E_GEO_PROVIDER_FREE_1, // Free place
		E_GEO_PROVIDER_GOOGLE_API,
		E_GEO_PROVIDER_BING_API,
		E_GEO_PROVIDER_CLOUDMADE_API,
		E_GEO_PROVIDER_TOMTOM_API,
		E_GEO_PROVIDER_TOMTOM_ROUTING_API,
		E_GEO_PROVIDER_TOMTOM_SEARCH_API,
		E_GEO_PROVIDER_GEONAMES_API,
		E_GEO_PROVIDER_VIAMICHELIN_API,
		E_GEO_PROVIDER_OSRM_API,
		E_GEO_PROVIDER_GEOPORTAL_API,
		E_GEO_PROVIDER_VTRAFIC,
		E_GEO_PROVIDER_OSM_MAP,
		E_GEO_PROVIDER_USGC_MAP,
		E_GEO_PROVIDER_NRCAN_MAP,
		E_GEO_PROVIDER_ICC_MAP,
		E_GEO_PROVIDER_WAZE_MAP,
		E_GEO_PROVIDER_WAZE_WORLD_MAP,
		E_GEO_PROVIDER_HERE_API,
		E_GEO_PROVIDER_ORS_API,
		E_GEO_PROVIDER_CUSTOM
	} E_GEO_PROVIDER;

	typedef enum
	{
		E_GEO_TILE_ROUTE,
		E_GEO_TILE_AERIAL,
		E_GEO_TILE_HYBRID,
		E_GEO_TILE_PHYSICAL,
		E_GEO_TILE_TRAFFIC,
		E_GEO_TILE_OVERLAY
	} E_GEO_TILE_TYPE;

	class CGeoProvider
	{
	public:
		CGeoProvider(E_GEO_PROVIDER provider) : m_provider(provider) {}
		virtual ~CGeoProvider() = default;
		
		E_GEO_PROVIDER getProvider() const noexcept { return m_provider; }

		void setKey(const std::string& strKey) noexcept { m_strKey = strKey; }
		const std::string& getKey() const noexcept { return m_strKey; }

		void setLang(const std::string& strLang) noexcept { m_strLang = strLang; }
		const std::string& getLang() const noexcept { return m_strLang; }

		void setReferer(const std::string& strReferer) noexcept { m_strReferer = strReferer; }
		const std::string& getReferer() const noexcept { return m_strReferer; }

	private:
		E_GEO_PROVIDER m_provider;
		std::string m_strKey;
		std::string m_strLang;
		std::string m_strReferer;
	};

	class CGeoProviders
	{
	public:
		static CGeoProviders& instance();

		const CGeoProvider& get(E_GEO_PROVIDER provider) const;
		CGeoProvider& get(E_GEO_PROVIDER provider);
		CGeoProvider& get(E_GEO_PROVIDER provider, const std::nothrow_t&) noexcept;

		template<class T = CGeoProvider>
		T& add(E_GEO_PROVIDER provider)
		{
			if (exist(provider))
				throw std::invalid_argument("Provider already exist");

			m_geoProviders[provider] = std::make_unique<T>(provider);
			return static_cast<T&>(*m_geoProviders[provider]);
		}

		void remove(E_GEO_PROVIDER provider);

		bool exist(E_GEO_PROVIDER provider) const noexcept;

		E_GEO_PROVIDER getDefaultProvider() const noexcept;
		E_GEO_PROVIDER setDefaultProvider(E_GEO_PROVIDER provider);
		E_GEO_PROVIDER setDefaultProvider(E_GEO_PROVIDER provider, const std::nothrow_t&) noexcept;

		CGeoProviders(const CGeoProviders&) = delete;
		CGeoProviders& operator=(const CGeoProviders&) = delete;

	private:
		CGeoProviders();
		~CGeoProviders() = default;

	private:
		geo::E_GEO_PROVIDER m_defaultProvider;
		std::map<E_GEO_PROVIDER, std::unique_ptr<CGeoProvider>> m_geoProviders;
	};

	typedef std::set<E_GEO_TILE_TYPE> GeoTileTypes;

	const size_t InfiniteTimeOut = static_cast<size_t>(-1);
} // namespace geo

#endif // _GEO_API_H_INCLUDED_
