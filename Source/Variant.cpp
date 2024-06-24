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

#include "stdafx.h"
#include <algorithm>
#include <OleAuto.h>
#include "ITN Tools.h"
#include "Variant.h"

namespace
{
	template<class _charT>
	void printVariantString(std::basic_ostream<_charT>& bos, const CVariant& variant);

	template<>
	void printVariantString(std::basic_ostream<char>& bos, const CVariant& variant) { bos << variant.asString(); }

	template<>
	void printVariantString(std::basic_ostream<wchar_t>& bos, const CVariant& variant) { bos << variant.asWString(); }

	template<class _charT>
	void printVariant(std::basic_ostream<_charT>& bos, const CVariant& variant)
	{
		switch (variant.type())
		{
		case VT_I1:
			bos << std::dec << variant.asChar();
			break;
		case VT_UI1:
			bos << std::dec << variant.asUChar();
			break;
		case VT_I2:
			bos << std::dec << variant.asShort();
			break;
		case VT_UI2:
			bos << std::dec << variant.asUShort();
			break;
		case VT_INT:
			bos << std::dec << variant.asInteger();
			break;
		case VT_UINT:
			bos << std::dec << variant.asUInteger();
			break;
		case VT_I4:
			bos << std::dec << variant.asLong();
			break;
		case VT_UI4:
			bos << std::dec << variant.asULong();
			break;
		case VT_I8:
			bos << std::dec << variant.asLongLong();
			break;
		case VT_UI8:
			bos << std::dec << variant.asULongLong();
			break;
		case VT_R4:
			bos << std::dec << std::fixed << variant.asFloat();
			break;
		case VT_R8:
			bos << std::dec << std::fixed << variant.asDouble();
			break;
		case VT_BOOL:
			bos << std::boolalpha << variant.asBool();
			break;
		case VT_BSTR:
			printVariantString(bos, variant);
			break;
		case VT_DISPATCH:
			bos << std::hex << variant.asDispatch();
			break;
		case VT_EMPTY:
			bos << '#';
			break;
		default:
			bos << '[' << variant.type() << ']';
			break;
		}
	}
}

CVariant::CVariant()
{
	VariantInit(&m_variant);
}

CVariant::CVariant(const CVariant& variant) : m_variant(variant.m_variant)
{ }

CVariant::CVariant(const VARIANT& variant) : m_variant(variant)
{ }

bool CVariant::operator!() const
{
	return type() == VT_EMPTY;
}

CVariant& CVariant::operator=(const CVariant& variant)
{
	return operator=(variant.m_variant);
}

CVariant& CVariant::operator=(const VARIANT& variant)
{
	HRESULT hResult = VariantCopy(&m_variant, &variant);
	if (FAILED(hResult))
		throw CWinApiException(hResult);
	return *this;
}

CVariant& CVariant::operator=(char cArg)
{
	m_variant.vt = VT_I1;
	m_variant.cVal = cArg;
	return *this;
}

CVariant& CVariant::operator=(unsigned char ucArg)
{
	m_variant.vt = VT_UI1;
	m_variant.bVal = ucArg;
	return *this;
}

CVariant& CVariant::operator=(short sArg)
{
	m_variant.vt = VT_I2;
	m_variant.iVal = sArg;
	return *this;
}

CVariant& CVariant::operator=(unsigned short usArg)
{
	m_variant.vt = VT_UI2;
	m_variant.uiVal = usArg;
	return *this;
}

CVariant& CVariant::operator=(int nArg)
{
	m_variant.vt = VT_INT;
	m_variant.intVal = nArg;
	return *this;
}

CVariant& CVariant::operator=(unsigned int unArg)
{
	m_variant.vt = VT_UINT;
	m_variant.uintVal = unArg;
	return *this;
}

CVariant& CVariant::operator=(long lArg)
{
	m_variant.vt = VT_I4;
	m_variant.lVal = lArg;
	return *this;
}

CVariant& CVariant::operator=(unsigned long ulArg)
{
	m_variant.vt = VT_UI4;
	m_variant.ulVal = ulArg;
	return *this;
}

CVariant& CVariant::operator=(long long llArg)
{
	m_variant.vt = VT_I8;
	m_variant.llVal = llArg;
	return *this;
}

CVariant& CVariant::operator=(unsigned long long ullArg)
{
	m_variant.vt = VT_UI8;
	m_variant.ullVal = ullArg;
	return *this;
}

CVariant& CVariant::operator=(float fArg)
{
	m_variant.vt = VT_R4;
	m_variant.fltVal = fArg;
	return *this;
}

CVariant& CVariant::operator=(double dArg)
{
	m_variant.vt = VT_R8;
	m_variant.dblVal = dArg;
	return *this;
}

CVariant& CVariant::operator=(bool bArg)
{
	m_variant.vt = VT_BOOL;
	m_variant.boolVal = bArg ? VARIANT_TRUE : VARIANT_FALSE;
	return *this;
}

CVariant& CVariant::operator=(const std::string& strArg)
{
	return operator=(stdx::wstring_helper::from_utf8(strArg));
}

CVariant& CVariant::operator=(const std::wstring& strArg)
{
	m_variant.vt = VT_BSTR;
	m_variant.bstrVal = SysAllocString(strArg.c_str());
	return *this;
}

CVariant& CVariant::operator=(IDispatch* pDispatch)
{
	if (pDispatch)
		pDispatch->AddRef();

	m_variant.vt = VT_DISPATCH;
	m_variant.pdispVal = pDispatch;
	return *this;
}

char CVariant::asChar() const
{
	return m_variant.cVal;
}

unsigned char CVariant::asUChar() const
{
	return m_variant.bVal;
}

short CVariant::asShort() const
{
	return m_variant.iVal;
}

unsigned short CVariant::asUShort() const
{
	return m_variant.uiVal;
}

int CVariant::asInteger() const
{
	return m_variant.intVal;
}

unsigned int CVariant::asUInteger() const
{
	return m_variant.uintVal;
}

long CVariant::asLong() const
{
	return m_variant.lVal;
}

unsigned long CVariant::asULong() const
{
	return m_variant.ulVal;
}

long long CVariant::asLongLong() const
{
	return m_variant.llVal;
}

unsigned long long CVariant::asULongLong() const
{
	return m_variant.ullVal;
}

float CVariant::asFloat() const
{
	return m_variant.fltVal;
}

double CVariant::asDouble() const
{
	return m_variant.dblVal;
}

bool CVariant::asBool() const
{
	return m_variant.boolVal != VARIANT_FALSE;
}

std::string CVariant::asString() const
{
	return stdx::wstring_helper::to_utf8(asWString());
}

std::wstring CVariant::asWString() const
{
	return m_variant.bstrVal;
}

IDispatch* CVariant::asDispatch() const
{
	return m_variant.pdispVal;
}

VARIANT& CVariant::variant()
{
	return m_variant;
}

const VARIANT& CVariant::variant() const
{
	return m_variant;
}

VARTYPE CVariant::type() const
{
	return m_variant.vt;
}

CVariant& CVariant::changeType(VARTYPE varType)
{
	HRESULT hResult = VariantChangeType(&m_variant, &m_variant, 0, varType);
	if (FAILED(hResult))
		throw CWinApiException(hResult);

	return *this;
}

void CVariant::clear()
{
	VariantClear(&m_variant);
}

void CVariant::copyTo(VARIANT& variant) const
{
	HRESULT hResult = VariantCopy(&variant, &m_variant);
	if (FAILED(hResult))
		throw CWinApiException(hResult);
}

void CVariant::copyTo(CVariant& variant) const
{
	copyTo(variant.m_variant);
}

std::ostream& operator<<(std::ostream& oss, const CVariant& variant)
{
	printVariant(oss, variant);
	return oss;
}

std::wostream& operator<<(std::wostream& oss, const CVariant& variant)
{
	printVariant(oss, variant);
	return oss;
}

CAutoVariant::CAutoVariant()
{
}

CAutoVariant::CAutoVariant(const CAutoVariant& variant)
{
	variant.copyTo(*this);
}

CAutoVariant::CAutoVariant(const CVariant& variant) : CVariant(variant)
{

}

CAutoVariant::CAutoVariant(CAutoVariant&& variant) : CVariant(variant)
{
	variant.clear();
}

CAutoVariant::~CAutoVariant()
{
	clear();
}

CAutoVariant& CAutoVariant::operator=(const CAutoVariant& var)
{
	clear();
	var.copyTo(*this);
	return *this;
}

CAutoVariant& CAutoVariant::operator=(const CVariant& var)
{
	clear();
	CVariant::operator=(var);
	return *this;
}

CAutoVariant& CAutoVariant::operator=(CAutoVariant&& var)
{
	clear();
	std::swap(var.variant(), this->variant());
	return *this;
}

CVariant CAutoVariant::release()
{
	CVariant var;
	std::swap(var.variant(), this->variant());
	return var;
}

CAutoVariantList::CAutoVariantList()
{
}

CAutoVariantList::CAutoVariantList(const std::vector<CAutoVariant>& variants)
{
	operator=(variants);
}

CAutoVariantList::CAutoVariantList(std::vector<CAutoVariant>&& variants)
{
	std::swap(*this, variants);
}

CAutoVariantList::CAutoVariantList(const DISPPARAMS& dispParams)
{
	operator=(dispParams);
}

CAutoVariantList& CAutoVariantList::operator=(const std::vector<CAutoVariant>& variants)
{
	assign(variants.begin(), variants.end());
	return *this;
}

CAutoVariantList& CAutoVariantList::operator=(std::vector<CAutoVariant>&& variants)
{
	clear();
	std::swap(*this, variants);
	return *this;
}

CAutoVariantList& CAutoVariantList::operator=(const DISPPARAMS& dispParams)
{
	resize(dispParams.cArgs);
	std::copy(dispParams.rgvarg, dispParams.rgvarg + dispParams.cArgs, rbegin());
	return *this;
}

void CAutoVariantList::release()
{
	for (auto it = begin(); it != end(); ++it)
		it->release();
	clear();
}

CVariantList::CVariantList(DISPPARAMS& dispParams)
{
	if (dispParams.cArgs)
	{
		for (VARIANTARG* pv = dispParams.rgvarg + dispParams.cArgs - 1; pv >= dispParams.rgvarg; --pv)
			emplace_back(*pv);
	}
}
