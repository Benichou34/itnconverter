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

#ifndef JSONARRAY_H_INCLUDED
#define JSONARRAY_H_INCLUDED

#include <memory>
#include <string>
#include <vector>

class CJsonValue;

class CJsonArray
{
public:
	friend std::ostream& operator << (std::ostream& oss, const CJsonArray& jsArray);

	CJsonArray();
	virtual ~CJsonArray();

	size_t size() const;
	bool empty() const;

	CJsonValue& operator[](size_t index);
	const CJsonValue& operator[](size_t index) const;

	CJsonValue& at(size_t index);
	const CJsonValue& at(size_t index) const;

	CJsonArray& operator+=(const CJsonArray& jsArray);
	CJsonArray& operator=(const CJsonArray& jsArray);

	void clear();
	void erase(size_t index);

	CJsonValue& add();
	void add(const CJsonValue& jsValue);

	CJsonValue& insert(size_t index);
	void insert(size_t index, const CJsonValue& jsValue);

	size_t parse(const std::string& strArray);
	std::string str() const;

private:
	std::vector<std::unique_ptr<CJsonValue>>::iterator GetIterator(size_t index);
	const std::unique_ptr<CJsonValue>& At(size_t index) const;

private:
	std::vector<std::unique_ptr<CJsonValue>> m_Array;
};

#endif // !JSONARRAY_H_INCLUDED
