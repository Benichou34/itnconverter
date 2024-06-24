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
 * Purpose : Provide write to log.
 */

#include "logstream.h"
#include <streambuf>

#if defined(_WIN32) || defined(_MSC_VER)
#include <windows.h>
#elif defined(_linux__)
#include <syslog.h>
#include <codecvt>
#endif

namespace
{
	template<class CharT, class Traits = std::char_traits<CharT>>
	struct debugOutput { void operator()(const std::basic_string<CharT, Traits>&) { } };

#ifdef _WIN32
	template<>
	struct debugOutput<char>
	{
		void operator()(const std::basic_string<char>& str) { OutputDebugStringA(str.c_str()); }
	};

	template<>
	struct debugOutput<wchar_t>
	{
		void operator()(const std::basic_string<wchar_t>& wstr) { OutputDebugStringW(wstr.c_str()); }
	};
#elif defined(_linux__)
	template<>
	struct debugOutput<char>
	{
		void operator()(const std::basic_string<char>& str)
		{
			openlog(NULL, LOG_PID | LOG_NDELAY, LOG_USER);
			syslog(LOG_ERR, str.c_str());
			closelog();
		}
	};

	template<>
	struct debugOutput<wchar_t>
	{
		void operator()(const std::basic_string<wchar_t>& wstr)
		{
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
			debugOutput<char>()(cv.to_bytes(wstr));
		}
	};
#endif // _WIN32

	template<class CharT, class Traits = std::char_traits<CharT>>
	class basic_logbuf : public std::basic_streambuf<CharT, Traits>
	{
	protected:
		typedef typename Traits::char_type char_type;
		typedef typename Traits::int_type int_type;
		typedef typename std::basic_string<CharT, Traits>::size_type size_type;

		virtual std::streamsize xsputn(const char_type* __s, std::streamsize __n)
		{
			debugOutput<CharT, Traits>()(std::basic_string<CharT, Traits>(__s, static_cast<size_type>(__n)));
			return __n;
		}

		virtual int_type overflow(int_type c)
		{
			debugOutput<CharT, Traits>()(std::basic_string<CharT, Traits>(1, Traits::to_char_type(c)));
			return Traits::not_eof(c);
		}
	};

	typedef basic_logbuf<char> logbuf;
	typedef basic_logbuf<wchar_t> wlogbuf;

	int s_refcount;
	std::streambuf* s_pLogBuff;
	std::wstreambuf* s_pwLogBuff;
}

namespace stdx
{
	namespace log
	{
		Overload::Overload()
		{
#if defined(_MSC_VER)
			if (InterlockedIncrement(reinterpret_cast<long volatile*>(&s_refcount)) == 1)
#elif defined(__GNUC__)
			if (__atomic_fetch_add(&s_refcount, 1, __ATOMIC_ACQ_REL) == 0)
#endif
			{
				s_pLogBuff = std::clog.rdbuf(new logbuf);
				s_pwLogBuff = std::wclog.rdbuf(new wlogbuf);
			}
		}

		Overload::~Overload()
		{
#if defined(_MSC_VER)
			if (InterlockedDecrement(reinterpret_cast<long volatile*>(&s_refcount)) == 0)
#elif defined(__GNUC__)
			if (__atomic_fetch_add(&s_refcount, -1, __ATOMIC_ACQ_REL) == 1)
#endif
			{
				try
				{
					// Flush streams
					std::clog.flush();
					std::wclog.flush();

					delete std::clog.rdbuf(s_pLogBuff);
					delete std::wclog.rdbuf(s_pwLogBuff);
				}
				catch (...)
				{
				}
			}
		}
	}
}
