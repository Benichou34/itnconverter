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

#include "GeoStep.h"

using namespace geo;

CGeoStep::CGeoStep()
{
}

CGeoStep::CGeoStep(const CGeoLatLng& gLatLng) : CGeoLocation(gLatLng)
{
}

CGeoStep::CGeoStep(const CGeoLocation& gLocation) : CGeoLocation(gLocation)
{
}

CGeoStep::CGeoStep(const CGeoStep& gStep) : CGeoLocation(gStep),
m_gSummary(gStep.m_gSummary),
m_strInstructions(gStep.m_strInstructions)
{
}

CGeoStep::CGeoStep(CGeoStep&& gStep) : CGeoLocation(std::move(gStep)),
m_gSummary(std::move(gStep.m_gSummary)),
m_strInstructions(std::move(gStep.m_strInstructions))
{
}

CGeoStep& CGeoStep::operator= (const CGeoLatLng& gLatLng)
{
	CGeoLocation::operator =(gLatLng);
	m_gSummary.clear();
	m_strInstructions.clear();

	return *this;
}

CGeoStep& CGeoStep::operator= (const CGeoLocation& gLocation)
{
	CGeoLocation::operator =(gLocation);
	m_gSummary.clear();
	m_strInstructions.clear();

	return *this;
}

CGeoStep& CGeoStep::operator=(const CGeoStep& gStep)
{
	CGeoLocation::operator =(gStep);
	m_gSummary = gStep.m_gSummary;
	m_strInstructions = gStep.m_strInstructions;

	return *this;
}

CGeoStep& CGeoStep::operator=(CGeoStep&& gStep)
{
	CGeoLocation::operator =(std::move(gStep));
	m_gSummary = std::move(gStep.m_gSummary);
	m_strInstructions = std::move(gStep.m_strInstructions);

	return *this;
}

void CGeoStep::clear()
{
	CGeoLocation::clear();
	m_gSummary.clear();
	m_strInstructions.clear();
}
