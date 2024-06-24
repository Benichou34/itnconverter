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

#include <sstream>
#include "JsonParser.h"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

CJsonValue::CJsonValue() :
	m_eType(JSON_TYPE_NONE),
	m_dNumber(0),
	m_Boolean(false)
{
}

CJsonValue::CJsonValue(const CJsonValue& jsValue)
{
	operator=(jsValue);
}

CJsonValue::CJsonValue(const std::string& strName) :
	m_strName(strName),
	m_eType(JSON_TYPE_NONE),
	m_dNumber(0),
	m_Boolean(false)
{
}

CJsonValue::~CJsonValue()
{
}

char CJsonValue::DecodeHex(const std::string& strHex) const
{
	std::istringstream iss(strHex);
	int nValue;
	iss >> std::hex >> nValue;

	return static_cast<char>(nValue);
}

void CJsonValue::EncodeEscapeCharacter(std::string& strEscape) const
{
	size_t nPos = strEscape.find_first_of("\\\"\b\f\n\r\t");
	while (nPos != std::string::npos)
	{
		strEscape.insert(nPos, 1, '\\');
		nPos = strEscape.find_first_of("\\\"\b\f\n\r\t", nPos + 2);
	}
}

void CJsonValue::DecodeEscapeCharacter(std::string& strEscape) const
{
	size_t nPos = strEscape.find('\\');

	while (nPos != std::string::npos)
	{
		if (nPos + 1 < strEscape.size())
		{
			strEscape.erase(nPos, 1);
			switch (strEscape[nPos])
			{
			case '"':
			case '\\':
			case '/':
				break;
			case 'b':
				strEscape[nPos] = '\b';
				break;
			case 'f':
				strEscape[nPos] = '\f';
				break;
			case 'n':
				strEscape[nPos] = '\n';
				break;
			case 'r':
				strEscape[nPos] = '\r';
				break;
			case 't':
				strEscape[nPos] = '\t';
				break;
			case 'x':
				strEscape[nPos] = DecodeHex(strEscape.substr(nPos + 1, 2));
				strEscape.erase(nPos + 1, 2);
				break;
			case 'u':
				strEscape[nPos] = DecodeHex(strEscape.substr(nPos + 3, 2));
				strEscape.erase(nPos + 1, 4);
				break;
			default:
				//throw CJsonException(CJsonParser::JSON_BAD_STRING, strEscape);
				break;
			}
		}
		else
		{
			throw CJsonException(CJsonParser::JSON_BAD_STRING, strEscape);
		}

		nPos = strEscape.find('\\', nPos + 1);
	}
}

CJsonValue& CJsonValue::setName(const std::string& strName)
{
	m_strName = strName;
	return *this;
}

CJsonValue& CJsonValue::setType(E_JSON_TYPE eJsonType)
{
	m_eType = eJsonType;
	return *this;
}

CJsonValue::operator CJsonObject& ()
{
	if (m_eType != JSON_TYPE_OBJECT)
		throw CJsonException(CJsonParser::JSON_BAD_OBJECT, getName());

	return m_Object;
}

CJsonValue::operator const CJsonObject& () const
{
	if (m_eType != JSON_TYPE_OBJECT)
		throw CJsonException(CJsonParser::JSON_BAD_OBJECT, getName());

	return m_Object;
}

CJsonValue::operator CJsonArray& ()
{
	if (m_eType != JSON_TYPE_ARRAY)
		throw CJsonException(CJsonParser::JSON_BAD_ARRAY, getName());

	return m_Array;
}

CJsonValue::operator const CJsonArray& () const
{
	if (m_eType != JSON_TYPE_ARRAY)
		throw CJsonException(CJsonParser::JSON_BAD_ARRAY, getName());

	return m_Array;
}

CJsonValue::operator const std::string& () const
{
	if (m_eType != JSON_TYPE_STRING)
		throw CJsonException(CJsonParser::JSON_BAD_STRING, getName());

	return m_String;
}

CJsonValue::operator double() const
{
	if (m_eType != JSON_TYPE_NUMBER)
		throw CJsonException(CJsonParser::JSON_BAD_NUMBER, getName());

	return m_dNumber;
}

CJsonValue::operator int() const
{
	return static_cast<int>(operator double());
}

CJsonValue::operator unsigned int() const
{
	return static_cast<unsigned int>(operator double());
}

CJsonValue::operator long() const
{
	return static_cast<long>(operator double());
}

CJsonValue::operator unsigned long() const
{
	return static_cast<unsigned long>(operator double());
}

CJsonValue::operator long long() const
{
	return static_cast<long long>(operator double());
}

CJsonValue::operator unsigned long long() const
{
	return static_cast<unsigned long long>(operator double());
}

CJsonValue::operator bool() const
{
	if (m_eType != JSON_TYPE_BOOLEAN)
		throw CJsonException(CJsonParser::JSON_BAD_BOOLEAN, getName());

	return m_Boolean;
}

CJsonValue& CJsonValue::operator=(const CJsonValue& jsValue)
{
	if (&jsValue != this)
	{
		if (!m_strName.size())
			m_strName = jsValue.m_strName;

		m_eType = jsValue.m_eType;
		m_String = jsValue.m_String;
		m_dNumber = jsValue.m_dNumber;
		m_Boolean = jsValue.m_Boolean;
		m_Array = jsValue.m_Array;
		m_Object = jsValue.m_Object;
	}

	return *this;
}

CJsonValue& CJsonValue::operator=(const std::string& strValue)
{
	m_eType = JSON_TYPE_STRING;
	m_String = strValue;

	return *this;
}

CJsonValue& CJsonValue::operator=(double dValue)
{
	m_eType = JSON_TYPE_NUMBER;
	m_dNumber = dValue;

	return *this;
}

CJsonValue& CJsonValue::operator=(float fValue)
{
	return operator=(static_cast<double>(fValue));
}

CJsonValue& CJsonValue::operator=(char cValue)
{
	return operator=(static_cast<double>(cValue));
}

CJsonValue& CJsonValue::operator=(unsigned char ucValue)
{
	return operator=(static_cast<double>(ucValue));
}

CJsonValue& CJsonValue::operator=(short sValue)
{
	return operator=(static_cast<double>(sValue));
}

CJsonValue& CJsonValue::operator=(unsigned short usValue)
{
	return operator=(static_cast<double>(usValue));
}
CJsonValue& CJsonValue::operator=(int nValue)
{
	return operator=(static_cast<double>(nValue));
}

CJsonValue& CJsonValue::operator=(unsigned int unValue)
{
	return operator=(static_cast<double>(unValue));
}

CJsonValue& CJsonValue::operator=(long lValue)
{
	return operator=(static_cast<double>(lValue));
}

CJsonValue& CJsonValue::operator=(unsigned long ulValue)
{
	return operator=(static_cast<double>(ulValue));
}

CJsonValue& CJsonValue::operator=(long long llvalue)
{
	return operator=(static_cast<double>(llvalue));
}

CJsonValue& CJsonValue::operator=(unsigned long long ullvalue)
{
	return operator=(static_cast<double>(ullvalue));
}

CJsonValue& CJsonValue::operator=(bool bValue)
{
	m_eType = JSON_TYPE_BOOLEAN;
	m_Boolean = bValue;

	return *this;
}

CJsonValue& CJsonValue::operator=(const CJsonObject& objValue)
{
	m_eType = JSON_TYPE_OBJECT;
	m_Object = objValue;

	return *this;
}

CJsonValue& CJsonValue::operator=(const CJsonArray& aryValue)
{
	m_eType = JSON_TYPE_ARRAY;
	m_Array = aryValue;

	return *this;
}

const CJsonValue& CJsonValue::operator()(const std::string& strName) const
{
	return static_cast<const CJsonObject&>(*this)(strName);
}

CJsonValue& CJsonValue::operator()(const std::string& strName)
{
	return static_cast<CJsonObject&>(*this)(strName);
}

const CJsonValue& CJsonValue::at(const std::string& xPath) const
{
	return static_cast<const CJsonObject&>(*this).at(xPath);
}

CJsonValue& CJsonValue::at(const std::string& xPath)
{
	return static_cast<CJsonObject&>(*this).at(xPath);
}

const CJsonValue& CJsonValue::operator[](size_t index) const
{
	return static_cast<const CJsonArray&>(*this)[index];
}

CJsonValue& CJsonValue::operator[](size_t index)
{
	return static_cast<CJsonArray&>(*this)[index];
}

const CJsonValue& CJsonValue::at(size_t index) const
{
	return static_cast<const CJsonArray&>(*this).at(index);
}

CJsonValue& CJsonValue::at(size_t index)
{
	return static_cast<CJsonArray&>(*this).at(index);
}

bool CJsonValue::exist(const std::string& xPath) const
{
	return static_cast<const CJsonObject&>(*this).exist(xPath);
}

void CJsonValue::clear()
{
	m_eType = JSON_TYPE_NONE;
	m_dNumber = 0;
	m_Boolean = false;
	m_Array.clear();
	m_Object.clear();
	m_String.clear();
}

size_t CJsonValue::parse(const std::string& strValue)
{
	size_t nPos = 0;
	size_t nPosFirst = 0;
	std::string strTmp;
	size_t nCounter;

	while (nPos < strValue.length())
	{
		nPos = strValue.find_first_of("{[\",]}", nPos);
		if (nPos == std::string::npos)
			throw CJsonException(CJsonParser::JSON_VALUE_ERROR);

		switch (strValue[nPos])
		{
		case '{': // Object
			if (nPosFirst)
			{
				nPos++;
			}
			else
			{
				m_eType = JSON_TYPE_OBJECT;
				nPos += m_Object.parse(strValue.substr(nPos));
				return nPos;
			}
			break;

		case '[': // Array
			if (nPosFirst)
			{
				nPos++;
			}
			else
			{
				m_eType = JSON_TYPE_ARRAY;
				nPos += m_Array.parse(strValue.substr(nPos));
				return nPos;
			}
			break;

		case '\"': // Name
			if (!nPosFirst)
			{
				nPos++;
				nPosFirst = nPos;
			}
			else
			{
				nCounter = 0;
				while (strValue[nPos - nCounter - 1] == '\\')
					nCounter++;

				if (nCounter % 2)
				{
					nPos++;
				}
				else
				{
					m_eType = JSON_TYPE_STRING;
					m_String = strValue.substr(nPosFirst, nPos - nPosFirst);
					DecodeEscapeCharacter(m_String);
					return nPos + 1;
				}
			}
			break;

		case ']': // End Array
		case '}': // End Object
		case ',': // Next value
			if (nPosFirst)
			{
				nPos++;
			}
			else
			{
				strTmp = strValue.substr(0, nPos);
				if (strTmp.size())
				{
					if (strTmp.find("true") != std::string::npos)
					{
						m_eType = JSON_TYPE_BOOLEAN;
						m_Boolean = true;
					}
					else if (strTmp.find("false") != std::string::npos)
					{
						m_eType = JSON_TYPE_BOOLEAN;
						m_Boolean = false;
					}
					else if (strTmp.find("null") != std::string::npos)
					{
						m_eType = JSON_TYPE_NULL;
					}
					else
					{
						std::istringstream iss(strTmp);
						iss >> m_dNumber;

						if (!iss.fail())
							m_eType = JSON_TYPE_NUMBER;
					}
				}
				return nPos;
			}
			break;

		default:
			break;
		}
	}

	return nPos;
}

std::string CJsonValue::str() const
{
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

std::ostream& operator << (std::ostream& oss, const CJsonValue& jsValue)
{
	if (jsValue.getName().size())
		oss << '"' << jsValue.getName() << "\": ";

	switch (jsValue.getType())
	{
	case CJsonValue::JSON_TYPE_STRING:
	{
		std::string strValue = jsValue;
		jsValue.EncodeEscapeCharacter(strValue);
		oss << '"' << strValue << '"';
	}
	break;
	case CJsonValue::JSON_TYPE_NUMBER:
		oss.precision(32);
		oss << static_cast<double>(jsValue);
		break;
	case CJsonValue::JSON_TYPE_OBJECT:
		oss << static_cast<const CJsonObject&>(jsValue);
		break;
	case CJsonValue::JSON_TYPE_ARRAY:
		oss << static_cast<const CJsonArray&>(jsValue);
		break;
	case CJsonValue::JSON_TYPE_BOOLEAN:
		oss << std::boolalpha << static_cast<bool>(jsValue);
		break;
	default:
		oss << "null";
		break;
	}

	return oss;
}

std::istream& operator >> (std::istream& iss, CJsonValue& jsValue)
{
	std::stringstream strParse;
	strParse << iss.rdbuf();

	jsValue.parse(strParse.str());
	return iss;
}
