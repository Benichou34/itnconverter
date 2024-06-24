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
 * Purpose : Dispatch class for a CGeoLatLng
 */

#include "stdafx.h"
#include "ITN Tools.h"
#include <string>
#include "ArrayDispatch.h"

#if 0
namespace
{
	enum
	{
		DISPID_LENGTH = 0x40000000,
		DISPID_INDEX = 0x80000000
	};
}

CArrayDispatch::CArrayDispatch()
{
}

CArrayDispatch::~CArrayDispatch()
{
}

HRESULT STDMETHODCALLTYPE CArrayDispatch::GetIDsOfNames(
	/* [in] */ __RPC__in REFIID /*riid*/,
	/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
	/* [range][in] */ UINT cNames,
	/* [in] */ LCID /*lcid*/,
	/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId)
{
	static const std::wstring c_strLength(L"length");

	if (!cNames)
		return DISP_E_UNKNOWNNAME;

	std::wstring strName(rgszNames[0]);

	try
	{
		if (strName == c_strLength)
			*rgDispId = DISPID_LENGTH;
		else
			*rgDispId = DISPID_INDEX | (std::stoi(strName) & 0x0FFFFFFF);
	}
	catch (std::exception&)
	{
		return DISP_E_UNKNOWNNAME;
	}

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CArrayDispatch::Invoke(
	/* [in] */ DISPID dispIdMember,
	/* [in] */ REFIID /*riid*/,
	/* [in] */ LCID /*lcid*/,
	/* [in] */ WORD /*wFlags*/,
	/* [out][in] */ DISPPARAMS* /*pDispParams*/,
	/* [out] */ VARIANT* pVarResult,
	/* [out] */ EXCEPINFO* /*pExcepInfo*/,
	/* [out] */ UINT* /*puArgErr*/)
{
	if (!pVarResult)
		return DISP_E_BADPARAMCOUNT;

	switch (dispIdMember & 0xF0000000)
	{
	case DISPID_LENGTH:
		refResult = static_cast<unsigned long>(1);
		break;

	case DISPID_INDEX:
		try
		{
			//			dispIdMember & 0x0FFFFFFF;
		}
		catch (std::out_of_range&)
		{
			return DISP_E_OVERFLOW;
		}
		break;

	default:
		return DISP_E_MEMBERNOTFOUND;
	}

	return S_OK;
}
#endif
