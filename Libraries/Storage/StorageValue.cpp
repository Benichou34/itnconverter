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

#include "Storage.h"
#include "StorageImpl.h"
#include <cstring>
#include "stdx/string_helper.h"

namespace storage
{
	SValue::SValue(std::unique_ptr<StorageValueImpl> pImpl) :
		m_pStorageValueImpl(std::move(pImpl))
	{
	}

	SValue::SValue(const SValue& value) :
		m_pStorageValueImpl(value.m_pStorageValueImpl)
	{
	}

	SValue::~SValue()
	{
	}

	SValue& SValue::operator= (const SValue& value)
	{
		if (&value != this)
			m_pStorageValueImpl = value.m_pStorageValueImpl;

		return *this;
	}

	SValue& SValue::operator= (bool bValue)
	{
		toBinary(reinterpret_cast<char*>(&bValue), sizeof(bool));
		return *this;
	}

	SValue& SValue::operator= (int32_t nValue)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(&nValue), reinterpret_cast<const unsigned char*>(&nValue) + sizeof(nValue));
		getImplementation().write(StorageValueImpl::E_TYPE_UINT32, value);
		return *this;
	}

	SValue& SValue::operator= (uint32_t unValue)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(&unValue), reinterpret_cast<const unsigned char*>(&unValue) + sizeof(unValue));
		getImplementation().write(StorageValueImpl::E_TYPE_UINT32, value);
		return *this;
	}

	SValue& SValue::operator= (int64_t nValue)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(&nValue), reinterpret_cast<const unsigned char*>(&nValue) + sizeof(nValue));
		getImplementation().write(StorageValueImpl::E_TYPE_UINT64, value);
		return *this;
	}

	SValue& SValue::operator= (uint64_t unValue)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(&unValue), reinterpret_cast<const unsigned char*>(&unValue) + sizeof(unValue));
		getImplementation().write(StorageValueImpl::E_TYPE_UINT64, value);
		return *this;
	}

	SValue& SValue::operator= (float fValue)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(&fValue), reinterpret_cast<const unsigned char*>(&fValue) + sizeof(fValue));
		getImplementation().write(StorageValueImpl::E_TYPE_UINT32, value);
		return *this;
	}

	SValue& SValue::operator= (double dValue)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(&dValue), reinterpret_cast<const unsigned char*>(&dValue) + sizeof(dValue));
		getImplementation().write(StorageValueImpl::E_TYPE_UINT64, value);
		return *this;
	}

	SValue& SValue::operator= (const std::string& strValue)
	{
		return SValue::operator=(stdx::wstring_helper::from_utf8(strValue));
	}

	SValue& SValue::operator= (const std::wstring& strValue)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(strValue.data()), reinterpret_cast<const unsigned char*>(strValue.data()) + strValue.size() * 2);
		getImplementation().write(StorageValueImpl::E_TYPE_SZ, value);
		return *this;
	}

	SValue& SValue::operator= (const char* szValue)
	{
		return SValue::operator=(stdx::wstring_helper::from_utf8(std::string(szValue)));
	}

	SValue& SValue::operator= (const wchar_t* szValue)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(szValue), reinterpret_cast<const unsigned char*>(szValue) + wcslen(szValue) * sizeof(wchar_t));
		getImplementation().write(StorageValueImpl::E_TYPE_SZ, value);
		return *this;
	}

	void SValue::toBinary(const void* lpValue, size_t nLength)
	{
		std::vector<unsigned char> value(reinterpret_cast<const unsigned char*>(lpValue), reinterpret_cast<const unsigned char*>(lpValue) + nLength);
		toBinary(value);
	}

	void SValue::toBinary(const std::vector<unsigned char>& value)
	{
		getImplementation().write(StorageValueImpl::E_TYPE_BINARY, value);
	}

	SValue::operator bool() const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_BINARY, value);
		return *reinterpret_cast<const bool*>(value.data());
	}

	SValue::operator int32_t() const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_UINT32, value);
		return *reinterpret_cast<const int32_t*>(value.data());
	}

	SValue::operator uint32_t() const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_UINT32, value);
		return *reinterpret_cast<const uint32_t*>(value.data());
	}

	SValue::operator int64_t() const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_UINT64, value);
		return *reinterpret_cast<const int64_t*>(value.data());
	}

	SValue::operator uint64_t() const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_UINT64, value);
		return *reinterpret_cast<const uint64_t*>(value.data());
	}

	SValue::operator float() const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_UINT32, value);
		return *reinterpret_cast<const float*>(value.data());
	}

	SValue::operator double() const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_UINT64, value);
		return *reinterpret_cast<const double*>(value.data());
	}

	SValue::operator std::string() const
	{
		return stdx::wstring_helper::to_utf8(operator std::wstring());
	}

	SValue::operator std::wstring() const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_SZ, value);
		return std::wstring(reinterpret_cast<const wchar_t*>(value.data()), reinterpret_cast<const wchar_t*>(value.data()) + value.size() / sizeof(wchar_t));
	}

	std::vector<unsigned char> SValue::asBinary() const
	{
		std::vector<unsigned char> value;
		asBinary(value);
		return value;
	}

	std::vector<unsigned char> SValue::asBinary(const std::vector<unsigned char>& default_value) const
	{
		try
		{
			return asBinary();
		}
		catch (Exception&)
		{
			return default_value;
		}
	}

	size_t SValue::asBinary(void* lpValue, size_t sLength) const
	{
		std::vector<unsigned char> value;
		getImplementation().read(StorageValueImpl::E_TYPE_BINARY, value);

		if (lpValue)
		{
			if (value.size() < sLength)
				sLength = value.size();

			memcpy(lpValue, value.data(), sLength);
			return sLength;
		}
		else
		{
			return value.size();
		}
	}

	size_t SValue::asBinary(void* lpValue, size_t nLength, unsigned char ucDefault) const
	{
		try
		{
			return asBinary(lpValue, nLength);
		}
		catch (Exception&)
		{
			if (lpValue)
				memset(lpValue, ucDefault, nLength);
			return 0;
		}
	}


	void SValue::asBinary(std::vector<unsigned char>& value) const
	{
		getImplementation().read(StorageValueImpl::E_TYPE_BINARY, value);
	}

	void SValue::asBinary(std::vector<unsigned char>& value, const std::vector<unsigned char>& default_value) const
	{
		try
		{
			asBinary(value);
		}
		catch (Exception&)
		{
			value = default_value;
		}
	}

	bool SValue::operator()(bool bDefault) const
	{
		try
		{
			return static_cast<bool>(*this);
		}
		catch (Exception&)
		{
			return bDefault;
		}
	}

	int32_t SValue::operator()(int32_t nDefault) const
	{
		try
		{
			return static_cast<int32_t>(*this);
		}
		catch (Exception&)
		{
			return nDefault;
		}
	}

	uint32_t SValue::operator()(uint32_t unDefault) const
	{
		try
		{
			return static_cast<uint32_t>(*this);
		}
		catch (Exception&)
		{
			return unDefault;
		}
	}

	int64_t SValue::operator()(int64_t nDefault) const
	{
		try
		{
			return static_cast<int64_t>(*this);
		}
		catch (Exception&)
		{
			return nDefault;
		}
	}

	uint64_t SValue::operator()(uint64_t unDefault) const
	{
		try
		{
			return static_cast<uint64_t>(*this);
		}
		catch (Exception&)
		{
			return unDefault;
		}
	}

	float SValue::operator()(float fDefault) const
	{
		try
		{
			return static_cast<float>(*this);
		}
		catch (Exception&)
		{
			return fDefault;
		}
	}

	double SValue::operator()(double dDefault) const
	{
		try
		{
			return static_cast<double>(*this);
		}
		catch (Exception&)
		{
			return dDefault;
		}
	}

	std::string SValue::operator()(const char* szDefault) const
	{
		return operator()(std::string(szDefault));
	}

	std::string SValue::operator()(const std::string& sDefault) const
	{
		try
		{
			return static_cast<std::string>(*this);
		}
		catch (Exception&)
		{
			return sDefault;
		}
	}

	std::wstring SValue::operator()(const wchar_t* szDefault) const
	{
		return operator()(std::wstring(szDefault));
	}

	std::wstring SValue::operator()(const std::wstring& wsDefault) const
	{
		try
		{
			return static_cast<std::wstring>(*this);
		}
		catch (Exception&)
		{
			return wsDefault;
		}
	}

	const std::wstring& SValue::getName() const
	{
		return getImplementation().getName();
	}

	bool SValue::exist() const
	{
		return getImplementation().exist();
	}

	void SValue::erase()
	{
		getImplementation().erase();
	}

	const StorageValueImpl& SValue::getImplementation() const
	{
		if (!m_pStorageValueImpl)
			throw Exception("SValue", Exception::err_missing_impl);

		return *m_pStorageValueImpl;
	}

	StorageValueImpl& SValue::getImplementation()
	{
		if (!m_pStorageValueImpl)
			throw Exception("SValue", Exception::err_missing_impl);

		return *m_pStorageValueImpl;
	}
}
