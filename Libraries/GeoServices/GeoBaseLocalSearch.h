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

#ifndef _GEO_BASE_LOCALSEARCH_H_INCLUDED_
#define _GEO_BASE_LOCALSEARCH_H_INCLUDED_

#include <vector>
#include <sstream>
#include <memory>
#include "GeoLocalSearch.h"
#include "GeoLocations.h"
#include "ToolsLibrary/Internet.h"

namespace geo
{
	class CGeoBaseLocalSearch : public IGeoLocalSearch
	{
	public:
		CGeoBaseLocalSearch();
		~CGeoBaseLocalSearch() override = default;

		void Load(const std::string& strText) override;
		void Load(const std::string& strText, const CGeoLatLng& gLocation, size_t radius = 0) override;
		void cancel() override;

		E_GEO_STATUS_CODE getStatus(size_t msTimeOut = InfiniteTimeOut) const override;
		const CGeoLocations& getLocations() const override;

		void setEndCallback(const CallbackFunction& EndCallback) override;

	protected:
		struct Request
		{
			std::string strUrl;
			std::string strPostData;
			std::string strReferrer;
		};

		virtual E_GEO_STATUS_CODE getRequestUrl(const std::string& strText, const CGeoLatLng& gLocation, size_t radius, Request& request) = 0;
		virtual E_GEO_STATUS_CODE parseRequest(const std::string& strRequest, CGeoLocations& gLocations) = 0;

		void callEndCallback();

	private:
		mutable E_GEO_STATUS_CODE m_eStatus;
		CallbackFunction m_EndCallback;
		std::unique_ptr<CInternetHttpSession> m_HttpSession;
		std::ostringstream m_oss;
		CGeoLocations m_GeoLocations;
	};
} // namespace geo

#endif // _GEO_BASE_LOCALSEARCH_H_INCLUDED_
