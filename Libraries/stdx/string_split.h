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

#ifndef STDX_STRING_SPLITTER_H_
#define STDX_STRING_SPLITTER_H_

#include <string_view>
#include <array>
#include <tuple>

#if __cplusplus > 201703L
#include <span>
#endif

namespace stdx
{
	namespace internal
	{
		template<typename T>
		struct ParseValue;

		template<typename T>
		struct ParseValue // Specialization for enums
		{
			static void	get(const std::string_view& sv, std::enable_if_t<std::is_enum<T>::value, T>& v)
			{
				std::underlying_type_t<T> vv;
				ParseValue<std::underlying_type_t<T>>::get(sv, vv);
				v = static_cast<T>(vv);
			}
		};

		template<>
		struct ParseValue<std::nullptr_t> { static void get(const std::string_view&, std::nullptr_t&) { } };

		template<>
		struct ParseValue<bool>
		{
			static void get(const std::string_view& sv, bool& v) { v = !sv.empty() && sv.front() == '1'; }
		};

		template<>
		struct ParseValue<float>
		{
			static void get(const std::string_view& sv, float& v) { v = sv.empty() ? 0.0f : std::strtof(sv.data(), 0); }
		};

		template<>
		struct ParseValue<double>
		{
			static void get(const std::string_view& sv, double& v) { v = sv.empty() ? 0.0 : std::strtod(sv.data(), 0); }
		};

		template<>
		struct ParseValue<long double>
		{
			static void get(const std::string_view& sv, long double& v) { v = sv.empty() ? 0.0 : std::strtold(sv.data(), 0); }
		};

		template<>
		struct ParseValue<char>
		{
			static void get(const std::string_view& sv, char& v) { v = sv.empty() ? 0x00 : sv.front(); }
		};

		template<>
		struct ParseValue<short>
		{
			static void get(const std::string_view& sv, short& v) { v = sv.empty() ? 0 : static_cast<short>(std::atoi(sv.data())); }
		};

		template<>
		struct ParseValue<int>
		{
			static void get(const std::string_view& sv, int& v) { v = sv.empty() ? 0 : std::atoi(sv.data()); }
		};

		template<>
		struct ParseValue<long>
		{
			static void get(const std::string_view& sv, long& v) { v = sv.empty() ? 0 : std::strtol(sv.data(), nullptr, 10); }
		};

		template<>
		struct ParseValue<long long>
		{
			static void get(const std::string_view& sv, long long& v) { v = sv.empty() ? 0 : std::strtoll(sv.data(), nullptr, 10); }
		};

		template<>
		struct ParseValue<unsigned char>
		{
			static void get(const std::string_view& sv, unsigned char& v) { v = sv.empty() ? 0x00 : static_cast<unsigned char>(sv.front()); }
		};

		template<>
		struct ParseValue<unsigned short>
		{
			static void get(const std::string_view& sv, unsigned short& v) { v = sv.empty() ? 0 : static_cast<unsigned short>(std::strtoul(sv.data(), nullptr, 10)); }
		};

		template<>
		struct ParseValue<unsigned int>
		{
			static void get(const std::string_view& sv, unsigned int& v) { v = sv.empty() ? 0 : static_cast<unsigned int>(std::strtoul(sv.data(), nullptr, 10)); }
		};

		template<>
		struct ParseValue<unsigned long>
		{
			static void get(const std::string_view& sv, unsigned long& v) { v = sv.empty() ? 0 : std::strtoul(sv.data(), nullptr, 10); }
		};

		template<>
		struct ParseValue<unsigned long long>
		{
			static void get(const std::string_view& sv, unsigned long long& v) { v = sv.empty() ? 0 : std::strtoull(sv.data(), nullptr, 10); }
		};

		template<>
		struct ParseValue<std::string_view>
		{
			static void get(const std::string_view& sv, std::string_view& v)
			{
				v = sv;

				static constexpr char QUOTE = '"';
				if (v.size() > 1 && v.front() == QUOTE && v.back() == QUOTE) // trim
				{
					v.remove_prefix(1);
					v.remove_suffix(1);
				}
			}
		};

		template<typename Tuple, size_t Size, size_t N = Size>
		struct TupleParser
		{
			static void parse(const std::array<std::string_view, Size>& resp, Tuple& t)
			{
				TupleParser<Tuple, Size, N-1>::parse(resp, t);
				ParseValue<std::remove_reference_t<std::tuple_element_t<N-1, Tuple>>>::get(std::get<N-1>(resp), std::get<N-1>(t));
			}

#if __cplusplus > 201703L
			static void parse(const std::span<std::string_view>& resp, Tuple& t)
			{
				TupleParser<Tuple, Size, N-1>::parse(resp, t);
				ParseValue<std::remove_reference_t<std::tuple_element_t<N-1, Tuple>>>::get(std::get<N-1>(resp), std::get<N-1>(t));
			}
#endif
		};

		template<typename Tuple, size_t Size>
		struct TupleParser<Tuple, Size, 0>
		{
			static void parse(const std::array<std::string_view, Size>&, Tuple&) { }
#if __cplusplus > 201703L
			static void parse(const std::span<std::string_view>&, Tuple&) { }
#endif
		};
	}

	struct string_view
	{
		template <size_t N>
		using array = std::array<std::string_view, N>;

		static size_t split(const std::string_view& resp, std::string_view* pSV, size_t max_size, char separator = ',');

		template<size_t N>
		static size_t split(const std::string_view& resp, array<N>& tab, char separator = ',')
		{
			return split(resp, tab.data(), tab.size(), separator);
		}

		template<class... Types>
		static size_t split(const std::string_view& resp, std::tuple<Types...>& tpl, char separator = ',')
		{
			typedef std::tuple<Types...> Tuple;

			array<std::tuple_size<Tuple>::value> tab;
			size_t n = split(resp, tab.data(), tab.size(), separator);

			internal::TupleParser<Tuple, std::tuple_size<Tuple>::value>::parse(tab, tpl);
			return n;
		}

		template<size_t N, class... Types>
		static size_t array_to_tuple(const array<N>& tab, std::tuple<Types...>& tpl)
		{
			typedef std::tuple<Types...> Tuple;
			constexpr size_t n = std::min<size_t>(std::tuple_size<Tuple>::value, N);

			internal::TupleParser<Tuple, n>::parse(tab, tpl);
			return n;
		}

#if __cplusplus > 201703L
		using span = std::span<std::string_view>;

		static size_t split(const std::string_view& resp, span& sp, char separator = ',')
		{
			return split(resp, sp.data(), sp.size(), separator);
		}

		template<class... Types>
		static size_t span_to_tuple(const span& tab, std::tuple<Types...>& tpl)
		{
			typedef std::tuple<Types...> Tuple;

			if (tab.size() < std::tuple_size<Tuple>::value)
				return 0;

			internal::TupleParser<Tuple, std::tuple_size<Tuple>::value>::parse(tab, tpl);
			return std::tuple_size<Tuple>::value;
		}
#endif
	};
}

#endif // STDX_STRING_SPLITTER_H_
