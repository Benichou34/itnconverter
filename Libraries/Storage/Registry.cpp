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

#ifdef _WIN32
#include "Registry.h"
#include "StorageImpl.h"
#include <map>
#include "stdx/string_helper.h"

namespace storage
{
	namespace
	{
		[[noreturn]] void ThrowError(long lResult, const std::wstring& what)
		{
			Exception::error_t eError = Exception::err_unknown;

			switch (lResult)
			{
			case ERROR_FILE_NOT_FOUND:
				eError = Exception::err_data_not_found;
				break;

			case ERROR_INVALID_DATA:
				eError = Exception::err_invalid_data;
				break;

			case ERROR_BAD_LENGTH:
				eError = Exception::err_bad_length;
				break;

			default:
				break;
			}

			throw Exception(stdx::wstring_helper::to_utf8(what), eError);
		}

		DWORD getWinRegistryFromType(StorageValueImpl::type_t eType)
		{
			switch (eType)
			{
			case StorageValueImpl::E_TYPE_BINARY:
				return REG_BINARY;

			case StorageValueImpl::E_TYPE_UINT32:
				return REG_DWORD;

			case StorageValueImpl::E_TYPE_UINT64:
#ifdef REG_QWORD
				return REG_QWORD;
#else
				return REG_BINARY;
#endif

			case StorageValueImpl::E_TYPE_SZ:
				return REG_SZ;

			default:
				return REG_NONE;
			}
		}

		class CRegistryValue : public StorageValueImpl
		{
		public:
			CLONEABLE(CRegistryValue);
			~CRegistryValue() override = default;

			void read(type_t eType, std::vector<unsigned char>& value) const override
			{
				if (!m_hParent)
					ThrowError(ERROR_FILE_NOT_FOUND, getName());

				DWORD dwDataLen;
				DWORD dwType;

				long lResult = RegQueryValueExW(
					m_hParent,
					getName().c_str(),
					nullptr,
					&dwType,
					nullptr,
					&dwDataLen);
				if (lResult != ERROR_SUCCESS)
					ThrowError(lResult, getName());

				if (dwType != getWinRegistryFromType(eType))
					ThrowError(ERROR_INVALID_DATA, getName());

				if (!dwDataLen)
				{
					value.clear();
					return;
				}

				value.resize(dwDataLen);

				lResult = RegQueryValueExW(
					m_hParent,
					getName().c_str(),
					nullptr,
					&dwType,
					reinterpret_cast<LPBYTE>(value.data()),
					&dwDataLen);
				if (lResult != ERROR_SUCCESS)
					ThrowError(lResult, getName());

				if (eType == E_TYPE_SZ && value.size() > 1)
				{
					size_t len = value.size() - 2;
					if (*reinterpret_cast<wchar_t*>(value.data() + len) == std::wstring::traits_type::to_char_type(0x0000))
						value.resize(len);
				}
			}

			void write(type_t eType, const std::vector<unsigned char>& value) override
			{
				if (!m_hParent)
					ThrowError(ERROR_FILE_NOT_FOUND, getName());

				long lResult;

				if (eType == E_TYPE_SZ && value.size() > 1)
				{
					std::wstring valueSZ(reinterpret_cast<const wchar_t*>(value.data()), value.size() / 2);
					if (valueSZ.back() != 0x000)
						valueSZ += std::wstring::traits_type::to_char_type(0x0000);

					lResult = RegSetValueExW(
						m_hParent,
						getName().c_str(),
						0,
						getWinRegistryFromType(eType),
						reinterpret_cast<CONST BYTE*>(valueSZ.data()),
						static_cast<DWORD>(valueSZ.size() * 2));
				}
				else
				{
					lResult = RegSetValueExW(
						m_hParent,
						getName().c_str(),
						0,
						getWinRegistryFromType(eType),
						reinterpret_cast<CONST BYTE*>(value.data()),
						static_cast<DWORD>(value.size()));
				}

				if (lResult != ERROR_SUCCESS)
					ThrowError(lResult, getName());
			}

			bool exist() const override
			{
				if (!m_hParent)
					return false;

				return (RegQueryValueExW(m_hParent, getName().c_str(), nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS);
			}

			void erase() override
			{
				if (!m_hParent)
					ThrowError(ERROR_FILE_NOT_FOUND, getName());

				long lResult = RegDeleteValueW(m_hParent, getName().c_str());
				if (lResult != ERROR_SUCCESS)
					ThrowError(lResult, getName());
			}

		private:
			friend class CRegistryFolder;

			CRegistryValue(const std::wstring& strName, HKEY hParent) : StorageValueImpl(strName),
				m_hParent(hParent) {}

			CRegistryValue(const CRegistryValue& value) : StorageValueImpl(value),
				m_hParent(value.m_hParent) {}

		private:
			HKEY m_hParent;
		};

		class CRegistryFolder : public StorageFolderImpl
		{
		public:
			CLONEABLE(CRegistryFolder);
			~CRegistryFolder() override = default;

			std::unique_ptr<StorageValueImpl> getValue(const std::wstring& strValue) override
			{
				return std::unique_ptr<StorageValueImpl>(new CRegistryValue(strValue, m_hKey));
			}

			std::unique_ptr<StorageValueImpl> getConstValue(const std::wstring& strValue) const override
			{
				return std::unique_ptr<StorageValueImpl>(new CRegistryValue(strValue, m_hKey));
			}

			std::unique_ptr<StorageFolderImpl> getFolder(const std::wstring& strFolder) override
			{
				// Create this key
				HKEY hKey;

				long lResult = RegCreateKeyExW(
					m_hKey,
					strFolder.c_str(),
					0,
					nullptr,
					REG_OPTION_NON_VOLATILE,
					m_RegSam,
					nullptr,
					&hKey,
					nullptr);
				if (lResult != ERROR_SUCCESS)
					ThrowError(lResult, getName());

				return std::unique_ptr<StorageFolderImpl>(new CRegistryFolder(strFolder, m_hKey, hKey, m_RegSam));
			}

			std::unique_ptr<StorageFolderImpl> getConstFolder(const std::wstring& strFolder) const override
			{
				// Try to open the key	
				HKEY hKey;

				long lResult = RegOpenKeyExW(m_hKey, strFolder.c_str(), 0, m_RegSam, &hKey);
				if (lResult == ERROR_SUCCESS)
					return std::unique_ptr<StorageFolderImpl>(new CRegistryFolder(strFolder, m_hKey, hKey, m_RegSam));

				return std::unique_ptr<StorageFolderImpl>(new CRegistryFolder(strFolder, nullptr, nullptr, 0));
			}

			bool exist() const override
			{
				return (m_hParent && m_hKey);
			}

			void erase() override
			{
				if (!m_hParent)
					ThrowError(ERROR_FILE_NOT_FOUND, getName());

				long lResult = RegDeleteKeyW(m_hParent, getName().c_str());
				if (lResult != ERROR_SUCCESS)
					ThrowError(lResult, getName());

				m_hParent = nullptr;
				m_hKey = nullptr;
			}

		private:
			friend class CRegistry;

			CRegistryFolder(const std::wstring& strName, HKEY hParent, HKEY hKey, REGSAM sam) : StorageFolderImpl(strName),
				m_hParent(hParent),
				m_hKey(hKey),
				m_RegSam(sam)
			{
			}

			CRegistryFolder(const CRegistryFolder& folder) : StorageFolderImpl(folder),
				m_hParent(folder.m_hParent),
				m_hKey(folder.m_hKey),
				m_RegSam(folder.m_RegSam)
			{
			}

		private:
			HKEY m_hParent;
			HKEY m_hKey;
			REGSAM m_RegSam;
		};

		class CRegistry : public StorageImpl
		{
		public:
			CLONEABLE(CRegistry);
			CRegistry(HKEY hRoot, REGSAM sam) :
				m_hRoot(hRoot),
				m_RegSam(sam)
			{
			}

			CRegistry(const CRegistry& registry) :
				m_hRoot(registry.m_hRoot),
				m_RegSam(registry.m_RegSam)
			{
			}

			~CRegistry()  override = default;

			CRegistry& operator=(const CRegistry& registry)
			{
				open(registry.m_hRoot, registry.m_RegSam);
				return *this;
			}

			void open(HKEY hRoot, REGSAM sam)
			{
				close();

				m_hRoot = hRoot;
				m_RegSam = sam;
			}

			void flush() override
			{
				std::map<std::wstring, HKEY>::iterator it;
				for (it = m_mapOpenedKeys.begin(); it != m_mapOpenedKeys.end(); ++it)
				{
					long lResult = RegFlushKey(it->second);
					if (lResult != ERROR_SUCCESS)
						ThrowError(lResult, L"Key");
				}
			}

			void close() override
			{
				std::map<std::wstring, HKEY>::iterator it;
				for (it = m_mapOpenedKeys.begin(); it != m_mapOpenedKeys.end(); ++it)
					RegCloseKey(it->second);

				m_mapOpenedKeys.clear();

				if (m_hRoot)
					RegCloseKey(m_hRoot);

				m_hRoot = nullptr;
				m_RegSam = 0;
			}

			std::unique_ptr<StorageFolderImpl> getFolder(const std::wstring& strFolder) override
			{
				// Search for an already opened folder
				std::map<std::wstring, HKEY>::iterator it = m_mapOpenedKeys.find(strFolder);
				if (it == m_mapOpenedKeys.end())
				{
					// Create this key
					HKEY hKey;

					long lResult = RegCreateKeyExW(
						m_hRoot,
						strFolder.c_str(),
						0,
						nullptr,
						REG_OPTION_NON_VOLATILE,
						m_RegSam,
						nullptr,
						&hKey,
						nullptr);
					if (lResult != ERROR_SUCCESS)
						ThrowError(lResult, L"Key");

					it = m_mapOpenedKeys.insert(std::pair<std::wstring, HKEY>(strFolder, hKey)).first;
				}

				return std::unique_ptr<StorageFolderImpl>(new CRegistryFolder(strFolder, m_hRoot, it->second, m_RegSam));
			}

			std::unique_ptr<StorageFolderImpl> getConstFolder(const std::wstring& strFolder) const override
			{
				// Search for an alrealy opened folder
				std::map<std::wstring, HKEY>::iterator it = m_mapOpenedKeys.find(strFolder);
				if (it != m_mapOpenedKeys.end())
					return std::unique_ptr<StorageFolderImpl>(new CRegistryFolder(strFolder, m_hRoot, it->second, m_RegSam));

				// Try to open the key	
				HKEY hKey;
				long lResult = RegOpenKeyExW(m_hRoot, strFolder.c_str(), 0, m_RegSam, &hKey);
				if (lResult == ERROR_SUCCESS)
				{
					m_mapOpenedKeys.insert(std::pair<std::wstring, HKEY>(strFolder, hKey));
					return std::unique_ptr<StorageFolderImpl>(new CRegistryFolder(strFolder, m_hRoot, hKey, m_RegSam));
				}

				return std::unique_ptr<StorageFolderImpl>(new CRegistryFolder(strFolder, nullptr, nullptr, 0));
			}

		private:
			HKEY m_hRoot;
			REGSAM m_RegSam;
			mutable std::map<std::wstring, HKEY> m_mapOpenedKeys;
		};
	}

	Registry::Registry(HKEY hRoot, REGSAM sam) : Storage(std::make_unique<CRegistry>(hRoot, sam))
	{
	}

	void Registry::open(HKEY hRoot, REGSAM sam)
	{
		reinterpret_cast<CRegistry&>(getImplementation()).open(hRoot, sam);
	}
}
#endif // _WIN32
