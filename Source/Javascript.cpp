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
#include "Itn Tools.h"
#include "Javascript.h"

namespace
{
	DISPID GetIDsOfNamesFromDispatch(autoDispatch<IDispatch>& pDispatch, const std::wstring& name, bool bThrow)
	{
		if (!pDispatch)
			throw CWinApiException(DISP_E_UNKNOWNINTERFACE);

		DISPID dispId = DISPID_UNKNOWN;
		LPOLESTR lpOleStrMember = const_cast<wchar_t*>(name.c_str());
		HRESULT hresult = pDispatch->GetIDsOfNames(IID_NULL, &lpOleStrMember, 1, LOCALE_SYSTEM_DEFAULT, &dispId);
		if (FAILED(hresult) && bThrow)
			throw CWinApiException(hresult);

		return dispId;
	}
}

CJavaScript::CJavaScript(IDispatch* pDispatch) : m_pDispatch(pDispatch), m_dispId(DISPID_VALUE), m_wFlag(DispatchMethod)
{
	if (!m_pDispatch)
		throw CWinApiException(DISP_E_UNKNOWNINTERFACE);
}

CJavaScript::CJavaScript(CJavaScript&& javascript) :
	m_pDispatch(std::move(javascript.m_pDispatch)),
	m_dispId(std::move(javascript.m_dispId)),
	m_vecArg(std::move(javascript.m_vecArg))
{
}

CJavaScript::~CJavaScript()
{
}

CJavaScript& CJavaScript::operator=(CJavaScript&& javascript)
{
	m_pDispatch = std::move(javascript.m_pDispatch);
	std::swap(m_dispId, javascript.m_dispId);
	std::swap(m_vecArg, javascript.m_vecArg);

	return *this;
}

CJavaScript& CJavaScript::method(const std::string& name)
{
	return method(stdx::string_helper::widen(name));
}

CJavaScript& CJavaScript::method(const std::wstring& name)
{
	m_dispId = GetIDsOfNamesFromDispatch(m_pDispatch, name, false);
	m_wFlag = DispatchMethod;

#if defined(_DEBUG) || defined(FORCE_LOG)
	CITNConverterApp::wlog() << clock() << L": CJavaScript::method(" << name << L") = " << std::dec << m_dispId << std::endl;
#endif //_DEBUG

	return *this;
}

CJavaScript& CJavaScript::property(const std::string& name)
{
	return property(stdx::string_helper::widen(name));
}

CJavaScript& CJavaScript::property(const std::wstring& name)
{
	m_dispId = GetIDsOfNamesFromDispatch(m_pDispatch, name, false);
	m_wFlag = DispatchPropertyGet;

#if defined(_DEBUG) || defined(FORCE_LOG)
	CITNConverterApp::wlog() << clock() << L": CJavaScript::property(" << name << L") = " << std::dec << m_dispId << std::endl;
#endif //_DEBUG

	return *this;
}

CJavaScript& CJavaScript::arg(const CVariant& var)
{
	m_vecArg.push_back(var);
	return *this;
}

CJavaScript& CJavaScript::arg(CVariant&& var)
{
	m_vecArg.push_back(std::move(var));
	return *this;
}

CJavaScript& CJavaScript::clear()
{
	m_vecArg.clear();
	return *this;
}

bool CJavaScript::operator!() const
{
	if (!m_pDispatch)
		throw CWinApiException(DISP_E_UNKNOWNINTERFACE);

	return m_dispId == DISPID_UNKNOWN;
}

CJavaScript::operator bool() const
{
	return !operator!();
}

CAutoVariant CJavaScript::execute()
{
	CAutoVariant vResult;
	execute(vResult);
	return vResult;
}

void CJavaScript::execute(CVariant& vResult)
{
	if (!m_pDispatch)
		throw CWinApiException(DISP_E_UNKNOWNINTERFACE);

	if (m_dispId == DISPID_UNKNOWN)
		throw CWinApiException(DISP_E_UNKNOWNNAME);

	// The arguments in the array should be placed from last to first, so rgvarg[0] has the last argument and rgvarg[cArgs - 1] has the first argument.
	DISPPARAMS DispParams = { nullptr, nullptr, 0, 0 };
	std::vector<VARIANT> dispVariants;

#if defined(_DEBUG) || defined(FORCE_LOG)
	CITNConverterApp::wlog() << clock() << L": CJavaScript::execute[" << std::dec << m_dispId << L"](" << stdx::wformat().array(m_vecArg.begin(), m_vecArg.end(), L", ") << L")" << std::endl;
#endif //_DEBUG

	if (!m_vecArg.empty())
	{
		if (m_wFlag == DispatchPropertyGet)
			m_wFlag = DispatchPropertyPut;

		dispVariants.reserve(m_vecArg.size());
		for (auto it = m_vecArg.rbegin(); it != m_vecArg.rend(); ++it)
			dispVariants.push_back(it->variant());

		DispParams.cArgs = dispVariants.size();
		DispParams.rgvarg = &dispVariants.front();
	}

	vResult.clear();
	HRESULT hResult = m_pDispatch->Invoke(m_dispId, IID_NULL, LOCALE_SYSTEM_DEFAULT, m_wFlag, &DispParams, &vResult.variant(), nullptr, nullptr);
	if (FAILED(hResult))
		throw CWinApiException(hResult);

#if defined(_DEBUG) || defined(FORCE_LOG)
	CITNConverterApp::wlog() << clock() << L": CJavaScript::execute[" << std::dec << m_dispId << L"] = " << vResult << std::endl;
#endif //_DEBUG
}
