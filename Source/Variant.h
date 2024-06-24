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

#ifndef AFX_VARIANT_H_INCLUDED_
#define AFX_VARIANT_H_INCLUDED_

#include <string>
#include <vector>
#include <OAIdl.h>

class CVariant
{
public:
	CVariant();
	CVariant(const CVariant& variant);
	CVariant(const VARIANT& variant);
	template<class T> CVariant(const T& t)
	{
		VariantInit(&m_variant);
		operator=(t);
	}

	bool operator!() const;

	CVariant& operator=(const CVariant& variant);
	CVariant& operator=(const VARIANT& variant);
	CVariant& operator=(char cArg);
	CVariant& operator=(unsigned char ucArg);
	CVariant& operator=(short sArg);
	CVariant& operator=(unsigned short usArg);
	CVariant& operator=(int nArg);
	CVariant& operator=(unsigned int unArg);
	CVariant& operator=(long lArg);
	CVariant& operator=(unsigned long ulArg);
	CVariant& operator=(long long llArg);
	CVariant& operator=(unsigned long long ullArg);
	CVariant& operator=(float fArg);
	CVariant& operator=(double dArg);
	CVariant& operator=(bool bArg);
	CVariant& operator=(const std::string& strArg);
	CVariant& operator=(const std::wstring& strArg);
	CVariant& operator=(IDispatch* pDispatch);

	char asChar() const;
	unsigned char asUChar() const;
	short asShort() const;
	unsigned short asUShort() const;
	int asInteger() const;
	unsigned int asUInteger() const;
	long asLong() const;
	unsigned long asULong() const;
	long long asLongLong() const;
	unsigned long long asULongLong() const;
	float asFloat() const;
	double asDouble() const;
	bool asBool() const;
	std::string asString() const;
	std::wstring asWString() const;
	IDispatch* asDispatch() const;

	const VARIANT& variant() const;
	VARIANT& variant();
	VARTYPE type() const;
	CVariant& changeType(VARTYPE varType);

	void clear();
	void copyTo(VARIANT& variant) const;
	void copyTo(CVariant& variant) const;

private:
	VARIANT m_variant;
};

std::ostream& operator << (std::ostream& oss, const CVariant& variant);
std::wostream& operator << (std::wostream& oss, const CVariant& variant);

class CAutoVariant : public CVariant
{
public:
	CAutoVariant();
	CAutoVariant(const CAutoVariant& variant);
	CAutoVariant(const CVariant& variant);
	CAutoVariant(CAutoVariant&& variant);
	template<class T> CAutoVariant(T& t)
	{
		CVariant::operator=(t);
	}
	~CAutoVariant();

	CAutoVariant& operator=(const CAutoVariant& variant);
	CAutoVariant& operator=(const CVariant& variant);
	CAutoVariant& operator=(CAutoVariant&& variant);
	template<class T> CAutoVariant& operator=(const T& t)
	{
		CVariant::operator=(t); return *this;
	}

	CVariant release();
};

class CAutoVariantList : public std::vector<CAutoVariant>
{
public:
	CAutoVariantList();
	CAutoVariantList(const std::vector<CAutoVariant>& variants);
	CAutoVariantList(std::vector<CAutoVariant>&& variants);
	CAutoVariantList(const DISPPARAMS& dispParams);

	CAutoVariantList& operator=(const std::vector<CAutoVariant>& variants);
	CAutoVariantList& operator=(std::vector<CAutoVariant>&& variants);
	CAutoVariantList& operator=(const DISPPARAMS& dispParams);

	void release();
};

class CVariantList : public std::vector<CVariant>
{
public:
	CVariantList(DISPPARAMS& dispParams);
};

#endif // !AFX_VARIANT_H_INCLUDED_
