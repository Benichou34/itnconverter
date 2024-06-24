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

#ifndef AFX_JAVASCRIPT_H_INCLUDED_
#define AFX_JAVASCRIPT_H_INCLUDED_

#include "Variant.h"
#include "ToolsLibrary/Raii.h"

class CJavaScript
{
public:
	CJavaScript(IDispatch* pDispatch);
	CJavaScript(CJavaScript&& javascript);
	~CJavaScript();

	CJavaScript& operator=(CJavaScript&& javascript);

	bool operator!() const;
	operator bool() const;

	CJavaScript& method(const std::string& name);
	CJavaScript& method(const std::wstring& name);

	CJavaScript& property(const std::string& name);
	CJavaScript& property(const std::wstring& name);

	CJavaScript& arg(const CVariant& var);
	CJavaScript& arg(CVariant&& var);

	CJavaScript& clear();
	CAutoVariant execute();
	void execute(CVariant& vResult);

private:
	enum
	{
		DispatchMethod = 0x1,
		DispatchPropertyGet = 0x2,
		DispatchPropertyPut = 0x4
	};

	autoDispatch<IDispatch> m_pDispatch;
	DISPID m_dispId;
	CAutoVariantList m_vecArg;
	WORD m_wFlag;
};

#endif // !AFX_JAVASCRIPT_H_INCLUDED_
