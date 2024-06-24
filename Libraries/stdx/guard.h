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
 * Purpose: Implementation of the Resource Acquisition Is Initialization (RAII) programming idiom.
 *          http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization
 */

#ifndef STDX_GUARD_H_INCLUDED
#define STDX_GUARD_H_INCLUDED

#ifdef __cpp_exceptions
#include <exception>
#endif
#include <functional>

namespace stdx
{
	class function_guard
	{
	public:
		explicit function_guard(const std::function<void()>& function) noexcept : m_Function(function) {}
		explicit function_guard(std::function<void()>&& function) noexcept : m_Function(std::move(function)) {}
		explicit function_guard(function_guard&& rhs) noexcept : m_Function(std::move(rhs.m_Function))
		{
			rhs.release();
		}

		~function_guard() noexcept(false)
		{
#ifdef __cpp_exceptions
			int count = std::uncaught_exceptions();

			try
			{
#endif //__cpp_exceptions
				if (m_Function)
					m_Function();
#ifdef __cpp_exceptions
			}
			catch (...)
			{
				if (std::uncaught_exceptions() == count)
					throw;
			}
#endif //__cpp_exceptions
		}

		void release() noexcept
		{
			m_Function = nullptr;
		}

		function_guard(const function_guard&) = delete;
		function_guard& operator=(const function_guard&) = delete;
		function_guard& operator=(function_guard&&) = delete;

		void* operator new(size_t) = delete;
		void* operator new[](size_t) = delete;
		void operator delete(void*) = delete;
		void operator delete[](void*) = delete;

	private:
		std::function<void()> m_Function;
	};
}

#endif // STDX_GUARD_H_INCLUDED
