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

#include "hierarchical_uri.h"
#include "string_helper.h"

namespace
{
	constexpr char path_separator = '/';
	constexpr char user_separator = '@';
	constexpr char colon_separator = ':';
	constexpr char query_separator = '?';
	constexpr char fragment_separator = '#';

	constexpr std::string_view query_value_delimiters = "&;";
	constexpr char key_value_delimiter = '=';

	constexpr std::string_view authority_prefix = "//";

	// https://en.wikipedia.org/wiki/List_of_URI_schemes
	constexpr struct
	{
		std::string_view str_scheme;
		stdx::uri::scheme_t scheme;
		bool secure;
	} scheme_desc[] =
	{
		{"urn"   , stdx::uri::scheme_t::urn   , false},
		{"dns"   , stdx::uri::scheme_t::dns   , false},
		{"file"  , stdx::uri::scheme_t::file  , false},
		{"ftp"   , stdx::uri::scheme_t::ftp   , false},
		{"ftps"  , stdx::uri::scheme_t::ftp   , true },
		{"http"  , stdx::uri::scheme_t::http  , false},
		{"https" , stdx::uri::scheme_t::http  , true },
		{"coap"  , stdx::uri::scheme_t::coap  , false},
		{"coaps" , stdx::uri::scheme_t::coap  , true },
		{"mailto", stdx::uri::scheme_t::mailto, false},
		{"tel"   , stdx::uri::scheme_t::tel   , false},
		{"sms"   , stdx::uri::scheme_t::sms   , false},
		{"ssh"   , stdx::uri::scheme_t::ssh   , false},
		{"sftp"  , stdx::uri::scheme_t::sftp  , true },
		{"shttp" , stdx::uri::scheme_t::shttp , true }
	};
}

namespace stdx
{
	std::error_code uri::parse_uri(const std::string_view& uri,
		std::string_view& scheme,
		std::string_view& left) noexcept
	{
		size_t startPos = 0;
		scheme = {};
		left = {};

		// Searching scheme
		const size_t pos = uri.find(colon_separator, startPos);
		if (pos == std::string_view::npos)
			return std::make_error_code(std::errc::wrong_protocol_type);

		scheme = uri.substr(startPos, pos - startPos);
		startPos = pos + sizeof(colon_separator);

		left = uri.substr(startPos);
		return std::error_code();
	}

	std::error_code uri::parse_uri(const std::string_view& uri,
		std::string_view& scheme,
		std::string_view& authority,
		std::string_view& left) noexcept
	{
		size_t startPos = 0;
		std::string_view uri_left;
		authority = {};
		left = {};

		std::error_code ec = parse_uri(uri, scheme, uri_left);
		if (ec)
			return ec;

		// Searching authority
		if (uri_left.compare(startPos, authority_prefix.size(), authority_prefix) == 0)
		{
			startPos += authority_prefix.size();
			const size_t pos = uri_left.find(path_separator, startPos);
			if (pos == std::string_view::npos)
			{
				authority = uri_left.substr(startPos);
				return std::error_code();
			}

			authority = uri_left.substr(startPos, pos - startPos);
			startPos = pos;
		}

		left = uri_left.substr(startPos);
		return std::error_code();
	}

	std::error_code uri::parse_uri(const std::string_view& uri,
		std::string_view& scheme,
		std::string_view& authority,
		std::string_view& path,
		std::string_view& left) noexcept
	{
		size_t startPos = 0;
		std::string_view uri_left;
		path = {};
		left = {};

		std::error_code ec = parse_uri(uri, scheme, authority, uri_left);
		if (ec)
			return ec;

		// Searching Path
		const size_t pos = uri_left.find(query_separator, startPos);
		if (pos == std::string_view::npos)
		{
			path = uri_left.substr(startPos);
			return std::error_code();
		}

		path = uri_left.substr(startPos, pos - startPos);
		startPos = pos + sizeof(query_separator);

		left = uri_left.substr(startPos);
		return std::error_code();
	}

	std::error_code uri::parse_uri(const std::string_view& uri,
		std::string_view& scheme,
		std::string_view& authority,
		std::string_view& path,
		std::string_view& query,
		std::string_view& fragment) noexcept
	{
		size_t startPos = 0;
		std::string_view uri_left;
		query = {};
		fragment = {};

		std::error_code ec = parse_uri(uri, scheme, authority, path, uri_left);
		if (ec)
			return ec;

		// Searching Query
		const size_t pos = uri_left.find(fragment_separator, startPos);
		if (pos == std::string_view::npos)
		{
			query = uri_left.substr(startPos);
			return std::error_code();
		}

		query = uri_left.substr(startPos, pos - startPos);
		startPos = pos + sizeof(fragment_separator);

		// Fragment
		fragment = uri_left.substr(startPos);
		return std::error_code();
	}

	std::error_code uri::parse_scheme(const std::string_view& scheme,
		scheme_t& type,
		bool& secure) noexcept
	{
		for (const auto& sd : scheme_desc)
		{
			if (scheme == sd.str_scheme)
			{
				type = sd.scheme;
				secure = sd.secure;

				return std::error_code();
			}
		}

		type = scheme_t::unknown;
		secure = false;

		return std::error_code();
	}

	std::error_code uri::parse_authority(const std::string_view& authority,
		std::string_view& user,
		std::string_view& password,
		std::string_view& host,
		uint16_t& port) noexcept
	{
		size_t startPos = 0;
		user = {};
		password = {};
		host = {};
		port = {};

		// Searching user/password
		size_t pos = authority.find(user_separator, startPos);
		if (pos != std::string_view::npos)
		{
			startPos = pos + sizeof(user_separator);

			std::string_view userinfo = authority.substr(0, pos);
			pos = userinfo.find(colon_separator);
			if (pos != std::string_view::npos)
			{
				user = userinfo.substr(0, pos);
				password = userinfo.substr(pos + sizeof(colon_separator));
			}
			else
			{
				user = userinfo;
			}
		}

		// Searching host
		pos = authority.find(colon_separator, startPos);
		if (pos == std::string_view::npos)
		{
			host = authority.substr(startPos);
			if (host.empty())
				return std::make_error_code(std::errc::wrong_protocol_type);

			return std::error_code();
		}

		host = authority.substr(startPos, pos - startPos);
		startPos = pos + sizeof(colon_separator);

		// Port
		const std::string_view svport = authority.substr(startPos);
		if (!svport.empty())
			port = static_cast<uint16_t>(strtoul(svport.data(), 0, 10));

		return std::error_code();
	}

	std::error_code uri::parse_path(const std::string_view& path,
		std::vector<std::string_view>& paths)
	{
		stdx::string_view_helper::split(path, paths, stdx::find_first(path_separator), false);
		return std::error_code();
	}

	std::error_code uri::parse_query(const std::string_view& query,
		std::unordered_map<std::string_view, std::string_view>& queries)
	{
		stdx::string_view_helper::split(query, queries, stdx::find_first_of(query_value_delimiters), stdx::find_first(key_value_delimiter), true);
		return std::error_code();
	}
}
