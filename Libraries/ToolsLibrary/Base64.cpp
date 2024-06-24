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

#include "Base64.h"

namespace
{
	constexpr char fillchar = '=';
}

std::string CBase64::Encode(const std::string& data)
{
	std::string ret;
	Encode(data, ret);
	return ret;
}

void CBase64::Encode(const std::string& data, std::string& result)
{
	const std::string cvt("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	std::string::size_type len = data.length();

	for (std::string::size_type i = 0; i < len; ++i)
	{
		char c = (data[i] >> 2) & 0x3f;
		result.append(1, cvt[c]);
		c = (data[i] << 4) & 0x3f;
		if (++i < len)
			c |= (data[i] >> 4) & 0x0f;
		result.append(1, cvt[c]);
		if (i < len)
		{
			c = (data[i] << 2) & 0x3f;
			if (++i < len)
				c |= (data[i] >> 6) & 0x03;
			result.append(1, cvt[c]);
		}
		else
		{
			++i;
			result.append(1, fillchar);
		}

		if (i < len)
		{
			c = data[i] & 0x3f;
			result.append(1, cvt[c]);
		}
		else
		{
			result.append(1, fillchar);
		}
	}
}

std::string CBase64::Decode(const std::string& data)
{
	std::string ret;
	Decode(data, ret);
	return ret;
}

void CBase64::Decode(const std::string& data, std::string& result)
{
	const std::string cvt("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
	std::string::size_type len = data.length();

	for (std::string::size_type i = 0; i < len; ++i)
	{
		char c = static_cast<char>(cvt.find(data[i]));
		++i;
		char c1 = static_cast<char>(cvt.find(data[i]));
		c = (c << 2) | ((c1 >> 4) & 0x3);
		result.append(1, c);
		if (++i < len)
		{
			c = data[i];
			if (fillchar == c)
				break;
			c = static_cast<char>(cvt.find(c));
			c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
			result.append(1, c1);
		}
		if (++i < len)
		{
			c1 = data[i];
			if (fillchar == c1)
				break;
			c1 = static_cast<char>(cvt.find(c1));
			c = ((c << 6) & 0xc0) | c1;
			result.append(1, c);
		}
	}
}
