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

#ifndef _GEO_STEP_H_INCLUDED_
#define _GEO_STEP_H_INCLUDED_

#include <deque>
#include "GeoLocation.h"
#include "GeoSummary.h"

namespace geo
{
	class CGeoStep : public CGeoLocation
	{
	public:
		CLONEABLE(CGeoStep);

		CGeoStep();
		CGeoStep(const CGeoLatLng& gLatLng);
		CGeoStep(const CGeoLocation& gLocation);
		CGeoStep(const CGeoStep& gStep);
		CGeoStep(CGeoStep&& gStep);
		virtual ~CGeoStep() = default;

		CGeoStep& operator= (const CGeoLatLng& gLatLng);
		CGeoStep& operator= (const CGeoLocation& gLocation);
		CGeoStep& operator= (const CGeoStep& gStep);
		CGeoStep& operator= (CGeoStep&& gStep);
		virtual void clear() override;

		const CGeoSummary& summary() const noexcept { return m_gSummary; }
		CGeoSummary& summary() noexcept { return m_gSummary; }

		const std::string& instructions() const noexcept { return m_strInstructions; }
		void instructions(const std::string& strInstructions) noexcept { m_strInstructions = strInstructions; }

	private:
		CGeoSummary m_gSummary;
		std::string m_strInstructions;
	};

	typedef std::deque<CGeoStep> GeoSteps;
} // namespace geo

#endif // _GEO_STEP_H_INCLUDED_
