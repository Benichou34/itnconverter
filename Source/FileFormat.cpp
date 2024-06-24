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
#include "FileFormat.h"
#include "ITN Tools.h"

namespace
{
	const std::vector<FileFormatDesc> cs_FileFormats =
	{
		{ TYPE_AXE, IDS_AXEFILTERS, ReadAXE, WriteAXE, 0 },
		{ TYPE_AXG, IDS_AXGFILTERS, ReadAXE, nullptr, 0 },
		{ TYPE_EST, IDS_ESTFILTERS, ReadAXE, WriteEST, 0 },
		{ TYPE_PTM, IDS_PTMFILTERS, ReadAXE, WritePTM, 0 },
		{ TYPE_MPS, IDS_MPSFILTERS, ReadMPS, WriteMPS, 0 },
		{ TYPE_GDB, IDS_GDBFILTERS, ReadGDB, WriteGDB, 0 },
		{ TYPE_RTE, IDS_RTEFILTERS, ReadRTE, WriteRTE, 0 },
		{ TYPE_WPT, 0, ReadRTE, nullptr, 0 },
		{ TYPE_GPX, IDS_GPGFILTERS, nullptr, WriteGPG, 0 },
		{ TYPE_GPX, IDS_GBCFILTERS, nullptr, WriteGBC, 0 },
		{ TYPE_GPX, IDS_GPXFILTERS, ReadGPX, WriteGPX, IDS_REMOVE_COMMA },
		{ TYPE_GPX, IDS_GPXDAIMLER50FILTERS, ReadDaimlerGPX, WriteDaimler50GPX, 0 },
		{ TYPE_GPX, IDS_GPXDAIMLER55FILTERS, nullptr, WriteDaimler55GPX, 0},
		{ TYPE_GPX, IDS_GPXDAIMLERUBXFILTERS, nullptr, WriteDaimlerUbxGPX, 0 },
		{ TYPE_ITN, IDS_ITNFILTERS, ReadITN, WriteITN, 0 },
		{ TYPE_OV2, IDS_OV2FILTERS, ReadOV2, WriteOV2, 0 },
		{ TYPE_RTE, IDS_OZIFILTERS, ReadOZI, WriteOZI, 0 },
		{ TYPE_RT2, IDS_RT2FILTERS, ReadRT2, WriteRT2, 0 },
		{ TYPE_PLT, IDS_PLTFILTERS, ReadPLT, WritePLT, 0 },
		{ TYPE_WPT, IDS_WPTFILTERS, ReadWPT, WriteWPT, 0 },
		{ TYPE_RTE, IDS_MAGFILTERS, ReadMAG, nullptr, 0 },
		{ TYPE_UPT, 0, ReadMAG, nullptr, 0 },
		{ TYPE_MAG, IDS_MAGFILTERS, nullptr, WriteMAG, IDS_EXPLORIST },
		{ TYPE_RTE, IDS_MN4FILTERS, ReadMNX, WriteMN4, 0 },
		{ TYPE_RTE, IDS_MN6FILTERS, ReadMNX, WriteMN6, 0 },
		{ TYPE_RTE, IDS_BCKFILTERS, ReadMNX, WriteMN7, 0 },
		{ TYPE_ROUTE, IDS_ROUTEFILTERS, ReadROUTE, WriteROUTE, 0 },
		{ TYPE_STORE, 0, ReadROUTE, nullptr, 0 },
		{ TYPE_NVG, IDS_NVGFILTERS, ReadNVG, WriteNVG, 0 },
		{ TYPE_DAT, IDS_DATFILTERS, ReadDAT, WriteDAT, 0 },
		{ TYPE_XML, IDS_XMLFILTERS, ReadXML, WriteXML, 0 },
		{ TYPE_XML, IDS_NVMFILTERS, ReadNVM, WriteNVM, 0 },
		{ TYPE_CSV, IDS_CSVFILTERS, ReadCSV, WriteCSV, 0 },
		{ TYPE_KML, IDS_KMLFILTERS, ReadKML, WriteKML, 0 },
		{ TYPE_KML, IDS_NDRIVEFILTERS, nullptr, WriteNDRIVE, 0 },
		{ TYPE_BCR, IDS_BCRFILTERS, ReadBCR, WriteBCR, 0 },
		{ TYPE_XVM, IDS_XVMFILTERS, ReadXVM, WriteXVM, 0 },
		{ TYPE_RDN, IDS_RDNFILTERS, ReadRDN, WriteRDN, 0 },
		{ TYPE_TRP, IDS_TRPFILTERS, ReadTRP, WriteTRP, 0 },
		{ TYPE_TRP, IDS_CP8FILTERS, nullptr   , WriteCP8, 0 },
		{ TYPE_TRP, IDS_CP10FILTERS, ReadCP10, WriteCP10, 0 },
		{ TYPE_ITF, IDS_ITFFILTERS, ReadITF, WriteOldITF, 0 },
		{ TYPE_ITF, IDS_ITF11FILTERS, nullptr, WriteITF11, 0 },
		{ TYPE_ITF, IDS_ITF13FILTERS, nullptr, WriteITF13, 0 },
		{ TYPE_HTM, IDS_URLSYGICFILTERS, nullptr, WriteSygicUrl, 0 },
		{ TYPE_XML, IDS_GPLFILTERS, ReadGPL, WriteGPL, 0 },
		{ TYPE_LMX, IDS_LMXFILTERS, ReadLMX, WriteLMX, 0 },
		{ TYPE_FLK, IDS_FLKFILTERS, ReadFLK, WriteFLK, 0 },
		{ TYPE_KRT, IDS_KRTFILTERS, ReadKRT, WriteKRT, 0 },
		{ TYPE_TRL, IDS_TRLFILTERS, ReadTRL, WriteTRL, 0 },
		{ TYPE_TXT, IDS_TXTFILTERS, ReadGCL, WriteGCL, 0 },
		{ TYPE_OSM, IDS_OSMFILTERS, ReadOSM, WriteOSM, 0 },
		{ TYPE_RT, IDS_RTFILTERS, ReadGCL, WriteRT, 0 },
		{ TYPE_LOC, IDS_LOCFILTERS, nullptr, WriteLOC, 0 },
		{ TYPE_TK, IDS_TKFILTERS, ReadTK, WriteTK, 0 },
		{ TYPE_UPOI, IDS_UPOIFILTERS, ReadUPOI, nullptr, 0 },
		{ TYPE_UPOI, IDS_UPOI80FILTERS, nullptr, WriteUPOI80, 0 },
		{ TYPE_UPOI, IDS_UPOI83FILTERS, nullptr, WriteUPOI83, 0 },
		{ TYPE_XML, IDS_MPFCTRFILTERS, ReadMPFCTR, WriteMPFCTR, 0 },
		{ TYPE_GGM, IDS_GGMFILTERS, nullptr, WriteGGM, IDS_PRINT_VERSION },
		{ TYPE_HERE, IDS_HEREFILTERS, nullptr, WriteHERE, 0 },
		{ TYPE_VMM, IDS_VMMFILTERS, nullptr, WriteViaMichelin, IDS_PRINT_VERSION },
		{ TYPE_TTD, IDS_TTDFILTERS, nullptr, WriteTomtomDrive, 0 },
		{ TYPE_URL, IDS_URLFILTERS, ReadURL, nullptr, 0 }
	};

	const std::vector<std::wstring> cs_EmptyTargetFolders;
}

const std::vector<FileFormatDesc>& getFileFormats()
{
	return cs_FileFormats;
}

const std::vector<std::wstring>& getTargetFolders(const std::wstring&)
{
	return cs_EmptyTargetFolders;
}

bool alphabeticalSortFileFormats()
{
	return true;
}
