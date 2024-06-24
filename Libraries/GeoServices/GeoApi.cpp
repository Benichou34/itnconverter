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

#include "GeoApi.h"
#include <system_error>

namespace geo
{
	CGeoProviders& CGeoProviders::instance()
	{
		static CGeoProviders geoProviders;
		return geoProviders;
	}

	CGeoProviders::CGeoProviders() :
		m_defaultProvider(geo::E_GEO_PROVIDER_INTERNAL)
	{
	}
		
	const CGeoProvider& CGeoProviders::get(E_GEO_PROVIDER provider) const
	{
		auto it = m_geoProviders.find(provider);
		if (it == m_geoProviders.end())
			throw std::invalid_argument("Not valid provider");
		return *(it->second);
	}

	CGeoProvider& CGeoProviders::get(E_GEO_PROVIDER provider)
	{
		auto it = m_geoProviders.find(provider);
		if (it == m_geoProviders.end())
			throw std::invalid_argument("Not valid provider");
		return *(it->second);
	}

	CGeoProvider& CGeoProviders::get(E_GEO_PROVIDER provider, const std::nothrow_t&) noexcept
	{
		auto it = m_geoProviders.find(provider);
		if (it != m_geoProviders.end())
			return *(it->second);

		m_geoProviders[provider] = std::make_unique<CGeoProvider>(provider);
		return *m_geoProviders[provider];
	}

	void CGeoProviders::remove(E_GEO_PROVIDER provider)
	{
		m_geoProviders.erase(provider);
	}

	bool CGeoProviders::exist(E_GEO_PROVIDER provider) const noexcept
	{
		return m_geoProviders.find(provider) != m_geoProviders.end();
	}

	E_GEO_PROVIDER CGeoProviders::getDefaultProvider() const noexcept
	{
		return m_defaultProvider;
	}

	E_GEO_PROVIDER CGeoProviders::setDefaultProvider(E_GEO_PROVIDER provider)
	{
		if (!exist(provider))
			throw std::invalid_argument("Not valid provider");

		return setDefaultProvider(provider, std::nothrow);
	}

	E_GEO_PROVIDER CGeoProviders::setDefaultProvider(E_GEO_PROVIDER provider, const std::nothrow_t&) noexcept
	{
		E_GEO_PROVIDER oldProvider = m_defaultProvider;
		m_defaultProvider = provider;
		return oldProvider;
	}
}
