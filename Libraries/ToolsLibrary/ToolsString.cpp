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

#include "ToolsString.h"
#include "stdx/string_helper.h"
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
	template<class T> int LoadStringT(UINT uID, std::vector<T>&);

	template<>
	int LoadStringT<char>(UINT uID, std::vector<char>& buffer)
	{
		return LoadStringA(GetModuleHandle(nullptr), uID, &buffer[0], static_cast<int>(buffer.size()));
	}

	template<>
	int LoadStringT<wchar_t>(UINT uID, std::vector<wchar_t>& buffer)
	{
		return LoadStringW(GetModuleHandle(nullptr), uID, &buffer[0], static_cast<int>(buffer.size()));
	}

	template<class T>
	void Load(unsigned int uID, std::basic_string<T>& str)
	{
#ifdef _WIN32
		size_t nSize = 0;
		std::vector<T> vecTmpBuffer(256);

		do
		{
			if (nSize == vecTmpBuffer.size() - 1)
				vecTmpBuffer.resize(vecTmpBuffer.size() * 2);

			nSize = static_cast<size_t>(LoadStringT(uID, vecTmpBuffer));
			if (!nSize)
				throw GetLastError();
		} while (nSize == vecTmpBuffer.size() - 1);

		str.assign(&vecTmpBuffer[0], nSize);
#endif
	}
}

template<>
void BaseToolsString<char>::Load(unsigned int uID, std::string& str)
{
	std::wstring wstr;
	::Load<wchar_t>(uID, wstr);
	stdx::wstring_helper::to_utf8(wstr, str);
	//::Load<char>(uID, str);
}

template<>
void BaseToolsString<wchar_t>::Load(unsigned int uID, std::wstring& str)
{
	::Load<wchar_t>(uID, str);
}
