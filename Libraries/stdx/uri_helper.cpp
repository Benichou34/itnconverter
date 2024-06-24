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

#include "uri_helper.h"
#include <sstream>

namespace
{
	const unsigned short default_http_port = 80;
	const unsigned short default_https_port = 443;
}

namespace stdx
{
	/*
	Code tout sauf les caractères suivant:
	A à Z
	a à z
	0 à 9
	- _ . ! ~ * ' ( )
	*/
	std::string uri_helper::encode_component(const std::string& strUrl)
	{
		return escape(strUrl, "-_.!~*'()");
	}

	std::string uri_helper::decode_component(const std::string& strUrl)
	{
		return unescape(strUrl);
	}

	/*
	Code tout sauf les caractères suivant:
	A à Z
	a à z
	0 à 9
	- _ . ! ~ * ' ( )
	, / ? : @ & = + $ #
	*/
	std::string uri_helper::encode(const std::string& strUrl)
	{
		return escape(strUrl, ",/?:@&=+$#-_.!~*'()");
	}

	std::string uri_helper::decode(const std::string& strUrl)
	{
		return unescape(strUrl);
	}

	/*
	Code tout sauf les caractères suivant:
	A à Z
	a à z
	0 à 9
	*/
	std::string uri_helper::escape(const std::string& strUrl, const std::string& strFilter)
	{
		std::ostringstream ssResult;

		for (std::string::const_iterator it = strUrl.begin(); it != strUrl.end(); ++it)
		{
			if (isalnum(static_cast<int>(*it & 0xFF)) || strFilter.find(*it) != std::string::npos)
				ssResult << *it;
			else
				ssResult << "%" << std::uppercase << std::hex << static_cast<int>(*it & 0xFF);
		}

		return ssResult.str();
	}

	std::string uri_helper::unescape(const std::string& strUrl)
	{
		std::string strResult;
		size_t pos = 0;

		do
		{
			if (strUrl[pos] == '%')
			{
				int iHex;

				pos++;
				std::istringstream iss(strUrl.substr(pos, 2));
				iss >> std::hex >> iHex;

				strResult += static_cast<char>(iHex);
				pos += 2;
			}
			else
			{
				strResult += strUrl[pos];
				pos++;
			}
		} while (pos < strUrl.size());

		return strResult;
	}

	void url_helper::split(const std::string& strUrl, std::string& strHost, unsigned short& port, std::string& strRequest, bool& bSecure)
	{
		strHost = strUrl;
		port = default_http_port;
		bSecure = false;

		size_t pos = strHost.find("http://");
		if (pos != std::string::npos)
		{
			strHost.erase(0, pos + 7);
		}
		else
		{
			pos = strHost.find("https://");
			if (pos != std::string::npos)
			{
				strHost.erase(0, pos + 8);
				port = default_https_port;
				bSecure = true;
			}
		}

		pos = strHost.find('/');
		if (pos != std::string::npos)
		{
			strRequest = strHost.substr(pos + 1);
			strHost.resize(pos);
		}

		pos = strHost.find(':');
		if (pos != std::string::npos)
		{
			port = static_cast<unsigned short>(std::stoi(strHost.substr(pos + 1)));
			strHost.resize(pos);
		}
	}
	/*
	std::string url_helper::extractBaseUrl(const std::string& strUrl)
	{
		size_t pos = strUrl.find("//");
		if (pos != std::string::npos)
			pos += 2;
		else
			pos = 0;

		std::string strTmp = strUrl.substr(pos);

		return strTmp.substr(0, strTmp.find('/'));
	}
	*/
}
