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

#ifndef JSONVALUE_H_INCLUDED
#define JSONVALUE_H_INCLUDED

#include "JsonObject.h"
#include "JsonArray.h"

class CJsonValue
{
public:
	typedef enum
	{
		JSON_TYPE_NONE,
		JSON_TYPE_NULL,
		JSON_TYPE_STRING,
		JSON_TYPE_NUMBER,
		JSON_TYPE_OBJECT,
		JSON_TYPE_ARRAY,
		JSON_TYPE_BOOLEAN
	} E_JSON_TYPE;

private:
	std::string m_strName;
	E_JSON_TYPE m_eType;
	std::string m_String;
	double m_dNumber;
	bool m_Boolean;
	CJsonArray m_Array;
	CJsonObject m_Object;

public:
	friend std::ostream& operator << (std::ostream& oss, const CJsonValue& jsValue);
	friend std::istream& operator >> (std::istream& iss, CJsonValue& jsValue);

	CJsonValue();
	CJsonValue(const CJsonValue& jsValue);
	CJsonValue(const std::string& strName);
	virtual ~CJsonValue();

	const std::string& getName() const { return m_strName; }
	CJsonValue& setName(const std::string& strName);

	E_JSON_TYPE getType() const { return m_eType; }
	CJsonValue& setType(E_JSON_TYPE eJsonType);

	operator CJsonObject& ();
	operator const CJsonObject& () const;
	operator CJsonArray& ();
	operator const CJsonArray& () const;
	operator const std::string& () const;
	operator double() const;
	operator int() const;
	operator unsigned int() const;
	operator long() const;
	operator unsigned long() const;
	operator long long() const;
	operator unsigned long long() const;
	operator bool() const;

	CJsonValue& operator=(const CJsonValue& jsValue);
	CJsonValue& operator=(const CJsonObject& objValue);
	CJsonValue& operator=(const CJsonArray& aryValue);
	CJsonValue& operator=(const std::string& strValue);
	CJsonValue& operator=(double dValue);
	CJsonValue& operator=(float fValue);
	CJsonValue& operator=(char cValue);
	CJsonValue& operator=(unsigned char ucValue);
	CJsonValue& operator=(short sValue);
	CJsonValue& operator=(unsigned short usValue);
	CJsonValue& operator=(int nValue);
	CJsonValue& operator=(unsigned int unValue);
	CJsonValue& operator=(long lValue);
	CJsonValue& operator=(unsigned long ulValue);
	CJsonValue& operator=(long long llvalue);
	CJsonValue& operator=(unsigned long long ullvalue);
	CJsonValue& operator=(bool bValue);

	const CJsonValue& operator()(const std::string& strName) const;
	CJsonValue& operator()(const std::string& strName);

	const CJsonValue& at(const std::string& xPath) const;
	CJsonValue& at(const std::string& xPath);

	const CJsonValue& operator[](size_t index) const;
	CJsonValue& operator[](size_t index);

	const CJsonValue& at(size_t index) const;
	CJsonValue& at(size_t index);

	bool exist(const std::string& xPath) const;
	void clear();

	size_t parse(const std::string& strValue);
	std::string str() const;

private:
	char DecodeHex(const std::string& strHex) const;
	void EncodeEscapeCharacter(std::string& str) const;
	void DecodeEscapeCharacter(std::string& str) const;
};

#endif // !JSONVALUE_H_INCLUDED
