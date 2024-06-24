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
 * Purpose : Header for Sygic McGuider file (.itf)
 *           www.sygic.com / www.mcguider.com
 */

#ifndef _ITF_H_INCLUDED_
#define _ITF_H_INCLUDED_

#include <stdint.h>
#include <string>

class SygicMcGuider
{
public:
#pragma pack(push, 2) // packing is now 2
	typedef struct
	{
		uint32_t uiSignature; // Always 55 2B C3 03
		uint16_t usPointNumber1;
		uint32_t uiPadding; // Always 00 00 FF FF
		uint16_t usPointNumber2;
	} ITF_HEADER;

	typedef struct
	{
		int32_t nLongitude;
		int32_t nLatitude;
		uint16_t usType;
		uint16_t usNameLength;
	} ITF_POINT;
#pragma pack(pop) // packing is default

	static const uint32_t HDR_SIGNATURE; // 0x03C32B55
	static const uint32_t HDR_PADDING; // 0xFFFF0000
	static const int COORDS_FACTOR; // 100000
};

class SygicGpsNavigation
{
public:
#pragma pack(push, 2) // packing is now 2
	typedef struct
	{
		uint32_t uiSignature; // Always 03 00 40 00
		uint32_t uiPadding; // Always 76 00 37 00
	} ITF_HEADER;

	typedef struct
	{
		int32_t nLongitude;
		int32_t nLatitude;
		int32_t nNextLongitude;
		int32_t nNextLatitude;
	} ITF_POINT_11;

	typedef struct
	{
		int32_t nLongitude;
		int32_t nLatitude;
		int64_t nPadding; // 0x00
		int32_t nNextLongitude;
		int32_t nNextLatitude;
		int64_t nNextPadding; // 0x00
	} ITF_POINT_13;

	typedef struct
	{
		uint16_t usLength;
		wchar_t wString;
	} ITF_STRING;
#pragma pack(pop) // packing is default

	static const std::wstring CAR;
	static const uint32_t FTI;
	static const uint32_t TSR;
	static const uint32_t OVA;

	static const uint32_t HDR_SIGNATURE; // 0x00400003
	static const uint32_t HDR_PADDING; // 0x00370076
	static const int COORDS_FACTOR; // 100000
};

#endif // _ITF_H_INCLUDED_
