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
 * Purpose: Predicates classes for string manipulation.
 */

#ifndef STDX_PREDICATE_H_INCLUDED
#define STDX_PREDICATE_H_INCLUDED

#include <string_view>

namespace stdx
{
	template <class charT, class Traits = std::char_traits<charT>>
	class find_predicate
	{
	public:
		virtual ~find_predicate() {}

		find_predicate(const std::basic_string_view<charT, Traits>& strFinder) : m_strFinder(strFinder) {}
		find_predicate(const charT* strFinder) : m_strFinder(strFinder) {}
		find_predicate(const charT& cFinder) : m_strFinder(&cFinder, 1) {}
		bool empty() const { return m_strFinder.empty(); }
		virtual size_t size() const = 0;
		virtual size_t find(const std::basic_string_view<charT, Traits>& str, size_t pos = 0) const = 0;

	protected:
		const std::basic_string_view<charT, Traits>& finder() const { return m_strFinder; }

	private:
		std::basic_string_view<charT, Traits> m_strFinder;
	};

	template <class charT, class Traits = std::char_traits<charT>>
	class basic_find_first : public find_predicate<charT, Traits>
	{
	public:
		explicit basic_find_first(const std::basic_string_view<charT, Traits>& strFinder) : find_predicate<charT, Traits>(strFinder) {}
		explicit basic_find_first(const charT* strFinder) : find_predicate<charT, Traits>(strFinder) {}
		explicit basic_find_first(const charT& cFinder) : find_predicate<charT, Traits>(cFinder) {}
		virtual size_t size() const override { return this->finder().size(); }
		virtual size_t find(const std::basic_string_view<charT, Traits>& str, size_t pos = 0) const override { return str.find(this->finder(), pos); }
	};

	template <class charT, class Traits = std::char_traits<charT>>
	class basic_find_last : public find_predicate<charT, Traits>
	{
	public:
		explicit basic_find_last(const std::basic_string_view<charT, Traits>& strFinder) : find_predicate<charT, Traits>(strFinder) {}
		explicit basic_find_last(const charT* strFinder) : find_predicate<charT, Traits>(strFinder) {}
		explicit basic_find_last(const charT& cFinder) : find_predicate<charT, Traits>(cFinder) {}
		virtual size_t size() const override { return this->finder().size(); }
		virtual size_t find(const std::basic_string_view<charT, Traits>& str, size_t pos = 0) const override { return str.rfind(this->finder(), pos); }
	};

	template <class charT, class Traits = std::char_traits<charT>>
	class basic_find_first_of : public find_predicate<charT, Traits>
	{
	public:
		explicit basic_find_first_of(const std::basic_string_view<charT, Traits>& strFinder) : find_predicate<charT, Traits>(strFinder) {}
		explicit basic_find_first_of(const charT* strFinder) : find_predicate<charT, Traits>(strFinder) {}
		explicit basic_find_first_of(const charT& cFinder) : find_predicate<charT, Traits>(cFinder) {}
		virtual size_t size() const override { return 1; }
		virtual size_t find(const std::basic_string_view<charT, Traits>& str, size_t pos = 0) const override { return str.find_first_of(this->finder(), pos); }
	};

	template <class charT, class Traits = std::char_traits<charT>>
	class basic_find_last_of : public find_predicate<charT, Traits>
	{
	public:
		explicit basic_find_last_of(const std::basic_string_view<charT, Traits>& strFinder) : find_predicate<charT, Traits>(strFinder) {}
		explicit basic_find_last_of(const charT* strFinder) : find_predicate<charT, Traits>(strFinder) {}
		explicit basic_find_last_of(const charT& cFinder) : find_predicate<charT, Traits>(cFinder) {}
		virtual size_t size() const override { return 1; }
		virtual size_t find(const std::basic_string_view<charT, Traits>& str, size_t pos = 0) const override { return str.find_last_of(this->finder(), pos); }
	};

	typedef basic_find_first<char> find_first;
	typedef basic_find_first<wchar_t> wfind_first;

	typedef basic_find_last<char> find_last;
	typedef basic_find_last<wchar_t> wfind_last;

	typedef basic_find_first_of<char> find_first_of;
	typedef basic_find_first_of<wchar_t> wfind_first_of;

	typedef basic_find_last_of<char> find_last_of;
	typedef basic_find_last_of<wchar_t> wfind_last_of;

	typedef basic_find_first<char16_t> u16find_first;
	typedef basic_find_first<char32_t> u32find_first;

	typedef basic_find_last<char16_t> u16find_last;
	typedef basic_find_last<char32_t> u32find_last;

	typedef basic_find_first_of<char16_t> u16find_first_of;
	typedef basic_find_first_of<char32_t> u32find_first_of;

	typedef basic_find_last_of<char16_t> u16find_last_of;
	typedef basic_find_last_of<char32_t> u32find_last_of;
}
#endif // STDX_PREDICATE_H_INCLUDED
