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
 * Purpose : Provide write to log output.
 */

#ifndef STDX_LOG_STREAM_H_INCLUDED
#define STDX_LOG_STREAM_H_INCLUDED

#include <iostream>

namespace stdx
{
	namespace log
	{
		struct Overload
		{
			Overload();
			~Overload();
		};
	}

#ifdef AUTO_OVERLOAD_STDLOG
	static log::Overload s_overload;
#endif
}

#if defined(__GNUC__)
#if (defined(__i386__) || defined(__x86_64__))
#define __debugbreak() __asm__ __volatile__("int {$}3") // SIGTRAP signal
#elif defined(__arm__)
#define __debugbreak() __asm__ __volatile__("bkpt")
#else
#define __debugbreak() __builtin_trap()
#endif
#endif

#ifdef _DEBUG
#define DEBUG_ASSERT(x) if (!(x)) __debugbreak()
#define DEBUG_LOG(x) std::clog << (x) << std::endl
#define DEBUG_LOGW(x) std::wclog << (x) << std::endl
#else
#define DEBUG_ASSERT(x)
#define DEBUG_LOG(x)
#define DEBUG_LOGW(x)
#endif

#endif // STDX_LOG_STREAM_H_INCLUDED
