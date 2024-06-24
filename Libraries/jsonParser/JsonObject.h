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

#ifndef JSONOBJECT_H_INCLUDED
#define JSONOBJECT_H_INCLUDED

#include <memory>
#include <string>
#include <deque>

class CJsonValue;

class CJsonObject
{
public:
	friend std::ostream& operator << (std::ostream& oss, const CJsonObject& jsObject);
	friend std::istream& operator >> (std::istream& iss, CJsonObject& jsObject);

	CJsonObject();
	CJsonObject(const CJsonObject& jsObject);
	CJsonObject(const std::string& strObject);
	virtual ~CJsonObject();

	CJsonObject& operator+=(const CJsonObject& jsObject);
	CJsonObject& operator=(const CJsonObject& jsObject);

	size_t size() const;
	bool exist(const std::string& xPath) const;

	CJsonValue& operator[](size_t pos);
	const CJsonValue& operator[](size_t pos) const;

	CJsonValue& at(size_t pos);
	const CJsonValue& at(size_t pos) const;

	CJsonValue& operator()(const std::string& strName);
	const CJsonValue& operator()(const std::string& strName) const;

	CJsonValue& at(const std::string& xPath);
	const CJsonValue& at(const std::string& xPath) const;

	CJsonValue& add(const std::string& strName);
	void add(const CJsonValue& jsValue);

	void clear();
	void erase(const std::string& strName);

	size_t parse(const std::string& strObject);
	std::string str() const;

private:
	void TrimSpaces(std::string& strName);
	CJsonValue* GetValue(size_t pos) const;
	CJsonValue* GetValue(const std::string& strName) const;
	CJsonValue* GetValueEx(const std::string& xPath) const;

private:
	std::deque<std::unique_ptr<CJsonValue>> m_Values;
};

#endif // !JSONOBJECT_H_INCLUDED
