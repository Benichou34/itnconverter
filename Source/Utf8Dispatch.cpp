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
 *
 * Purpose : Dispatch class for UTF-8 Coder/Decoder
 */

#include "stdafx.h"
#include "Utf8Dispatch.h"
#include "stdx/string_helper.h"
#include "Variant.h"

namespace
{
	enum
	{
		DISPID_ENCODE = 1,
		DISPID_DECODE
	};

	const std::wstring c_wstrEncode(L"Encode");
	const std::wstring c_wstrDecode(L"Decode");
}

HRESULT STDMETHODCALLTYPE CUtf8Dispatch::GetIDsOfNames(
	/* [in] */ __RPC__in REFIID /*riid*/,
	/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
	/* [range][in] */ UINT cNames,
	/* [in] */ LCID /*lcid*/,
	/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
{
	if (!cNames)
		return DISP_E_UNKNOWNNAME;

	std::wstring strName(rgszNames[0]);

	if (strName == c_wstrEncode)
		*rgDispId = DISPID_ENCODE;
	else if (strName == c_wstrDecode)
		*rgDispId = DISPID_DECODE;
	else
		return DISP_E_UNKNOWNNAME;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CUtf8Dispatch::Invoke(
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID /*riid*/,
	/* [in] */ LCID /*lcid*/,
	/* [in] */ WORD /*wFlags*/,
	/* [out][in] */ DISPPARAMS* pDispParams,
	/* [out] */ VARIANT* pVarResult,
	/* [out] */ EXCEPINFO* /*pExcepInfo*/,
	/* [out] */ UINT* /*puArgErr*/)
{
	if (pDispParams->cArgs < 1 || !pVarResult)
		return DISP_E_BADPARAMCOUNT;

	CVariantList args(*pDispParams);

	if (args.front().type() != VT_BSTR)
		return DISP_E_BADVARTYPE;

	switch (dispIdMember)
	{
	case DISPID_ENCODE:
		*pVarResult = CVariant(stdx::wstring_helper::to_utf8(args.front().asWString())).variant();
		break;

	case DISPID_DECODE:
		*pVarResult = CVariant(stdx::wstring_helper::from_utf8(args.front().asString())).variant();
		break;

	default:
		return DISP_E_MEMBERNOTFOUND;
	}

	return S_OK;
}
