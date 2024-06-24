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
 * Purpose : Optimize a route
 */

#include "stdafx.h"
#include "travel.h"
#include "GpsWaypointArray.h"

 // Apply the 2-opt algorithm for the Traveling Salesman Problem
 // For more efficient algorithm, may be see Kernighan-Lin

CTravel::CTravel(CGpsPointArray& cGpsPointArray) :
	m_cGpsPointArray(cGpsPointArray),
	m_ulMatrixSize(m_cGpsPointArray.size()),
	m_vecDistMatrix(m_ulMatrixSize* m_ulMatrixSize, 0)
{
}

CTravel::~CTravel()
{
}

size_t CTravel::GetDistance(size_t src, size_t dst, bool bDirections)
{
	size_t ulDistance = 0;

	if (src != dst)
	{
		if (bDirections)
		{
			geo::CGeoDirections gDirections(geo::CGeoProviders::instance().getDefaultProvider());
			gDirections->Load(m_cGpsPointArray[src], m_cGpsPointArray[dst]);
			if (gDirections->getStatus() == geo::E_GEO_OK)
				ulDistance = gDirections->getRoutes().front().summary().distance();
		}
		else
		{
			ulDistance = m_cGpsPointArray[src].distanceFrom(m_cGpsPointArray[dst]);
		}
	}

	return ulDistance;
}

size_t CTravel::CumulativeDistance(const std::vector<size_t>& vecOptTravel, bool bDirections)
{
	size_t ulDistance = 0;
	size_t ulUpper = vecOptTravel.size() - 1;

	for (size_t i = 0; i < ulUpper; ++i)
	{
		size_t x = vecOptTravel[i];
		size_t y = vecOptTravel[i + 1];

		if (!m_vecDistMatrix[TAB(x, y)])
			m_vecDistMatrix[TAB(x, y)] = m_vecDistMatrix[TAB(y, x)] = GetDistance(x, y, bDirections);

		ulDistance += m_vecDistMatrix[TAB(x, y)];
	}

	return ulDistance;
}

void CTravel::Reverse(std::vector<size_t>& vecTravel, size_t src, size_t dst)
{
	size_t ulLower, ulHigher;

	if (src < dst)
	{
		ulLower = src;
		ulHigher = dst;
	}
	else
	{
		ulLower = dst;
		ulHigher = src;
	}

	while (ulLower < ulHigher)
	{
		std::swap(vecTravel[ulLower], vecTravel[ulHigher]);
		ulLower++;
		ulHigher--;
	}
}

void CTravel::Optimize(std::vector<size_t>& vecOptTravel, bool bDirections)
{
	size_t i, j;
	vecOptTravel.reserve(m_ulMatrixSize);

	for (i = 0; i < m_ulMatrixSize; ++i)
		vecOptTravel.push_back(i);

	if (m_ulMatrixSize < 4)
		return;

	std::vector<size_t> vecNewTravel = vecOptTravel;

	size_t ulOptDistance = CumulativeDistance(vecOptTravel, bDirections);
	size_t ulNewDistance = 0;
	size_t ulUpper = vecOptTravel.size() - 1;
	bool bContinue;

	do
	{
		bContinue = false;

		for (i = 1; i < ulUpper; ++i)
		{
			for (j = i + 1; j < ulUpper; j++)
			{
				// Make a swap (2-change)
				Reverse(vecNewTravel, i, j);
				ulNewDistance = CumulativeDistance(vecNewTravel, bDirections);

				// if new route is shortest than the older
				if (ulNewDistance < ulOptDistance)
				{
					vecOptTravel = vecNewTravel;
					ulOptDistance = ulNewDistance;
					bContinue = true;
				}
				else
				{
					// Restore previous travel
					vecNewTravel = vecOptTravel;
				}
			}
		}
	} while (bContinue);
}

void CTravel::Optimize(CGpsPointArray& cTargetGpsPointArray, bool bDirections)
{
	std::vector<size_t> vecOptTravel;
	std::vector<size_t>::iterator it;

	Optimize(vecOptTravel, bDirections);

	cTargetGpsPointArray.clear();
	for (it = vecOptTravel.begin(); it != vecOptTravel.end(); ++it)
		cTargetGpsPointArray.push_back(m_cGpsPointArray[*it]);

	cTargetGpsPointArray.name(m_cGpsPointArray.name());
}

void CTravel::Optimize(bool bDirections)
{
	CGpsWaypointArray cTargetGpsPointArray;
	Optimize(cTargetGpsPointArray, bDirections);
	m_cGpsPointArray = cTargetGpsPointArray;
}
