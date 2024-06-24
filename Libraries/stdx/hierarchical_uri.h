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

#ifndef STDX_HIERARCHICAL_URI_PARSER_H_
#define STDX_HIERARCHICAL_URI_PARSER_H_

#include <system_error>
#include <string_view>
#include <vector>
#include <unordered_map>

// https://en.wikipedia.org/wiki/Uniform_Resource_Identifier
// https://tools.ietf.org/html/rfc3986
/*
     foo://example.com:8042/over/there?tag=networking&order=newest#top
     \_/   \______________/\_________/ \_________________________/ \_/
      |           |            |                   |                |
   scheme     authority       path               query           fragment

	URI = scheme ":" ["//" authority] path ["?" query] ["#" fragment]
	authority = [userinfo "@"] host [":" port]
*/

namespace stdx
{
	struct uri
	{
		enum class scheme_t
		{
			unknown,
			urn, // Uniform Resource Names
			dns, //	Domain Name System
			file, // Addressing files on local or network file systems
			ftp, // FTP resources
			http, // HTTP resources
			coap, // Constrained Application Protocol
			mailto, // SMTP e - mail addresses and default content
			tel, //	Used for telephone numbers
			sms, // Interact with SMS capable devices for composingand sending messages
			ssh, // SSH connections
			sftp, // SFTP file transfers(not be to confused with FTPS(FTP / SSL))
			shttp // Secure HTTP
		};

		static std::error_code parse_uri(const std::string_view& uri,
			std::string_view& scheme,
			std::string_view& left) noexcept;

		static std::error_code parse_uri(const std::string_view& uri,
			std::string_view& scheme,
			std::string_view& authority,
			std::string_view& left) noexcept;

		static std::error_code parse_uri(const std::string_view& uri,
			std::string_view& scheme,
			std::string_view& authority,
			std::string_view& path,
			std::string_view& left) noexcept;

		static std::error_code parse_uri(const std::string_view& uri,
			std::string_view& scheme,
			std::string_view& authority,
			std::string_view& path,
			std::string_view& query,
			std::string_view& fragment) noexcept;

		static std::error_code parse_scheme(const std::string_view& scheme,
			scheme_t& type,
			bool& secure) noexcept;

		static std::error_code parse_authority(const std::string_view& authority,
			std::string_view& user,
			std::string_view& password,
			std::string_view& host,
			uint16_t& port) noexcept;

		static std::error_code parse_path(const std::string_view& path,
			std::vector<std::string_view>& paths);

		static std::error_code parse_query(const std::string_view& query,
			std::unordered_map<std::string_view, std::string_view>& queries);
	};
}

#endif // STDX_HIERARCHICAL_URI_PARSER_H_
