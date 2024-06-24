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

#if !defined(_GPSPOINTARRAY_H_INCLUDED_)
#define _GPSPOINTARRAY_H_INCLUDED_

#include <deque>
#include "GpsPoint.h"

class CGpsPointArray : public std::deque<CGpsPoint>
{
public:
	typedef enum
	{
		E_ARRAY_ROUTE,
		E_ARRAY_TRACK,
		E_ARRAY_WPT,
		E_ARRAY_POI,
		E_ARRAY_NB
	} E_ARRAY_TYPE;

	CGpsPointArray();
	CGpsPointArray(const CGpsPointArray& array);
	CGpsPointArray(const CGpsPointArray& array, size_t begin, size_t count = 0);
	template <class InputIterator> CGpsPointArray(InputIterator first, InputIterator last) : std::deque<CGpsPoint>(first, last) {}
	virtual ~CGpsPointArray() {}

	virtual CGpsPointArray& operator= (const CGpsPointArray& array);
	virtual CGpsPointArray& operator= (const geo::CGeoLocations& gLocations);

	virtual CGpsPointArray& operator+=(const CGpsPointArray& array);
	virtual CGpsPointArray& operator+=(const geo::CGeoLocations& gLocations);

	bool operator==(const CGpsPointArray& array) const;

	virtual E_ARRAY_TYPE getType() const = 0;

	virtual const std::string& name() const throw() { return m_sName; }
	virtual void name(const std::string& sName) throw() { m_sName = sName; }

	template <class InputIterator> void append(InputIterator first, InputIterator last)
	{
		for (InputIterator it = first; it != last; ++it)
			push_back(*it);
	}

	virtual void clear();
	virtual size_t upper_bound() const;
	virtual void insert(size_t pos, const CGpsPoint& cGpsPoint);
	virtual void erase(size_t pos);
	virtual void move(size_t posSrc, size_t posDst);
	virtual void reverse();

	virtual void sortByAddress();
	virtual void removeDuplicates();
	virtual void removeEmpties();

private:
	std::string m_sName;
};

#endif // !defined(_GPSPOINTARRAY_H_INCLUDED_)
