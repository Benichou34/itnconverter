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

#ifndef _GEO_FACTORY_H_INCLUDED_
#define _GEO_FACTORY_H_INCLUDED_

#include <stdexcept>
#include <memory>
#include "GeoApi.h"

namespace geo
{
	template <class F, class T = F::interface_type>
	class TGeoFactory : public std::unique_ptr<T>
	{
	public:
		TGeoFactory() = default;
		TGeoFactory(TGeoFactory&& factory) : std::unique_ptr<T>(std::move(factory)) {}
		TGeoFactory(E_GEO_PROVIDER eGeoProvider) : std::unique_ptr<T>(F::Get(eGeoProvider)) {}
		TGeoFactory(E_GEO_PROVIDER eGeoProvider, const std::nothrow_t& t) noexcept : std::unique_ptr<T>(F::Get(eGeoProvider, t)) {}
		~TGeoFactory() = default;

		TGeoFactory& assign(E_GEO_PROVIDER eGeoProvider)
		{
			std::unique_ptr<T>::operator=(std::move(F::Get(eGeoProvider)));
			return *this;
		}

		TGeoFactory& assign(E_GEO_PROVIDER eGeoProvider, const std::nothrow_t& t) noexcept
		{
			std::unique_ptr<T>::operator=(std::move(F::Get(eGeoProvider, t)));
			return *this;
		}

		TGeoFactory& operator=(TGeoFactory&& factory)
		{
			std::unique_ptr<T>::operator=(std::move(factory));
			return *this;
		}

		TGeoFactory& operator=(E_GEO_PROVIDER eGeoProvider) { return assign(eGeoProvider); }

		operator bool() const noexcept { return std::unique_ptr<T>::operator bool(); }

		TGeoFactory(const TGeoFactory&) = delete;
		TGeoFactory& operator=(const TGeoFactory&) = delete;
	};
}

#endif // _GEO_FACTORY_H_INCLUDED_
