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
#include "GeoBaseDirections.h"
#include "ToolsLibrary/HttpClient.h"

using namespace geo;

CGeoBaseDirections::CGeoBaseDirections() :
	m_eStatus(E_GEO_INVALID_REQUEST),
	m_HttpSession(new CInternetHttpSession),
	m_vehicleType(GeoVehicleType::Default)
{
}

void CGeoBaseDirections::Load(const CGeoLatLng& gStart, const CGeoLatLng& gStop, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions)
{
	CGeoLatLngs cgLatLngs;

	cgLatLngs.push_back(gStart);
	cgLatLngs.push_back(gStop);

	Load(cgLatLngs, vehicleType, cgOptions);
}

void CGeoBaseDirections::Load(const CGeoLatLngs& cgLatLngs, GeoVehicleType::type_t vehicleType, const CGeoRouteOptions& cgOptions)
{
	m_eStatus = E_GEO_UNKNOWN_ERROR;
	m_vecRoutes.clear();

	size_t maxStep = getMaximumStepsByRequest();

	if (cgLatLngs.size() < 2 || maxStep < 2)
	{
		 m_eStatus = E_GEO_BAD_ARGUMENTS;
		 callEndCallback();
		 return;
	}

	m_vecRoutes.reserve((cgLatLngs.size() > maxStep)? (cgLatLngs.size() / maxStep + 1): 1);
	size_t idxSteps = 0;
	size_t leftSteps = cgLatLngs.size();

	m_vecGeoLatLngs.clear();
	while (leftSteps > 0)
	{
		size_t curSteps = std::min(leftSteps, maxStep);
		m_vecGeoLatLngs.emplace_back(CGeoLatLngs(cgLatLngs, idxSteps, curSteps));
		
		if (curSteps < leftSteps)
			--curSteps;
		
		idxSteps += curSteps;
		leftSteps -= curSteps;
	}

	m_itLatLngs = m_vecGeoLatLngs.begin();
	m_vehicleType = vehicleType;
	m_cgOptions = cgOptions;

	m_HttpSession->setEndCallback([&](CInternetHttpSession&, const CInternetException& inetException)
	{
		if (inetException.code() != ERROR_SUCCESS)
		{
			m_eStatus = static_cast<E_GEO_STATUS_CODE>(E_HTTP_ERROR + inetException.code());
			callEndCallback();
			return;
		}

		m_eStatus = parseRequest(m_oss.str(), m_vehicleType, *m_cgOptions, m_vecRoutes);
		if (m_eStatus != E_GEO_OK)
		{
			callEndCallback();
			return;
		}
		
		++m_itLatLngs;
		sendNextRequest();
	});

	sendNextRequest();
}

void CGeoBaseDirections::cancel()
{
	m_HttpSession->cancel();
}

const GeoRoutes& CGeoBaseDirections::getRoutes() const
{
	return m_vecRoutes;
}

E_GEO_STATUS_CODE CGeoBaseDirections::getStatus(size_t msTimeOut) const
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

void CGeoBaseDirections::setEndCallback(const CallbackFunction& EndCallback)
{
	m_EndCallback = EndCallback;
}

void CGeoBaseDirections::callEndCallback()
{
	if (m_EndCallback)
		try { m_EndCallback(m_eStatus, m_vecRoutes); } catch (...) {}
}

void CGeoBaseDirections::sendNextRequest()
{
	if (m_itLatLngs == m_vecGeoLatLngs.end())
	{
		if (m_eStatus == E_GEO_OK)
			m_eStatus = m_vecRoutes.empty() ? E_GEO_ZERO_RESULTS : E_GEO_OK;

		callEndCallback();
		return;
	}

	Request request;
	m_eStatus = getRequestUrl(*m_itLatLngs, m_vehicleType, *m_cgOptions, request);
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
