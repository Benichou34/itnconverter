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

#ifndef _TOOLS_STRING_H_INCLUDED_
#define _TOOLS_STRING_H_INCLUDED_

#include <string>

namespace BaseTools
{
	template <class charT> struct Char;

	template <> struct Char<char>
	{
		static std::string sep() { return std::string("\\/"); }
	};

	template <> struct Char<wchar_t>
	{
		static std::wstring sep() { return std::wstring(L"\\/"); }
	};
}

template<class charT, class Traits = std::char_traits<charT>>
class BaseToolsString
{
public:
	static std::basic_string<charT> FileDirectory(const std::basic_string<charT>& strFilePath)
	{
		std::basic_string<charT> strDirectory;

		size_t dir_pos = strFilePath.find_last_of(BaseTools::Char<charT>::sep());
		if (dir_pos != std::string::npos)
			strDirectory.assign(strFilePath, 0, dir_pos + 1);

		return strDirectory;
	}

	static std::basic_string<charT> FileName(const std::basic_string<charT>& strFilePath)
	{
		std::basic_string<charT> strName(strFilePath);

		size_t dir_pos = strName.find_last_of(BaseTools::Char<charT>::sep());
		if (dir_pos != std::basic_string<charT>::npos)
			strName.erase(0, dir_pos + 1);

		return strName;
	}

	static std::basic_string<charT> FileTitle(const std::basic_string<charT>& strFilePath)
	{
		std::basic_string<charT> strName(FileName(strFilePath));

		size_t ext_pos = strName.find_last_of('.');
		if (ext_pos != std::basic_string<charT>::npos)
			strName.erase(ext_pos);

		return strName;
	}

	static std::basic_string<charT> FileExt(const std::basic_string<charT>& strFilePath)
	{
		std::basic_string<charT> strExt;

		size_t ext_pos = strFilePath.find_last_of('.');
		if (ext_pos != std::basic_string<charT>::npos)
			strExt.assign(strFilePath, ext_pos + 1, strFilePath.size());

		return strExt;
	}

	static void Load(unsigned int uID, std::string& str);
	static void Load(unsigned int uID, std::wstring& str);

	static std::basic_string<charT> Load(unsigned int uID)
	{
		std::basic_string<charT> str;
		Load(uID, str);
		return str;
	}
};

typedef BaseToolsString<char> CToolsString;
typedef BaseToolsString<wchar_t> CWToolsString;

#endif // _TOOLS_STRING_H_INCLUDED_
