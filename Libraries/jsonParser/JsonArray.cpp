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
#include <algorithm>
#include <memory>
#include <stdexcept>
#include "JsonParser.h"

 //////////////////////////////////////////////////////////////////////
 // Construction/Destruction
 //////////////////////////////////////////////////////////////////////

CJsonArray::CJsonArray()
{
}

CJsonArray::~CJsonArray()
{
}

std::vector<std::unique_ptr<CJsonValue>>::iterator CJsonArray::GetIterator(size_t index)
{
	auto it = m_Array.begin();
	std::advance(it, index);

	return it;
}

size_t CJsonArray::size() const
{
	return m_Array.size();
}

bool CJsonArray::empty() const
{
	return m_Array.empty();
}

const std::unique_ptr<CJsonValue>& CJsonArray::At(size_t index) const
{
	try
	{
		return m_Array.at(index);
	}
	catch (std::out_of_range&)
	{
		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE);
	}
}

CJsonValue& CJsonArray::operator[](size_t index)
{
	return *At(index);
}

const CJsonValue& CJsonArray::operator[](size_t index) const
{
	return *At(index);
}

const CJsonValue& CJsonArray::at(size_t index) const
{
	return *At(index);
}

CJsonValue& CJsonArray::at(size_t index)
{
	return *At(index);
}

CJsonArray& CJsonArray::operator+=(const CJsonArray& jsArray)
{
	if (&jsArray == this)
		return *this;

	for (size_t i = 0; i < jsArray.size(); i++)
		add() = jsArray[i];

	return *this;
}

CJsonArray& CJsonArray::operator=(const CJsonArray& jsArray)
{
	if (&jsArray == this)
		return *this;

	clear();
	return operator+=(jsArray);
}

void CJsonArray::clear()
{
	m_Array.clear();
}

void CJsonArray::erase(size_t index)
{
	auto it = GetIterator(index);
	if (it == m_Array.end())
		throw CJsonException(CJsonParser::JSON_OUT_OF_RANGE);

	m_Array.erase(it);
}

CJsonValue& CJsonArray::add()
{
	m_Array.push_back(std::unique_ptr<CJsonValue>(new CJsonValue));
	return *m_Array.back();
}

void CJsonArray::add(const CJsonValue& jsValue)
{
	m_Array.push_back(std::make_unique<CJsonValue>(jsValue));
}

CJsonValue& CJsonArray::insert(size_t index)
{
	return **m_Array.insert(GetIterator(index), std::unique_ptr<CJsonValue>(new CJsonValue));
}

void CJsonArray::insert(size_t index, const CJsonValue& jsValue)
{
	m_Array.insert(GetIterator(index), std::make_unique<CJsonValue>(jsValue));
}

size_t CJsonArray::parse(const std::string& strArray)
{
	std::unique_ptr<CJsonValue> apValue;
	size_t nPos = 0;

	if (strArray[0] == '[')
		nPos++;

	while (nPos < strArray.length())
	{
		apValue.reset(new CJsonValue);
		nPos += apValue->parse(strArray.substr(nPos));
		if (apValue->getType() != CJsonValue::JSON_TYPE_NONE)
			m_Array.push_back(std::move(apValue));

		nPos = strArray.find_first_of(",]", nPos);
		if (nPos == std::string::npos)
			throw CJsonException(CJsonParser::JSON_ARRAY_ERROR);

		switch (strArray[nPos])
		{
		case ',': // Next Value
			nPos++;
			break;

		case ']': // End Array
			return nPos + 1;

		default:
			break;
		}
	}

	return nPos;
}

std::string CJsonArray::str() const
{
	std::ostringstream oss;
	oss << *this;
	return oss.str();
}

std::ostream& operator << (std::ostream& oss, const CJsonArray& jsArray)
{
	oss << '[';
	for (size_t i = 0; i < jsArray.size(); i++)
	{
		if (i > 0)
			oss << ',';

		oss << jsArray[i];
	}
	oss << ']';

	return oss;
}
