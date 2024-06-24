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

#include <algorithm>
#include "GeoBaseLocalSearch.h"
#include "ToolsLibrary/HttpClient.h"
#include "stdx/guard.h"

using namespace geo;

CGeoBaseLocalSearch::CGeoBaseLocalSearch() :
	m_eStatus(E_GEO_INVALID_REQUEST),
	m_HttpSession(new CInternetHttpSession)
{
}

void CGeoBaseLocalSearch::Load(const std::string& strText)
{
	Load(strText, CGeoLatLng());
}

void CGeoBaseLocalSearch::Load(const std::string& strText, const CGeoLatLng& gLocation, size_t radius)
{
	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_GeoLocations.clear();

	m_HttpSession->setEndCallback([&](CInternetHttpSession&, const CInternetException& inetException)
	{
		if (inetException.code() != ERROR_SUCCESS)
			m_eStatus = static_cast<E_GEO_STATUS_CODE>(E_HTTP_ERROR + inetException.code());
		else
			m_eStatus = parseRequest(m_oss.str(), m_GeoLocations);

		callEndCallback();
	});

	Request request;
	m_eStatus = getRequestUrl(strText, gLocation, radius, request);
	if (m_eStatus != E_GEO_OK)
	{
		callEndCallback();
		return;
	}

	try
	{
		m_oss.str(std::string());
		m_HttpSession->send(m_oss, request.strUrl, request.strPostData, request.strReferrer);
	}
	catch (CInternetException& inetException)
	{
		m_eStatus = static_cast<E_GEO_STATUS_CODE>(E_HTTP_ERROR + inetException.code());
		callEndCallback();
	}
}

void CGeoBaseLocalSearch::cancel()
{
	m_HttpSession->cancel();
}

const CGeoLocations& CGeoBaseLocalSearch::getLocations() const
{
	return m_GeoLocations;
}

E_GEO_STATUS_CODE CGeoBaseLocalSearch::getStatus(size_t msTimeOut) const
{
	try
	{
		if (!m_HttpSession->wait(msTimeOut))
			m_eStatus = E_GEO_TIMEOUT;
	}
	catch (CInternetException& inetException)
	{
		m_eStatus = static_cast<E_GEO_STATUS_CODE>(E_HTTP_ERROR + inetException.code());
	}

	return m_eStatus;
}

void CGeoBaseLocalSearch::setEndCallback(const CallbackFunction& EndCallback)
{
	m_EndCallback = EndCallback;
}

void CGeoBaseLocalSearch::callEndCallback()
{
	if (m_EndCallback)
		try { m_EndCallback(m_eStatus, m_GeoLocations); } catch (...) {}
}
