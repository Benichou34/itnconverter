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

#include "Profile.h"
#include "StorageImpl.h"
#include <fstream>
#include "stdx/string_helper.h"
#include "ToolsLibrary/Base64.h"
#include "JsonParser/JsonParser.h"

namespace storage
{
	namespace
	{
		class CProfileValue : public StorageValueImpl
		{
		public:
			CLONEABLE(CProfileValue);
			~CProfileValue() override = default;

			void read(type_t eType, std::vector<unsigned char>& value) const override
			{
				try
				{
					if (!m_pjsObject || static_cast<type_t>(static_cast<int>(m_pjsObject->at("type"))) != eType)
						throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_invalid_data);

					std::string strValue = CBase64::Decode(m_pjsObject->at("value"));
					if (eType == E_TYPE_SZ)
					{
						std::wstring strwValue = stdx::wstring_helper::from_utf8(strValue);
						value.assign(reinterpret_cast<const unsigned char*>(strwValue.data()), reinterpret_cast<const unsigned char*>(strwValue.data()) + strwValue.size() * sizeof(wchar_t));
					}
					else
					{
						value.assign(strValue.begin(), strValue.end());
					}
				}
				catch (CJsonException&)
				{
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);
				}
			}

			void write(type_t eType, const std::vector<unsigned char>& value) override
			{
				std::string strValue;
				size_t size = 0;

				if (eType == E_TYPE_SZ)
				{
					std::wstring strwValue(reinterpret_cast<const wchar_t*>(value.data()), reinterpret_cast<const wchar_t*>(value.data()) + value.size() / sizeof(wchar_t));
					strValue = stdx::wstring_helper::to_utf8(strwValue);
				}
				else
				{
					strValue.assign(value.begin(), value.end());
				}

				switch (eType)
				{
				case E_TYPE_UINT32:
					size = 4;
					break;

				case E_TYPE_UINT64:
					size = 8;
					break;

				default:
					size = strValue.size();
					break;
				}

				if (!m_pjsObject)
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_invalid_data);

				if (strValue.size() != size)
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_bad_length);

				try
				{
					if (m_pjsObject->exist("type"))
						m_pjsObject->at("type") = eType;
					else
						m_pjsObject->add("type") = eType;

					if (m_pjsObject->exist("value"))
						m_pjsObject->at("value") = CBase64::Encode(strValue);
					else
						m_pjsObject->add("value") = CBase64::Encode(strValue);
				}
				catch (CJsonException&)
				{
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);
				}
			}

			bool exist() const override
			{
				if (!m_pjsObject)
					return false;

				return m_pjsObject->exist(stdx::wstring_helper::to_utf8(getName()));
			}

			void erase() override
			{
				if (!exist())
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);
			}

		private:
			friend class CProfileFolder;

			CProfileValue(const std::wstring& strName, CJsonObject* pjsObject) : StorageValueImpl(strName),
				m_pjsObject(pjsObject) {}

			CProfileValue(const CProfileValue& value) : StorageValueImpl(value),
				m_pjsObject(value.m_pjsObject) {}

		private:
			CJsonObject* m_pjsObject;
		};

		class CProfileFolder : public StorageFolderImpl
		{
		public:
			CLONEABLE(CProfileFolder);
			~CProfileFolder() override = default;

			std::unique_ptr<StorageValueImpl> getValue(const std::wstring& strValue) override
			{
				std::string strUtf = stdx::wstring_helper::to_utf8(strValue);

				if (!m_pjsObject->exist(strUtf))
					m_pjsObject->add(strUtf).setType(CJsonValue::JSON_TYPE_OBJECT);

				return std::unique_ptr<StorageValueImpl>(new CProfileValue(strValue, &static_cast<CJsonObject&>(m_pjsObject->at(strUtf))));
			}

			std::unique_ptr<StorageValueImpl> getConstValue(const std::wstring& strValue) const override
			{
				std::string strUtf = stdx::wstring_helper::to_utf8(strValue);

				if (m_pjsObject && m_pjsObject->exist(strUtf))
					return std::unique_ptr<StorageValueImpl>(new CProfileValue(strValue, &static_cast<CJsonObject&>(m_pjsObject->at(strUtf))));

				return std::unique_ptr<StorageValueImpl>(new CProfileValue(strValue, nullptr));
			}

			std::unique_ptr<StorageFolderImpl> getFolder(const std::wstring& strFolder) override
			{
				if (!m_pjsObject)
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);

				std::string strUtf = stdx::wstring_helper::to_utf8(strFolder);

				if (!m_pjsObject->exist(strUtf))
					m_pjsObject->add(strUtf).setType(CJsonValue::JSON_TYPE_OBJECT);

				return std::unique_ptr<StorageFolderImpl>(new CProfileFolder(strFolder, &static_cast<CJsonObject&>(m_pjsObject->at(strUtf))));
			}

			std::unique_ptr<StorageFolderImpl> getConstFolder(const std::wstring& strFolder) const override
			{
				std::string strUtf = stdx::wstring_helper::to_utf8(strFolder);

				if (m_pjsObject && m_pjsObject->exist(strUtf))
					return std::unique_ptr<StorageFolderImpl>(new CProfileFolder(strFolder, &static_cast<CJsonObject&>(m_pjsObject->at(strUtf))));

				return std::unique_ptr<StorageFolderImpl>(new CProfileFolder(strFolder, nullptr));
			}

			bool exist() const override
			{
				if (!m_pjsObject)
					return false;

				return m_pjsObject->exist(stdx::wstring_helper::to_utf8(getName()));
			}

			void erase() override
			{
				try
				{
					m_pjsObject->clear();
					m_pjsObject = nullptr;
				}
				catch (CJsonException&)
				{
					throw Exception(stdx::wstring_helper::to_utf8(getName()), Exception::err_data_not_found);
				}
			}

		private:
			friend class CProfile;

			CProfileFolder(const std::wstring& strName, CJsonObject* pjsObject) : StorageFolderImpl(strName),
				m_pjsObject(pjsObject) {}

			CProfileFolder(const CProfileFolder& folder) : StorageFolderImpl(folder),
				m_pjsObject(folder.m_pjsObject) {}

		private:
			CJsonObject* m_pjsObject;
		};

		class CProfile : public StorageImpl
		{
		public:
			CLONEABLE(CProfile);
			CProfile(const std::wstring& strFileName)
			{
				if (!strFileName.empty())
					open(strFileName);
			}

			CProfile(const CProfile& profile) :
				m_jsParser(profile.m_jsParser),
				m_strFileName(profile.m_strFileName) {}

			~CProfile() override = default;

			CProfile& operator=(const CProfile& profile)
			{
				m_jsParser = profile.m_jsParser;
				m_strFileName = profile.m_strFileName;

				return *this;
			}

			void open(const std::wstring& strFileName)
			{
				m_strFileName = strFileName;

				std::ifstream ifsFile(strFileName.c_str(), std::ios_base::in | std::ios_base::binary);
				if (ifsFile)
				{
					try
					{
						ifsFile >> m_jsParser;
					}
					catch (CJsonException&)
					{
						throw Exception(stdx::wstring_helper::to_utf8(m_strFileName), Exception::err_data_not_found);
					}
				}
				else
				{
					m_jsParser.setType(CJsonParser::JSON_TYPE_OBJECT);
				}
			}

			void flush() override
			{
				if (!m_strFileName.empty())
				{
					std::ofstream ofsFile(m_strFileName.c_str(), std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
					if (!ofsFile)
						throw Exception(stdx::wstring_helper::to_utf8(m_strFileName), Exception::err_data_not_found);

					try
					{
						ofsFile << m_jsParser;
					}
					catch (CJsonException&)
					{
						throw Exception(stdx::wstring_helper::to_utf8(m_strFileName), Exception::err_data_not_found);
					}
				}
			}

			void close() override
			{
			}

			std::unique_ptr<StorageFolderImpl> getFolder(const std::wstring& strFolder) override
			{
				std::string strUtf = stdx::wstring_helper::to_utf8(strFolder);
				CJsonObject& jsObject = m_jsParser;

				if (!jsObject.exist(strUtf))
					jsObject.add(strUtf).setType(CJsonValue::JSON_TYPE_OBJECT);

				return std::unique_ptr<StorageFolderImpl>(new CProfileFolder(strFolder, &static_cast<CJsonObject&>(jsObject.at(strUtf))));
			}

			std::unique_ptr<StorageFolderImpl> getConstFolder(const std::wstring& strFolder) const override
			{
				std::string strUtf = stdx::wstring_helper::to_utf8(strFolder);

				if (m_jsParser.exist(strUtf))
					return std::unique_ptr<StorageFolderImpl>(new CProfileFolder(strFolder, &static_cast<CJsonObject&>(m_jsParser.at(strUtf))));

				return std::unique_ptr<StorageFolderImpl>(new CProfileFolder(strFolder, nullptr));
			}

		private:
			mutable CJsonParser m_jsParser;
			std::wstring m_strFileName;
		};
	}

	Profile::Profile(const std::wstring& strFileName) : Storage(std::make_unique<CProfile>(strFileName))
	{
	}

	void Profile::open(const std::wstring& strFileName)
	{
		reinterpret_cast<CProfile&>(getImplementation()).open(strFileName);
	}
}
