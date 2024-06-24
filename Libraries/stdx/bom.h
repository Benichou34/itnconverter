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
 * Purpose: Read and write Byte Order Mark.
 */

#ifndef STDX_BOM_H_INCLUDED
#define STDX_BOM_H_INCLUDED

#include <iostream>
#include <string>
#include <stdint.h>

namespace stdx
{
	namespace internal
	{
		typedef struct
		{
			uint8_t c_pattern[sizeof(uint32_t)];
			size_t l_pattern;
		} s_pattern;

		// This is to avoid endianness dependency
		static const s_pattern bom_utf32_be = { { 0x00, 0x00, 0xFE, 0xFF }, 4 }; // 00 00 FE FF UTF-32, big-endian
		static const s_pattern bom_utf32_le = { { 0xFF, 0xFE, 0x00, 0x00 }, 4 }; // FF FE 00 00 UTF-32, little-endian
		static const s_pattern bom_utf16_be = { { 0xFE, 0xFF, 0x00, 0x00 }, 2 }; // FE FF       UTF-16, big-endian
		static const s_pattern bom_utf16_le = { { 0xFF, 0xFE, 0x00, 0x00 }, 2 }; // FF FE       UTF-16, little-endian
		static const s_pattern bom_utf8     = { { 0xEF, 0xBB, 0xBF, 0x00 }, 3 }; // EF BB BF    UTF-8

		static const uint8_t cod_utf32_be[] = { 0xFF, 0xFF, 0xFF, 0x00 }; // 00 00 00 xx UTF-32 BE
		static const uint8_t cod_utf32_le[] = { 0x00, 0xFF, 0xFF, 0xFF }; // xx 00 00 00 UTF-32 LE
		static const uint8_t cod_utf16_be[] = { 0xFF, 0x00, 0xFF, 0x00 }; // 00 xx 00 xx UTF-16 BE
		static const uint8_t cod_utf16_le[] = { 0x00, 0xFF, 0x00, 0xFF }; // xx 00 xx 00 UTF-16 LE
	}
	
	template<class charT, class Traits = std::char_traits<charT>>
	struct basic_bom // Byte Order Mark
	{
		typedef charT char_type;
		typedef Traits traits_type;
		
		typedef enum
		{
			none,
			utf_8,    // UTF-8
			utf_16be, // UTF-16 BE: Default value for RFC 2781
			utf_16le, // UTF-16 LE: Default value for Windows
			utf_32be, // UTF-32 BE
			utf_32le  // UTF-32 LE
		} type_t;

		static type_t read(std::basic_istream<charT, Traits>& istream, bool consume_header = true)
		{
			std::streampos pos = istream.tellg();
			type_t bomType = type_t::none;

			// Automatically find the coding
			uint32_t i32 = 0;
			for (size_t i = 0; i < sizeof(uint32_t); ++i)
				reinterpret_cast<uint8_t*>(&i32)[i] = std::char_traits<char>::to_char_type(istream.get());

			auto checkBom = [&i32](const internal::s_pattern& pattern) -> bool
			{
				uint32_t mask = 0;
				for (size_t i = 0; i < pattern.l_pattern; ++i)
					reinterpret_cast<uint8_t*>(&mask)[i] = 0xFF;
			
				return ((i32 & mask) == *reinterpret_cast<const uint32_t*>(pattern.c_pattern));
			};

			// Check BOM
			if (checkBom(internal::bom_utf32_be))
				bomType = type_t::utf_32be;
			else if (checkBom(internal::bom_utf32_le))
				bomType = type_t::utf_32le;
			else if (checkBom(internal::bom_utf16_be))
				bomType = type_t::utf_16be;
			else if (checkBom(internal::bom_utf16_le))
				bomType = type_t::utf_16le;
			else if (checkBom(internal::bom_utf8))
				bomType = type_t::utf_8;
			// No BOM, check encoding
			else if ((i32 & *reinterpret_cast<const uint32_t*>(internal::cod_utf32_be)) == 0)
				bomType = type_t::utf_32be;
			else if ((i32 & *reinterpret_cast<const uint32_t*>(internal::cod_utf32_le)) == 0)
				bomType = type_t::utf_32le;
			else if ((i32 & *reinterpret_cast<const uint32_t*>(internal::cod_utf16_be)) == 0)
				bomType = type_t::utf_16be;
			else if ((i32 & *reinterpret_cast<const uint32_t*>(internal::cod_utf16_le)) == 0)
				bomType = type_t::utf_16le;
			else
				bomType = type_t::none;

			if (consume_header)
			{
				switch (bomType)
				{
				case type_t::utf_32be:
				case type_t::utf_32le:
					pos += internal::bom_utf32_be.l_pattern;
					break;

				case type_t::utf_16be:
				case type_t::utf_16le:
					pos += internal::bom_utf16_be.l_pattern;
					break;

				case type_t::utf_8:
					pos += internal::bom_utf8.l_pattern;
					break;

				default:
					break;
				}
			}

			istream.seekg(pos);
			return bomType;
		}

		static void write(std::basic_ostream<charT, Traits>& ostream, type_t bomType)
		{
			auto writeBom = [&](const internal::s_pattern& pattern)
			{
				for (size_t i = 0; i < pattern.l_pattern; ++i)
					ostream.put(Traits::to_char_type(pattern.c_pattern[i]));
			};
			
			switch (bomType)
			{
			case type_t::utf_8: // Write UTF-8 BOM (EF BB BF)
				writeBom(internal::bom_utf8);
				break;

			case type_t::utf_16le: // Write UTF-16 LE BOM (FF FE)
				writeBom(internal::bom_utf16_le);
				break;

			case type_t::utf_16be: // Write UTF-16 BE BOM (FE FF)
				writeBom(internal::bom_utf16_be);
				break;

			case type_t::utf_32le: // Write UTF-16 LE BOM (FF FE 00 00)
				writeBom(internal::bom_utf32_le);
				break;

			case type_t::utf_32be: // Write UTF-16 BE BOM (00 00 FE FF)
				writeBom(internal::bom_utf32_be);
				break;

			default:
				break;
			}
		}

#ifdef WIN32 // Open with wide-named file, microsoft specific
		static type_t read(const std::wstring& fileName)
		{
			std::basic_ifstream<charT, Traits> ifs(fileName, std::ios_base::binary);
			return read(ifs);
		}

		static void write(const std::wstring& fileName, type_t bomType)
		{
			std::basic_ofstream<charT, Traits> ofs(fileName, std::ios_base::binary);
			write(ofs, bomType);
		}
#endif

		static type_t read(const std::string& fileName)
		{
			std::basic_ifstream<charT, Traits> ifs(fileName, std::ios_base::binary);
			return read(ifs);
		}

		static void write(const std::string& fileName, type_t bomType)
		{
			std::basic_ofstream<charT, Traits> ofs(fileName, std::ios_base::binary);
			write(ofs, bomType);
		}

		// Sets the associated locale of the stream to the one depending to the given BOM.
		static void imbue(std::basic_ios<charT, Traits>& ios, type_t bomType)
		{
			switch (bomType)
			{
			case type_t::utf_8:
				ios.imbue(std::locale(ios.getloc(), new std::codecvt_utf8<charT>));
				break;

			case type_t::utf_16le:
				ios.imbue(std::locale(ios.getloc(), new std::codecvt_utf16<charT, 0x10ffff, std::little_endian>));
				break;

			case type_t::utf_16be:
				ios.imbue(std::locale(ios.getloc(), new std::codecvt_utf16<charT, 0x10ffff>));
				break;

			case type_t::utf_32le:
			case type_t::utf_32be:
				throw std::ios_base::failure("UTF-32 isn't supported by the standard library");

			default:
				// std::codecvt<char    , char, std::mbstate_t>	identity conversion
				// std::codecvt<char16_t, char, std::mbstate_t>	conversion between UTF - 16 and UTF - 8 (since C++11)
				// std::codecvt<char32_t, char, std::mbstate_t>	conversion between UTF - 32 and UTF - 8 (since C++11)
				// std::codecvt<wchar_t , char, std::mbstate_t>	conversion between the system's native wide and multibyte encodings
				ios.imbue(std::locale(ios.getloc(), new std::codecvt<charT, char, std::mbstate_t>));
				break;
			}
		}

		static void imbue(std::basic_istream<charT, Traits>& istream)
		{
			imbue(istream, read(istream));
		}
	};

	typedef basic_bom<char> bom;
	typedef basic_bom<wchar_t> wbom;
	typedef basic_bom<char16_t> u16bom;
	typedef basic_bom<char32_t> u32bom;
}
#endif // STDX_BOM_H_INCLUDED
