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

#include "stdafx.h"
#include <algorithm>
#include "GpsPointArray.h"

namespace
{
	bool uniquePred(const CGpsPoint& gpsPoint1, const CGpsPoint& gpsPoint2)
	{
		return static_cast<const geo::CGeoLatLng&>(gpsPoint1) == static_cast<const geo::CGeoLatLng&>(gpsPoint2);
	}

	bool sortPred(const CGpsPoint& gpsPoint1, const CGpsPoint& gpsPoint2)
	{
		return gpsPoint1.name() < gpsPoint2.name();
	}

	bool removePred(const CGpsPoint& gpsPoint)
	{
		return !gpsPoint;
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGpsPointArray::CGpsPointArray() : std::deque<CGpsPoint>()
{
}

CGpsPointArray::CGpsPointArray(const CGpsPointArray& array) :
	std::deque<CGpsPoint>(array),
	m_sName(array.m_sName)
{
}

CGpsPointArray::CGpsPointArray(const CGpsPointArray& array, size_t begin, size_t count) :
	std::deque<CGpsPoint>(),
	m_sName(array.m_sName)
{
	const_iterator itFirst = array.begin();
	const_iterator itLast = array.end();

	std::advance(itFirst, begin);

	if (count != static_cast<size_t>(-1))
	{
		itLast = itFirst;
		std::advance(itLast, count);
	}

	assign(itFirst, itLast);
}

CGpsPointArray& CGpsPointArray::operator=(const CGpsPointArray& array)
{
	if (&array == this)
		return *this;

	assign(array.begin(), array.end());
	m_sName = array.m_sName;
	return *this;
}

CGpsPointArray& CGpsPointArray::operator=(const geo::CGeoLocations& gLocations)
{
	assign(gLocations.begin(), gLocations.end());
	return *this;
}

CGpsPointArray& CGpsPointArray::operator+=(const CGpsPointArray& array)
{
	if (&array == this)
		return *this;

	append(array.begin(), array.end());
	return *this;
}

CGpsPointArray& CGpsPointArray::operator+=(const geo::CGeoLocations& gLocations)
{
	append(gLocations.begin(), gLocations.end());
	return *this;
}

bool CGpsPointArray::operator==(const CGpsPointArray& array) const
{
	return (m_sName == array.m_sName) && std::operator==(*this, array);
}

void CGpsPointArray::clear()
{
	m_sName.clear();
	std::deque<CGpsPoint>::clear();
}

size_t CGpsPointArray::upper_bound() const
{
	return empty() ? 0 : size() - 1;
}

void CGpsPointArray::insert(size_t pos, const CGpsPoint& cGpsPoint)
{
	if (pos < size())
		at(pos).clearRouteInfo();

	iterator it = begin();
	std::advance(it, pos);

	std::deque<CGpsPoint>::insert(it, cGpsPoint);
}

void CGpsPointArray::erase(size_t pos)
{
	iterator it = begin();
	std::advance(it, pos);

	std::deque<CGpsPoint>::erase(it);

	if (pos < size())
		at(pos).clearRouteInfo();
}

void CGpsPointArray::move(size_t posSrc, size_t posDst)
{
	if (posSrc != posDst)
	{
		CGpsPoint cGpsPoint(at(posSrc));
		erase(posSrc);

		if (posDst > posSrc)
			posDst--;

		insert(posDst, cGpsPoint);

		iterator itSrc = begin();
		iterator itDst = begin();

		if (posSrc > posDst)
		{
			std::advance(itSrc, posDst);
			std::advance(itDst, posSrc);
		}
		else
		{
			std::advance(itSrc, posSrc);
			std::advance(itDst, posDst);
		}

		++itDst;
		for (; itSrc != itDst; ++itSrc)
			itSrc->clearRouteInfo();
	}
}


void CGpsPointArray::reverse()
{
	for (iterator it = begin(); it != end(); ++it)
		it->clearRouteInfo();

	std::reverse(begin(), end());
}

void CGpsPointArray::sortByAddress()
{
	std::stable_sort(begin(), end(), sortPred);
}

void CGpsPointArray::removeDuplicates()
{
	std::stable_sort(begin(), end());
	resize(std::unique(begin(), end(), uniquePred) - begin());
}

void CGpsPointArray::removeEmpties()
{
	resize(std::remove_if(begin(), end(), removePred) - begin());
}
