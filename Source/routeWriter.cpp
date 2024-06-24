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
 * Purpose : Writer for Navigon MN|7 file (.route)
 *           www.navigon.com
 */

#include "stdafx.h"
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "ITN Tools.h"
#include "routeHeader.h"
#include "nvgfresh.h"

#define FIXED_SIZE_0X00		sizeof(_DATA) // + name size
#define FIXED_SIZE_FIELD	sizeof(_PT_FIELD)
#define FIXED_SIZE_0X28		48 // + name size + code size
#define FIXED_SIZE_0X0A		15
#define FIXED_SIZE_END		16

static void CoordToNavigon(double dCoord, _GPS_COORD& _Coord)
{
	double dValue = COORD_REF_VAL;
	dCoord *= COORD_FACTOR;

	_Coord.unMSB = COORD_REF_MSB;
	_Coord.unLSB = COORD_REF_LSB;

	if (dCoord < 0)
	{
		_Coord.unMSB |= COORD_NEGATIVE;
		dCoord = -dCoord;
	}

	// Premiere partie, on cherche la partie entiere
	while (dValue > dCoord)
	{
		dValue /= 2;
		if (_Coord.unLSB)
		{
			_Coord.unLSB -= 0x10000000;
		}
		else
		{
			_Coord.unMSB--;
			_Coord.unLSB = COORD_MAX_LSB;
		}
	}

	// On en deduit les facteurs de multiplication
	double dFactor = dValue / 16;
	for (int i = 0; i < 32; i += 8)
	{
		_Coord.unLSB |= static_cast<uint32_t>((dCoord - dValue) / dFactor) << (24 - i);
		dFactor /= 256;
	}
}

static std::string GetFied_28(const CGpsPoint& cGpsPoint, uint32_t unIdentifier)
{
	std::string strPadding(7, 0x00);
	std::ostringstream oss;
	oss.fill(0x00);

	// Champs 0x28, Nom + GPS + CP
	_DATA _Data28;

	_Data28.uiId = UID_NAME_GPS_CODE;
	_Data28.uiType = TYPE_DECAL2;
	_Data28.uiLength = FIXED_SIZE_0X28 + cGpsPoint.name().size();

	oss.write(reinterpret_cast<const char*>(&_Data28), sizeof(_Data28));

	_STRING _String;
	_String.uiType = STRING_NAME;
	_String.uiLength = cGpsPoint.name().size();
	oss.write(reinterpret_cast<const char*>(&_String), sizeof(_String));
	oss << cGpsPoint.name();

	// Coordonnees GPS
	_GPS_COORDS _Coords;

	if (_Data28.uiType == TYPE_DECAL || _Data28.uiType == TYPE_DECAL2)
		oss.write(strPadding.data(), 7);
	else
		oss.write(strPadding.data(), 3);

	CoordToNavigon(cGpsPoint.lng(), _Coords.Longitude);
	CoordToNavigon(cGpsPoint.lat(), _Coords.Latitude);
	_Coords.ucRFU = 0x00;
	oss.write(reinterpret_cast<const char*>(&_Coords), sizeof(_Coords));

	// Code postal (vide)
	_String.uiType = STRING_CITY;
	_String.uiLength = 0;
	oss.write(reinterpret_cast<const char*>(&_String), sizeof(_String));
	oss.write(strPadding.data(), 4);

	// Identifiant du champs
	oss.write(reinterpret_cast<const char*>(&unIdentifier), sizeof(uint32_t));

	return oss.str();
}

static std::string GetFied_0A(const CGpsPoint& cGpsPoint, uint32_t unIdentifier)
{
	std::ostringstream oss;
	oss.fill(0x00);

	// Champs 0xOA, Country
	_DATA _Data0A;

	_Data0A.uiId = UID_COUNTRY;
	_Data0A.uiType = TYPE_DECAL2;
	_Data0A.uiLength = FIXED_SIZE_0X0A;

	oss.write(reinterpret_cast<const char*>(&_Data0A), sizeof(_Data0A));

	_STRING _String;
	_String.uiType = STRING_NAME;
	_String.uiLength = 3;
	oss.write(reinterpret_cast<const char*>(&_String), sizeof(_String));
	oss << CNavigonFresh::GetCountry(cGpsPoint);

	// Identifiant du champs
	oss.write(reinterpret_cast<const char*>(&unIdentifier), sizeof(uint32_t));

	return oss.str();
}

static std::string GetPoint(const CGpsPoint& cGpsPoint)
{
	std::vector<std::string> vecField;
	std::vector<uint32_t> vecIdentifier;
	uint32_t unTotalLength = FIXED_SIZE_0X00 + FIXED_SIZE_FIELD + FIXED_SIZE_END;

	vecIdentifier.push_back(static_cast<uint32_t>(time(nullptr)));
	vecField.push_back(GetFied_28(cGpsPoint, vecIdentifier.back()));
	unTotalLength += static_cast<uint32_t>(vecField.back().size());

	vecIdentifier.push_back(vecIdentifier.back() / 2);
	vecField.push_back(GetFied_0A(cGpsPoint, vecIdentifier.back()));
	unTotalLength += static_cast<uint32_t>(vecField.back().size());

	std::string strPadding(FIXED_SIZE_END, 0x00);
	std::ostringstream oss;

	oss.fill(0x00);

	// Ecriture de la taille totale
	unTotalLength += (vecIdentifier.size() - 1) * sizeof(uint32_t) * 2;
	oss.write(reinterpret_cast<const char*>(&unTotalLength), sizeof(uint32_t));

	// Ecriture du nom (vide)
	_DATA _Name;

	_Name.uiId = UID_NAME;
	_Name.uiType = TYPE_NAME;
	_Name.uiLength = 0;
	oss.write(reinterpret_cast<const char*>(&_Name), sizeof(_Name));

	// Ecriture du nombre de champs
	_PT_FIELD _ptFields;

	_ptFields.uiType = TYPE_FIELD;
	_ptFields.uiFieldsNb = vecField.size();
	_ptFields.uiLastField = vecIdentifier.back();
	oss.write(reinterpret_cast<const char*>(&_ptFields), sizeof(_ptFields));

	// Liste des champs
	std::vector<std::string>::iterator itVecStr;
	for (itVecStr = vecField.begin(); itVecStr != vecField.end(); ++itVecStr)
		oss << *itVecStr;

	// Liste des identifiant
	vecIdentifier.pop_back(); // remove last identifier

	std::vector<uint32_t>::iterator itVecId;
	for (itVecId = vecIdentifier.begin(); itVecId != vecIdentifier.end(); ++itVecId)
	{
		oss.write(reinterpret_cast<const char*>(&(*itVecId)), sizeof(uint32_t));
		oss.write(strPadding.data(), 4);
	}

	// Padding de fin
	oss.write(strPadding.data(), FIXED_SIZE_END);

	return oss.str();
}

int WriteROUTE(const std::wstring& strPathName, const CGpsRoute& cGpsRoute, DWORD, bool)
{
	std::vector<std::string> vecPoint;
	CGpsRoute::const_iterator it;
	_HEADER _header = { 0, 0 };

	vecPoint.reserve(cGpsRoute.size());

	/* Format date Time */
	time_t rawtime = time(nullptr);
	struct tm* ptmTime = localtime(&rawtime);

	_STRING sDateTime = { TYPE_NAME, 0 };
	std::string strDateTime = stdx::format("R%04d%02d%02d-%02d:%02d:%02d")
		(ptmTime->tm_year + 1900)(ptmTime->tm_mon + 1)(ptmTime->tm_mday)
		(ptmTime->tm_hour)(ptmTime->tm_min)(ptmTime->tm_sec);
	sDateTime.uiLength = strDateTime.size();

	/* Total file length */
	uint32_t unTotalLength = sizeof(uint32_t) + sizeof(_header) + sizeof(_STRING) + sDateTime.uiLength;

	for (it = cGpsRoute.begin(); it != cGpsRoute.end(); ++it)
	{
		vecPoint.push_back(GetPoint(*it));
		unTotalLength += static_cast<uint32_t>(vecPoint.back().size());
	}

	_header.unPointNumber = static_cast<uint32_t>(vecPoint.size());

	std::ofstream ofsFile(strPathName.c_str(), std::ios_base::binary);
	if (!ofsFile)
		return S_FALSE;

	ofsFile.write(reinterpret_cast<const char*>(&gs_version), sizeof(gs_version));
	ofsFile.write(reinterpret_cast<const char*>(&unTotalLength), sizeof(uint32_t));
	ofsFile.write(reinterpret_cast<const char*>(&sDateTime), sizeof(_STRING));
	ofsFile << strDateTime;
	ofsFile.write(reinterpret_cast<const char*>(&_header), sizeof(_header));

	std::vector<std::string>::iterator itVecStr;
	for (itVecStr = vecPoint.begin(); itVecStr != vecPoint.end(); ++itVecStr)
		ofsFile << *itVecStr;

	ofsFile.close();
	return ofsFile.good() ? S_OK : S_FALSE;
}

