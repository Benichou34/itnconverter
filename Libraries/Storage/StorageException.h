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

#ifndef STORAGE_EXCEPTION_H_INCLUDED
#define STORAGE_EXCEPTION_H_INCLUDED

#include <string>
#include <exception>

namespace storage
{
	class Exception : public std::exception
	{
	public:
		typedef enum
		{
			err_missing_impl,
			err_invalid_data,
			err_bad_length,
			err_data_not_found,
			err_unknown
		} error_t;

		Exception(const std::string& strWhat, error_t eError) : m_strWhat(strWhat), m_eError(eError) {}
		virtual ~Exception() = default;

		Exception(const Exception&) = default;
		Exception& operator=(const Exception&) = default;

		virtual const char* what() const noexcept override { return m_strWhat.c_str(); }
		error_t error() const { return m_eError; }

	private:
		std::string m_strWhat;
		error_t m_eError;
	};
}
#endif // !STORAGE_EXCEPTION_H_INCLUDED





