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
 *
 * Purpose: Helper methods for string manipulation and conversion.
 */

#ifndef STDX_STRING_HELPER_H_INCLUDED
#define STDX_STRING_HELPER_H_INCLUDED

#include <string_view>
#include <vector>
#include <sstream>
#include <iomanip>
#include <locale>
#include <map>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <codecvt>
#include "predicate.h"

#pragma warning(disable : 4996)

namespace stdx
{
	namespace internal
	{
		template <class charT> struct Char;

		template <> struct Char < char >
		{
			static std::string hex() { return std::string("0x"); }
			static std::string space() { return std::string(" "); }
			static std::string sendl() { return std::string(" \r"); }
		};

		template <> struct Char < wchar_t >
		{
			static std::wstring hex() { return std::wstring(L"0x"); }
			static std::wstring space() { return std::wstring(L" "); }
			static std::wstring sendl() { return std::wstring(L" \r"); }
		};
	}

	template<class charT, class Traits = std::char_traits<charT>>
	struct basic_string_helper
	{
		typedef Traits traits_type;
		typedef charT value_type;
		typedef std::vector<std::basic_string<charT, Traits>> vector;
		typedef std::map<std::basic_string<charT, Traits>, std::basic_string<charT, Traits>> map;
		typedef std::unordered_map<std::basic_string<charT, Traits>, std::basic_string<charT, Traits>> unordered_map;
		typedef std::multimap<std::basic_string<charT, Traits>, std::basic_string<charT, Traits>> multimap;
		typedef std::unordered_multimap<std::basic_string<charT, Traits>, std::basic_string<charT, Traits>> unordered_multimap;
		typedef typename std::basic_string<charT, Traits>::iterator iterator;
		typedef typename std::basic_string<charT, Traits>::const_iterator const_iterator;

		template <size_t N>
		using array = std::array<std::basic_string<charT, Traits>, N>;

		static void split(
			const std::basic_string<charT, Traits>& strInput,
			vector& vecResults,
			const find_predicate<charT, Traits>& Delimiters, // find_first_of
			bool bKeepEmpties = true)
		{
			size_t ulPos = 0;
			size_t ulPrevPos = 0;

			if (Delimiters.empty() || strInput.empty())
				return;

			do
			{
				ulPos = Delimiters.find(strInput, ulPos);
				if (ulPos != std::basic_string<charT, Traits>::npos)
				{
					size_t ulsize = ulPos - ulPrevPos;
					if (ulsize || bKeepEmpties)
						vecResults.push_back(strInput.substr(ulPrevPos, ulsize));

					ulPos += Delimiters.size();
					ulPrevPos = ulPos;
				}
				else
				{
					if ((strInput.size() - ulPrevPos) || bKeepEmpties)
						vecResults.push_back(strInput.substr(ulPrevPos));
				}
			} while (ulPos != std::basic_string<charT, Traits>::npos);
		}

		template <size_t N>
		static void split(
			const std::basic_string<charT, Traits>& strInput,
			array<N>& arrayResults,
			const find_predicate<charT, Traits>& Delimiters, // find_first_of
			bool bKeepEmpties = true)
		{
			size_t ulPos = 0;
			size_t ulPrevPos = 0;
			size_t ulIndex = 0;

			if (Delimiters.empty() || strInput.empty() || arrayResults.size() == 0)
				return;
			do
			{
				ulPos = Delimiters.find(strInput, ulPos);
				if (ulPos != std::basic_string<charT, Traits>::npos)
				{
					size_t ulsize = ulPos - ulPrevPos;
					if (ulsize || bKeepEmpties)
					{
						if (ulIndex < (arrayResults.size() - 1))
							arrayResults[ulIndex++] = strInput.substr(ulPrevPos, ulsize);
						else
							arrayResults[ulIndex++] = strInput.substr(ulPrevPos);
					}

					ulPos += Delimiters.size();
					ulPrevPos = ulPos;
				}
				else
				{
					if ((strInput.size() - ulPrevPos) || bKeepEmpties)
						arrayResults[ulIndex++] = strInput.substr(ulPrevPos);
				}
			} while (ulPos != std::basic_string<charT, Traits>::npos && ulIndex < N);
		}

		template<typename T>
		static void split(
			const std::basic_string<charT, Traits>& strInput,
			T& mapResults, // map or multimap
			const find_predicate<charT, Traits>& Delimiters, // find_first_of
			const find_predicate<charT, Traits>& Separaters, // find_first
			bool bKeepEmpties = true)
		{
			vector vecResults;
			typename vector::iterator itResults;

			split(strInput, vecResults, Delimiters, bKeepEmpties);
			for (itResults = vecResults.begin(); itResults != vecResults.end(); ++itResults)
			{
				size_t ulPos = Separaters.find(*itResults);
				if (ulPos != std::basic_string<charT, Traits>::npos)
					mapResults.insert(std::pair<std::basic_string<charT, Traits>, std::basic_string<charT, Traits>>(itResults->substr(0, ulPos), itResults->substr(ulPos + 1)));
				else if (bKeepEmpties)
					mapResults.insert(std::pair<std::basic_string<charT, Traits>, std::basic_string<charT, Traits>>(*itResults, std::basic_string<charT, Traits>()));
			}
		}

		template<class T> static void string_to(const std::basic_string<charT, Traits>& str, T& dest)
		{
			std::basic_istringstream<charT, Traits> iss(str);

			if (!str.compare(0, 2, internal::Char<charT>::hex()))
			{
				iss.ignore(2);
				iss >> std::hex;
			}

			iss >> dest;
		}

		template<class T> static T string_to(const std::basic_string<charT, Traits>& str)
		{
			T dest;
			string_to<T>(str, dest);
			return dest;
		}

		template<class T> static std::basic_string<charT> to_string(const T& src, int precision = 6)
		{
			std::basic_ostringstream<charT, Traits> oss;
			oss << std::setiosflags(std::ios::fixed) << std::setprecision(precision) << src;
			return oss.str();
		}

		template<class T> static bool istype(const std::basic_string<charT, Traits>& str)
		{
			std::basic_istringstream<charT, Traits> iss(str);

			if (!str.compare(0, 2, internal::Char<charT>::hex()))
			{
				iss.ignore(2);
				iss >> std::hex;
			}

			T type;
			return (iss >> type) && iss.eof();
		}

		static void toupper(std::basic_string<charT, Traits>& str)
		{
			for (typename std::basic_string<charT, Traits>::iterator it = str.begin(); it != str.end(); ++it)
				*it = std::toupper(*it, std::locale::classic());
		}

		static void tolower(std::basic_string<charT, Traits>& str)
		{
			for (typename std::basic_string<charT, Traits>::iterator it = str.begin(); it != str.end(); ++it)
				*it = std::tolower(*it, std::locale::classic());
		}

		static void trimleft(std::basic_string<charT, Traits>& strString, const std::basic_string<charT, Traits>& strTargets = internal::Char<charT>::space())
		{
			if (strString.empty())
				return;

			const_iterator cit;
			size_t ulTrimeSize = 0;

			do
			{
				for (cit = strTargets.begin(); cit != strTargets.end(); ++cit)
				{
					if (strString[ulTrimeSize] == *cit)
					{
						ulTrimeSize++;
						break;
					}
				}
			} while (!(cit == strTargets.end()));

			strString.erase(0, ulTrimeSize);
		}

		static void trimright(std::basic_string<charT, Traits>& strString, const std::basic_string<charT, Traits>& strTargets = internal::Char<charT>::sendl())
		{
			if (strString.empty())
				return;

			const_iterator cit;
			size_t ulTrimeSize = strString.size();

			do
			{
				for (cit = strTargets.begin(); cit != strTargets.end(); ++cit)
				{
					if (strString[ulTrimeSize - 1] == *cit)
					{
						ulTrimeSize--;
						break;
					}
				}
			} while (ulTrimeSize && !(cit == strTargets.end()));

			strString.resize(ulTrimeSize);
		}

		static void replace(std::basic_string<charT, Traits>& strString, const std::basic_string<charT>& strSrc, const std::basic_string<charT, Traits>& strDst)
		{
			size_t pos = 0;

			while ((pos = strString.find(strSrc, pos)) != std::basic_string<charT, Traits>::npos)
			{
				strString.replace(pos, strSrc.size(), strDst);
				pos += strDst.size();
			}
		}

		static void replace(std::basic_string<charT, Traits>& strString, charT cSrc, charT cDst)
		{
			std::replace(strString.begin(), strString.end(), cSrc, cDst);
		}

		static void remove(std::basic_string<charT, Traits>& strString, charT cSrc)
		{
			strString.resize(std::distance(strString.begin(), std::remove(strString.begin(), strString.end(), cSrc)));
		}

		static std::string narrow(const std::wstring& ws)
		{
			std::string res;
			narrow(ws, res);
			return res;
		}

		static void narrow(const std::wstring& ws, std::string& s)
		{
			std::locale loc;
			s.resize(ws.size());
			std::use_facet< std::ctype<wchar_t> >(loc).narrow(&ws[0], &ws[ws.size()], '?', &s[0]);
		}

		static std::wstring widen(const std::string& s)
		{
			std::wstring res;
			widen(s, res);
			return res;
		}

		static void widen(const std::string& s, std::wstring& ws)
		{
			std::locale loc;
			ws.resize(s.size());
			std::use_facet< std::ctype<wchar_t> >(loc).widen(&s[0], &s[s.size()], &ws[0]);
		}

		static void to_utf8(const std::basic_string<charT, Traits>& ws, std::string& utfs)
		{
			std::wstring_convert<std::codecvt_utf8<charT>, charT> cv;
			utfs = cv.to_bytes(ws);
		}

		static std::string to_utf8(const std::basic_string<charT, Traits>& ws)
		{
			std::string utfs;
			to_utf8(ws, utfs);
			return utfs;
		}

		static void from_utf8(const std::string& utfs, std::basic_string<charT, Traits>& ws)
		{
			std::wstring_convert<std::codecvt_utf8<charT>, charT> cv;
			ws = cv.from_bytes(utfs);
		}

		static std::basic_string<charT, Traits> from_utf8(const std::string& utfs)
		{
			std::basic_string<charT, Traits> ws;
			from_utf8(utfs, ws);
			return ws;
		}
	};

	template<> // Specialization for char
	void basic_string_helper<char>::from_utf8(const std::string& utfs, std::string& s)
	{
		std::wstring ws;
		basic_string_helper<wchar_t>::from_utf8(utfs, ws);
		basic_string_helper<wchar_t>::narrow(ws, s);
	}

	template<> // Specialization for char
	void basic_string_helper<char>::to_utf8(const std::string& s, std::string& utfs)
	{
		std::wstring ws;
		basic_string_helper<wchar_t>::widen(s, ws);
		basic_string_helper<wchar_t>::to_utf8(ws, utfs);
	}

	template<class charT, class Traits = std::char_traits<charT>>
	struct basic_string_view_helper
	{
		typedef Traits traits_type;
		typedef charT value_type;
		typedef std::vector<std::basic_string_view<charT, Traits>> vector;
		typedef std::map<std::basic_string_view<charT, Traits>, std::basic_string_view<charT, Traits>> map;
		typedef std::unordered_map<std::basic_string_view<charT, Traits>, std::basic_string_view<charT, Traits>> unordered_map;
		typedef std::multimap<std::basic_string_view<charT, Traits>, std::basic_string_view<charT, Traits>> multimap;
		typedef std::unordered_multimap<std::basic_string_view<charT, Traits>, std::basic_string_view<charT, Traits>> unordered_multimap;
		typedef typename std::basic_string_view<charT, Traits>::iterator iterator;
		typedef typename std::basic_string_view<charT, Traits>::const_iterator const_iterator;

		static void split(
			const std::basic_string_view<charT, Traits>& strInput,
			vector& vecResults,
			const find_predicate<charT, Traits>& Delimiters, // find_first_of
			bool bKeepEmpties = true)
		{
			size_t ulPos = 0;
			size_t ulPrevPos = 0;

			if (Delimiters.empty() || strInput.empty())
				return;

			do
			{
				ulPos = Delimiters.find(strInput, ulPos);
				if (ulPos != std::basic_string_view<charT, Traits>::npos)
				{
					size_t ulsize = ulPos - ulPrevPos;
					if (ulsize || bKeepEmpties)
						vecResults.push_back(strInput.substr(ulPrevPos, ulsize));

					ulPos += Delimiters.size();
					ulPrevPos = ulPos;
				}
				else
				{
					if ((strInput.size() - ulPrevPos) || bKeepEmpties)
						vecResults.push_back(strInput.substr(ulPrevPos));
				}
			} while (ulPos != std::basic_string_view<charT, Traits>::npos);
		}

		template<typename T>
		static void split(
			const std::basic_string_view<charT, Traits>& strInput,
			T& mapResults, // map or multimap
			const find_predicate<charT, Traits>& Delimiters, // find_first_of
			const find_predicate<charT, Traits>& Separaters, // find_first
			bool bKeepEmpties = true)
		{
			vector vecResults;
			typename vector::iterator itResults;

			split(strInput, vecResults, Delimiters, bKeepEmpties);
			for (itResults = vecResults.begin(); itResults != vecResults.end(); ++itResults)
			{
				size_t ulPos = Separaters.find(*itResults);
				if (ulPos != std::basic_string_view<charT, Traits>::npos)
					mapResults.insert(std::pair<std::basic_string_view<charT, Traits>, std::basic_string_view<charT, Traits>>(itResults->substr(0, ulPos), itResults->substr(ulPos + 1)));
				else if (bKeepEmpties)
					mapResults.insert(std::pair<std::basic_string_view<charT, Traits>, std::basic_string_view<charT, Traits>>(*itResults, std::basic_string_view<charT, Traits>()));
			}
		}
	};

	typedef basic_string_helper<char> string_helper;
	typedef basic_string_helper<wchar_t> wstring_helper;

	typedef basic_string_view_helper<char> string_view_helper;
	typedef basic_string_view_helper<wchar_t> wstring_view_helper;

#ifdef _HAS_CPP11_
	typedef basic_string_helper<char16_t> u16string_helper;
	typedef basic_string_helper<char32_t> u32string_helper;

	typedef basic_string_view_helper<char16_t> u16string_view_helper;
	typedef basic_string_view_helper<char32_t> u32string_view_helper;
#endif
}
#endif // STDX_STRING_HELPER_H_INCLUDED
