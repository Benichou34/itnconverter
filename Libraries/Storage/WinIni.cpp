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

#include "WinIni.h"
#include "StorageImpl.h"
#include <fstream>
#include <map>
#include "ToolsLibrary/Base64.h"
#include "stdx/string_helper.h"

namespace storage
{
	namespace
	{
		class CWinIniValue : public StorageValueImpl
		{
		public:
			CLONEABLE(CWinIniValue);
			~CWinIniValue() override = default;

			void read(type_t eType, std::vector<unsigned char>& value) const override
			{
				if (!exist())
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);

				const std::string& strValue = m_pFolder->find(getName())->second;

				switch (eType)
				{
				case E_TYPE_BINARY:
				{
					std::string decValue = CBase64::Decode(strValue);
					value.assign(decValue.begin(), decValue.end());
				}
				break;

				case E_TYPE_UINT32:
				{
					if (strValue.size() != sizeof(uint32_t))
						throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_bad_length);

					uint32_t unValue = stdx::string_helper::string_to<uint32_t>(strValue);
					value.assign(reinterpret_cast<unsigned char*>(&unValue), reinterpret_cast<unsigned char*>(&unValue) + sizeof(unValue));
				}
				break;

				case E_TYPE_UINT64:
				{
					if (strValue.size() != sizeof(uint64_t))
						throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_bad_length);

					uint64_t unValue = stdx::string_helper::string_to<uint64_t>(strValue);
					value.assign(reinterpret_cast<unsigned char*>(&unValue), reinterpret_cast<unsigned char*>(&unValue) + sizeof(unValue));
				}
				break;

				default:
				{
					std::wstring strwValue = stdx::string_helper::widen(strValue);
					value.assign(reinterpret_cast<const unsigned char*>(strwValue.data()), reinterpret_cast<const unsigned char*>(strwValue.data()) + strwValue.size() * sizeof(wchar_t));
				}
				break;
				}
			}

			void write(type_t eType, const std::vector<unsigned char>& value) override
			{
				if (!exist())
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);

				switch (eType)
				{
				case E_TYPE_BINARY:
					(*m_pFolder)[getName()] = CBase64::Encode(std::string(reinterpret_cast<const char*>(value.data()), reinterpret_cast<const char*>(value.data()) + value.size()));
					break;

				case E_TYPE_UINT32:
					if (value.size() != sizeof(uint32_t))
						throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_bad_length);

					(*m_pFolder)[getName()] = stdx::string_helper::to_string<uint32_t>(*reinterpret_cast<const uint32_t*>(value.data()));
					break;

				case E_TYPE_UINT64:
					if (value.size() != sizeof(uint64_t))
						throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_bad_length);

					(*m_pFolder)[getName()] = stdx::string_helper::to_string<uint64_t>(*reinterpret_cast<const uint64_t*>(value.data()));
					break;

				default:
					(*m_pFolder)[getName()] = stdx::string_helper::narrow(std::wstring(reinterpret_cast<const wchar_t*>(value.data()), reinterpret_cast<const wchar_t*>(value.data()) + value.size() / sizeof(wchar_t)));
					break;
				}
			}

			bool exist() const override
			{
				if (!m_pFolder)
					return false;

				return (m_pFolder->count(getName()) > 0);
			}

			void erase() override
			{
				if (!exist())
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);

				m_pFolder->erase(getName());
			}

		private:
			friend class CWinIniFolder;

			CWinIniValue(const std::wstring& strName, std::map<std::wstring, std::string>* pFolder) : StorageValueImpl(strName),
				m_pFolder(pFolder) {}

			CWinIniValue(const CWinIniValue& value) : StorageValueImpl(value),
				m_pFolder(value.m_pFolder) {}

		private:
			std::map<std::wstring, std::string>* m_pFolder;
		};

		class CWinIniFolder : public StorageFolderImpl
		{
		public:
			CLONEABLE(CWinIniFolder);
			~CWinIniFolder() override = default;

			std::unique_ptr<StorageValueImpl> getValue(const std::wstring& strValue) override
			{
				return std::unique_ptr<StorageValueImpl>(new CWinIniValue(strValue, &(*m_pFolders)[strValue]));
			}

			std::unique_ptr<StorageValueImpl> getConstValue(const std::wstring& strValue) const override
			{
				return std::unique_ptr<StorageValueImpl>(new CWinIniValue(strValue, exist() ? &(*m_pFolders)[strValue] : nullptr));
			}

			std::unique_ptr<StorageFolderImpl> getFolder(const std::wstring&) override
			{
				return nullptr; // Not supported
			}

			std::unique_ptr<StorageFolderImpl> getConstFolder(const std::wstring&) const override
			{
				return nullptr; // Not supported
			}

			bool exist() const override
			{
				if (!m_pFolders)
					return false;

				return (m_pFolders->count(getName()) > 0);
			}

			void erase() override
			{
				if (!exist())
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);

				m_pFolders->erase(getName());
			}

		private:
			friend class CWinIni;

			CWinIniFolder(const std::wstring& strName, std::map< std::wstring, std::map<std::wstring, std::string> >* pFolders) : StorageFolderImpl(strName),
				m_pFolders(pFolders) {}

			CWinIniFolder(const CWinIniFolder& folder) : StorageFolderImpl(folder),
				m_pFolders(folder.m_pFolders) {}

		private:
			mutable std::map< std::wstring, std::map<std::wstring, std::string> >* m_pFolders;
		};

		class CWinIni : public StorageImpl
		{
		public:
			CLONEABLE(CWinIni);
			CWinIni(const std::wstring& strFileName)
			{
				if (!strFileName.empty())
					open(strFileName);
			}

			CWinIni(const CWinIni& winini) :
				m_strFileName(winini.m_strFileName),
				m_mapFolders(winini.m_mapFolders) {}

			~CWinIni() override = default;

			CWinIni& operator=(const CWinIni& winini)
			{
				m_strFileName = winini.m_strFileName;
				m_mapFolders = winini.m_mapFolders;

				return *this;
			}

			void open(const std::wstring& strFileName)
			{
				m_strFileName = strFileName;

				std::ifstream ifsFile(strFileName.c_str(), std::ios_base::in | std::ios_base::binary);
				if (!ifsFile)
					throw Exception(stdx::wstring_helper::to_utf8(m_strFileName), Exception::err_data_not_found);

				std::map<std::wstring, std::string>* pFolder = nullptr;

				std::string strLine;
				while (std::getline(ifsFile, strLine))
				{
					size_t nPos = strLine.find(';');
					if (nPos != std::string::npos)
						strLine.erase(nPos);

					nPos = strLine.find('['); // Search section
					if (nPos != std::string::npos)
					{
						size_t nEnd = strLine.find(']', nPos);
						if (nEnd != std::string::npos)
						{
							std::string strFolder = strLine.substr(nPos, nEnd - nPos);
							pFolder = &m_mapFolders[stdx::string_helper::widen(strFolder)];
						}
					}
					else if (pFolder) // Search SValue
					{
						stdx::string_helper::vector vecStrResult;
						stdx::string_helper::split(strLine, vecStrResult, stdx::find_first("="));

						if (vecStrResult.size() == 2)
						{
							stdx::string_helper::trimleft(vecStrResult[0], " \t");
							stdx::string_helper::trimright(vecStrResult[0], " \t\r");

							stdx::string_helper::trimleft(vecStrResult[1], " \t");
							stdx::string_helper::trimright(vecStrResult[1], " \t\r");

							(*pFolder)[stdx::string_helper::widen(vecStrResult[0])] = vecStrResult[1];
						}
					}
				}
			}

			void flush() override
			{
				if (!m_strFileName.empty())
				{
					std::ofstream ofsFile(m_strFileName.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
					if (!ofsFile)
						throw Exception(stdx::wstring_helper::to_utf8(m_strFileName), Exception::err_data_not_found);

					std::map< std::wstring, std::map<std::wstring, std::string> >::iterator itFolder;
					for (itFolder = m_mapFolders.begin(); itFolder != m_mapFolders.end(); ++itFolder)
					{
						ofsFile << '[' << stdx::string_helper::narrow(itFolder->first) << ']' << std::endl;

						std::map<std::wstring, std::string>::iterator itValue;
						for (itValue = itFolder->second.begin(); itValue != itFolder->second.end(); ++itValue)
							ofsFile << stdx::string_helper::narrow(itValue->first) << '=' << itValue->second << std::endl;

						ofsFile << std::endl;
					}
				}
			}

			void close() override
			{
				m_strFileName.clear();
				m_mapFolders.clear();
			}

			std::unique_ptr<StorageFolderImpl> getFolder(const std::wstring& strFolder) override
			{
				return std::unique_ptr<StorageFolderImpl>(new CWinIniFolder(strFolder, &m_mapFolders));
			}

			std::unique_ptr<StorageFolderImpl> getConstFolder(const std::wstring& strFolder) const override
			{
				if (m_mapFolders.count(strFolder))
					return std::unique_ptr<StorageFolderImpl>(new CWinIniFolder(strFolder, &m_mapFolders));

				return std::unique_ptr<StorageFolderImpl>(new CWinIniFolder(strFolder, nullptr));
			}

		private:
			std::wstring m_strFileName;
			mutable std::map< std::wstring, std::map<std::wstring, std::string> > m_mapFolders;
		};
	}

	WinIni::WinIni(const std::wstring& strFileName) : Storage(std::make_unique<CWinIni>(strFileName))
	{
	}

	void WinIni::open(const std::wstring& strFileName)
	{
		reinterpret_cast<CWinIni&>(getImplementation()).open(strFileName);
	}
}
