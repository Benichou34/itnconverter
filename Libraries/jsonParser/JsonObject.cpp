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
#include <memory>
#include "JsonParser.h"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

CJsonObject::CJsonObject()
{
}

CJsonObject::CJsonObject(const CJsonObject& jsObject)
{
	operator+=(jsObject);
}

CJsonObject::CJsonObject(const std::string& strObject)
{
	parse(strObject);
}

CJsonObject::~CJsonObject()
{
}

CJsonObject& CJsonObject::operator+=(const CJsonObject& jsObject)
{
	if (&jsObject == this)
		return *this;

	for (size_t i = 0; i < jsObject.size(); i++)
	{
		const CJsonValue* pjsValue = jsObject.GetValue(i);
		add(pjsValue->getName()) = *pjsValue;
	}

	return *this;
}

CJsonObject& CJsonObject::operator=(const CJsonObject& jsObject)
{
	if (&jsObject == this)
		return *this;

	clear();
	return operator+=(jsObject);
}

size_t CJsonObject::size() const
{
	return m_Values.size();
}

bool CJsonObject::exist(const std::string& xPath) const
{
	return (GetValueEx(xPath) != nullptr);
}

void CJsonObject::TrimSpaces(std::string& strName)
{
	// Left trim
	std::string::iterator it = strName.begin();

	while (it != strName.end() && *it == ' ')
		++it;

	strName.erase(strName.begin(), it);

	// Right trim
	std::string::reverse_iterator rit = strName.rbegin();

	while (rit != strName.rend() && *rit == ' ')
		++rit;

	strName.resize(std::distance(rit, strName.rend()));
}

CJsonValue* CJsonObject::GetValue(size_t pos) const
{
	auto it = m_Values.begin();
	std::advance(it, pos);

	return (it != m_Values.end()) ? it->get() : nullptr;
}

CJsonValue* CJsonObject::GetValue(const std::string& strName) const
{
	for (auto it = m_Values.begin(); it != m_Values.end(); ++it)
	{
		if ((*it)->getName() == strName)
			return it->get();
	}

	return nullptr;
}

CJsonValue* CJsonObject::GetValueEx(const std::string& xPath) const
{
	CJsonValue* pJsonValue = nullptr;

	size_t pos = xPath.find_first_of("\\/");
	if (pos == std::string::npos)
	{
		pJsonValue = GetValue(xPath);
	}
	else
	{
		std::string path = xPath.substr(0, pos);
		pJsonValue = GetValue(path);
		if (pJsonValue && pJsonValue->getType() == CJsonValue::JSON_TYPE_OBJECT)
			pJsonValue = static_cast<const CJsonObject&>(*pJsonValue).GetValueEx(xPath.substr(pos + 1));
		else
			pJsonValue = nullptr;
	}

	return pJsonValue;
}

CJsonValue& CJsonObject::operator[](size_t pos)
{
	return at(pos);
}

const CJsonValue& CJsonObject::operator[](size_t pos) const
{
	return at(pos);
}

CJsonValue& CJsonObject::at(size_t pos)
{
	CJsonValue* pValue = GetValue(pos);
	if (!pValue)
		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE);

	return *pValue;
}

const CJsonValue& CJsonObject::at(size_t pos) const
{
	CJsonValue* pValue = GetValue(pos);
	if (!pValue)
		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE);

	return *pValue;
}

CJsonValue& CJsonObject::operator()(const std::string& strName)
{
	CJsonValue* pValue = GetValue(strName);
	if (!pValue)
		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE, strName);

	return *pValue;
}

const CJsonValue& CJsonObject::operator()(const std::string& strName) const
{
	CJsonValue* pValue = GetValue(strName);
	if (!pValue)
		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE, strName);

	return *pValue;
}

CJsonValue& CJsonObject::at(const std::string& xPath)
{
	CJsonValue* pValue = GetValueEx(xPath);
	if (!pValue)
		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE, xPath);

	return *pValue;
}

const CJsonValue& CJsonObject::at(const std::string& xPath) const
{
	CJsonValue* pValue = GetValueEx(xPath);
	if (!pValue)
		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE, xPath);

	return *pValue;
}

void CJsonObject::clear()
{
	m_Values.clear();
}

CJsonValue& CJsonObject::add(const std::string& xPath)
{
	size_t pos = xPath.find_first_of("\\/");
	if (pos == std::string::npos)
	{
		if (GetValue(xPath))
			throw CJsonException(CJsonParser::JSON_ALREADY_EXIT, xPath);

		m_Values.push_back(std::make_unique<CJsonValue>(xPath));
		return *m_Values.back();
	}
	else
	{
		CJsonValue* pValue = GetValue(xPath.substr(0, pos));
		if (!pValue)
		{
			pValue = &add(xPath.substr(0, pos));
			pValue->setType(CJsonValue::JSON_TYPE_OBJECT);
		}

		return static_cast<CJsonObject&>(*pValue).add(xPath.substr(pos + 1));
	}
}

void CJsonObject::add(const CJsonValue& jsValue)
{
	if (GetValue(jsValue.getName()))
		throw CJsonException(CJsonParser::JSON_ALREADY_EXIT, jsValue.getName());

	m_Values.push_back(std::make_unique<CJsonValue>(jsValue));
}

void CJsonObject::erase(const std::string& xPath)
{
	size_t pos = xPath.find_last_of("\\/");
	if (pos == std::string::npos)
	{
		for (auto it = m_Values.begin(); it != m_Values.end(); ++it)
		{
			if ((*it)->getName() == xPath)
			{
				m_Values.erase(it);
				return;
			}
		}

		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE, xPath);
	}
	else
	{
		static_cast<CJsonObject&>(at(xPath.substr(0, pos))).erase(xPath.substr(pos + 1));
	}
}

size_t CJsonObject::parse(const std::string& strObject)
{
	size_t nPos = 0;
	size_t nPosFirst = 0;
	std::string strName;
	std::unique_ptr<CJsonValue> apValue;

	if (strObject[0] == '{')
		nPos++;

	size_t nBegin = nPos;

	while (nPos < strObject.length())
	{
		nPos = strObject.find_first_of("\"{[,:}", nPos);
		if (nPos == std::string::npos)
			throw CJsonException(CJsonParser::JSON_OBJECT_ERROR);

		apValue.reset();

		switch (strObject[nPos])
		{
		case ',': // Next Value
			nPos++;
			nBegin = nPos;
			break;

		case ':': // Value
			if (!nPosFirst)
			{
				if (strName.empty())
				{
					strName = strObject.substr(nBegin, nPos - nBegin);
					TrimSpaces(strName);
				}

				nPos++;
			}
		case '{': // Object
		case '[': // Array
			if (!nPosFirst)
			{
				apValue.reset(new CJsonValue(strName));
				nPos += apValue->parse(strObject.substr(nPos));
			}
			else
			{
				nPos++;
			}
			break;

		case '\"': // Name
			if (!nPosFirst)
			{
				nPos++;
				nPosFirst = nPos;
			}
			else if (strObject[nPos - 1] == '\\')
			{
				nPos++;
			}
			else
			{
				strName = strObject.substr(nPosFirst, nPos - nPosFirst);
				nPosFirst = 0;
				nPos++;
			}
			break;

		case '}': // End Object
			if (!nPosFirst)
				return nPos + 1;
			else
				nPos++;

		default:
			break;
		}

		if (apValue.get() && apValue->getType() != CJsonValue::JSON_TYPE_NONE)
		{
			m_Values.push_back(std::move(apValue));
			strName.clear();
		}
	}

	return nPos;
}

std::string CJsonObject::str() const
{
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

std::ostream& operator << (std::ostream& oss, const CJsonObject& jsObject)
{
	const CJsonValue* pValue = nullptr;
	size_t i = 0;

	oss << '{';
	do
	{
		pValue = jsObject.GetValue(i);
		if (pValue)
		{
			if (i > 0)
				oss << ',';

			oss << *pValue;
		}
		i++;
	} while (pValue);
	oss << '}';

	return oss;
}

std::istream& operator >> (std::istream& iss, CJsonObject& jsObject)
{
	std::stringstream strParse;
	strParse << iss.rdbuf();

	jsObject.parse(strParse.str());
	return iss;
}
