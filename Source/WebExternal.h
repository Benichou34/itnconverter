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
 * Purpose : Main Dispatch class for javascript
 */

#ifndef __WEB_EXTERNAL_H_
#define __WEB_EXTERNAL_H_

#include <map>
#include <memory>

class CNavPointView;
class CVariant;
class CNavigator;
class CAsyncHttpRequest;
class CAsyncRouteCalculation;

namespace geo
{
	class CGeoLatLng;
	class CGeoLocation;
}

/////////////////////////////////////////////////////////////////////////////
// CWebExternal
class CWebExternal : public CCmdTarget, public IDispatch
{
public: // Construction
	CWebExternal(CNavigator& navigator);
	virtual ~CWebExternal();

	void AddView(CNavPointView& cNavPointView, int nTabIndex);

private: // Implementation
	DECLARE_OLETYPELIB(CWebExternal)

		// IUnknown
		virtual HRESULT STDMETHODCALLTYPE QueryInterface(
			/* [in] */ REFIID riid,
			/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR* __RPC_FAR* ppvObject)
	{
		if (riid == IID_IDispatch)
		{
			*ppvObject = (LPUNKNOWN)this;
			this->AddRef();
			return S_OK;
		}

		return ExternalQueryInterface(&riid, ppvObject);
	}

	virtual ULONG STDMETHODCALLTYPE AddRef(void) { return ExternalAddRef(); }

	virtual ULONG STDMETHODCALLTYPE Release(void) { return ExternalRelease(); }

	// IDispatch
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
		/* [out] */ __RPC__out UINT*) {
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(
		/* [in] */ UINT,
		/* [in] */ LCID,
		/* [out] */ __RPC__deref_out_opt ITypeInfo**) {
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
		/* [in] */ __RPC__in REFIID riid,
		/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR* rgszNames,
		/* [range][in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID* rgDispId);

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS* pDispParams,
		/* [out] */ VARIANT* pVarResult,
		/* [out] */ EXCEPINFO* pExcepInfo,
		/* [out] */ UINT* puArgErr);

	geo::CGeoLocation GetLocation(const geo::CGeoLatLng& cgLatLng);
	geo::CGeoLocation GetRoadLocation(const geo::CGeoLatLng& cgLatLng);
	geo::CGeoLocation GetStreetViewLocation(const geo::CGeoLatLng& cgLatLng);
	geo::CGeoLatLng GetElevation(const geo::CGeoLatLng& cgLatLng);
	CNavPointView* RetrieveView(DISPPARAMS* pDispParams, int& nIndex);
	double GetDoubleFromVariant(const CVariant& variant);
	geo::CGeoLatLng GetLatLngFromVariant(const CVariant& variant);
	geo::CGeoLocation GetLocationFromVariant(const CVariant& varLatLng, const CVariant& varAddress);
	void AddPointToView(int nViewIndex, DISPPARAMS* pDispParams);

	void Trace(/*[in]*/ VARIANT& LOG, /*[out, retval]*/ VARIANT* retval);
	void MessageBox(/*[in]*/ VARIANT& MSG, /*[out, retval]*/ VARIANT* retval);
	void HttpRequest(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void AsyncHttpRequest(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void HttpDownload(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void GetPoint(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void SelectPoint(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void DeletePoint(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void RenamePoint(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void MovePoint(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void InsertPoint(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void AddPoint(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void AddFavorite(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void Calculate(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void SendToGps(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void GetLocation(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void GetRoadLocation(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void GetStreetViewLocation(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void ReverseGeocoding(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void GetElevation(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void GetRoutePreview(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void GetSettings(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void UseBase64(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void UseUtf8(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void GetStrCoords(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);
	void SetCurrentMap(/*[in]*/ DISPPARAMS* pDispParams, /*[out, retval]*/ VARIANT* retval);

private:
	CNavigator& m_Navigator;
	std::map<size_t, std::unique_ptr<CAsyncHttpRequest>> m_AsyncHttpRequests;
	std::unique_ptr<CAsyncRouteCalculation> m_AsyncRouteCalculation;
	std::map<int, CNavPointView*> m_mapNavPointView;
};

#endif //__WEB_EXTERNAL_H_
