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

#ifndef JSONPARSER_H_INCLUDED
#define JSONPARSER_H_INCLUDED

#include <exception>
#include "JsonObject.h"
#include "JsonValue.h"
#include "JsonArray.h"

class CJsonParser : public CJsonValue
{
public:
	typedef enum
	{
		JSON_SUCCESS,
		JSON_ALREADY_EXIT,
		JSON_OBJECT_ERROR,
		JSON_VALUE_ERROR,
		JSON_ARRAY_ERROR,
		JSON_OUT_OF_RANGE,
		JSON_BAD_STRING,
		JSON_BAD_NUMBER,
		JSON_BAD_BOOLEAN,
		JSON_BAD_OBJECT,
		JSON_BAD_ARRAY,
		JSON_BAD_VALUE
	} E_JSON_ERROR;

public:
	CJsonParser();
	CJsonParser(const CJsonParser& jsParser);
	CJsonParser(const std::string& strJson);
	virtual ~CJsonParser();

	CJsonParser& operator=(const CJsonParser&) = default;

	E_JSON_ERROR parse(const std::string& strJson);
	CJsonParser& operator<<(const std::string& strJson);
};

class CJsonException : public std::exception
{
public:
	explicit CJsonException(CJsonParser::E_JSON_ERROR eError, const std::string& strWhat = std::string()) : m_strWhat(strWhat), m_eError(eError) {}
	CJsonException(const CJsonException&) = default;
	virtual ~CJsonException() {}

	virtual const char* what() const noexcept override { return m_strWhat.c_str(); }
	CJsonParser::E_JSON_ERROR error() const { return m_eError; }

private:
	std::string m_strWhat;
	CJsonParser::E_JSON_ERROR m_eError;
};

#endif // !JSONPARSER_H_INCLUDED
