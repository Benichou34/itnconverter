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

#ifndef _CPP_RAII_H
#define _CPP_RAII_H

 // Implementation of the Resource Acquisition Is Initialization (RAII) programming idiom
 // http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization

#if defined(_WIN32) && defined(DISPID_VALUE)
template <class T>
class autoDispatch
{
public:
	autoDispatch(T* lpDispatch = NULL, bool bAddRef = true) : m_lpDispatch(NULL)
	{
		reset(lpDispatch, bAddRef);
	}

	autoDispatch(IDispatch* pDispatch, REFIID iid) : m_lpDispatch(NULL)
	{
		QueryInterfaceFrom(pDispatch, iid);
	}

	autoDispatch(const autoDispatch& ad) : m_lpDispatch(NULL)
	{
		reset(ad.get());
	}

	autoDispatch(autoDispatch&& ad) : m_lpDispatch(NULL)
	{
		std::swap(m_lpDispatch, ad.m_lpDispatch);
	}

	~autoDispatch()
	{
		reset();
	}

	autoDispatch& operator=(const autoDispatch& ad) throw()
	{
		if (&ad != this)
			reset(ad.get());
		return *this;
	}

	autoDispatch& operator=(autoDispatch&& ad) throw()
	{
		if (&ad != this)
		{
			reset();
			std::swap(m_lpDispatch, ad.m_lpDispatch);
		}
		return *this;
	}

	operator bool() const throw()
	{
		return m_lpDispatch != NULL;
	}

	T* release() throw()
	{
		T* lpDispatch = m_lpDispatch;
		m_lpDispatch = NULL;
		return lpDispatch;
	}

	T& operator*() const throw()
	{
		return *m_lpDispatch;
	}

	T* operator->() const throw()
	{
		return m_lpDispatch;
	}

	void reset(T* lpDispatch = NULL, bool bAddRef = true) throw()
	{
		if (m_lpDispatch)
			m_lpDispatch->Release();
		m_lpDispatch = lpDispatch;
		if (m_lpDispatch && bAddRef)
			m_lpDispatch->AddRef();
	}

	T* get() const throw()
	{
		return m_lpDispatch;
	}

	void QueryInterfaceFrom(IDispatch* pDispatch, REFIID iid)
	{
		reset();

		if (pDispatch)
			pDispatch->QueryInterface(iid, reinterpret_cast<PVOID*>(&m_lpDispatch));
	}

private:
	T* m_lpDispatch;
};
#endif // _WIN32 && DISPID_VALUE

#endif // _CPP_RAII_H
