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

#ifndef _JSPROVIDERINFOS_H_INCLUDED_
#define _JSPROVIDERINFOS_H_INCLUDED_

#include "GeoServices/GeoApi.h"
#include <map>

class jsProviderInfos
{
public:
	jsProviderInfos(geo::E_GEO_PROVIDER provider);
	~jsProviderInfos() = default;

	geo::E_GEO_PROVIDER getProvider() const noexcept { return m_provider; }
	bool isAvailable() const noexcept { return m_bAvailable; }
	const std::string& getKey() const noexcept { return m_strKey; }
	const std::string& getLang() const noexcept { return m_strLang; }
	const std::string& getReferer() const noexcept { return m_strReferer; }

	bool operator== (const jsProviderInfos& providerInfos) const;
	bool operator!= (const jsProviderInfos& providerInfos) const;

private:
	friend class jsProvidersInfos;

	void setAvailable(bool bAvailable) noexcept { m_bAvailable = bAvailable; }
	void setKey(const std::string& strKey) noexcept { m_strKey = strKey; }
	void setLang(const std::string& strLang) noexcept { m_strLang = strLang; }
	void setReferer(const std::string& strReferer) noexcept { m_strReferer = strReferer; }

private:
	geo::E_GEO_PROVIDER m_provider;
	bool m_bAvailable;
	std::string m_strKey;
	std::string m_strLang;
	std::string m_strReferer;
};

class jsProvidersInfos : public std::map<geo::E_GEO_PROVIDER, jsProviderInfos>
{
public:
	bool fromJs(const std::string& strContent);
};

#endif // _JSPROVIDERINFOS_H_INCLUDED_
