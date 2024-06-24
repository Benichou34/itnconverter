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
#include "GeoLatLngs.h"

using namespace geo;

namespace
{
	bool removePred(const CGeoLatLng& gLatLng)
	{
		return !gLatLng;
	}
}

CGeoLatLngs::CGeoLatLngs()
{
}

CGeoLatLngs::CGeoLatLngs(const CGeoLatLngs& gLatLngs) : GeoLatLngs(gLatLngs)
{
}

CGeoLatLngs::CGeoLatLngs(CGeoLatLngs&& gLatLngs) : GeoLatLngs(std::move(gLatLngs))
{
}

CGeoLatLngs::CGeoLatLngs(const CGeoLatLngs& gLatLngs, size_t start, size_t count)
{
	const_iterator itFirst = gLatLngs.begin();
	const_iterator itLast = gLatLngs.end();

	std::advance(itFirst, start);

	if (count > 0)
	{
		itLast = itFirst;
		std::advance(itLast, count);
	}

	append(itFirst, itLast);
}

CGeoLatLngs::~CGeoLatLngs()
{
}

CGeoLatLngs& CGeoLatLngs::operator=(const CGeoLatLngs& gLatLngs)
{
	GeoLatLngs::operator =(gLatLngs);
	return *this;
}

CGeoLatLngs& CGeoLatLngs::operator=(CGeoLatLngs&& gLatLngs)
{
	GeoLatLngs::operator =(std::move(gLatLngs));
	return *this;
}

CGeoLatLngs& CGeoLatLngs::operator+=(const CGeoLatLngs& gLatLngs)
{
	if (&gLatLngs == this)
		return *this;

	if (empty())
		return operator=(gLatLngs);

	append(gLatLngs.begin(), gLatLngs.end());
	return *this;
}

CGeoLatLngs& CGeoLatLngs::operator+=(CGeoLatLngs&& gLatLngs)
{
	if (&gLatLngs == this)
		return *this;

	if (empty())
		return operator=(std::move(gLatLngs));

	for (GeoLatLngs::iterator it = gLatLngs.begin(); it != gLatLngs.end(); ++it)
		GeoLatLngs::push_back(std::move(*it));

	return *this;
}

size_t CGeoLatLngs::upper_bound() const
{
	return empty() ? 0 : size() - 1;
}

CGeoLatLng& CGeoLatLngs::at(size_t n)
{
	iterator it(begin());
	std::advance(it, n);
	return *it;
}

const CGeoLatLng& CGeoLatLngs::at(size_t n) const
{
	const_iterator it(begin());
	std::advance(it, n);
	return *it;
}

void CGeoLatLngs::removeDuplicates()
{
	sort();
	unique();
}

void CGeoLatLngs::removeEmpties()
{
	remove_if(removePred);
}
