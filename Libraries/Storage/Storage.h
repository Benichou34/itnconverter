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

#ifndef STORAGE_H_INCLUDED
#define STORAGE_H_INCLUDED

#include <vector>
#include <memory>
#include <stdint.h>
#include "stdx/prototype.h"
#include "StorageException.h"

namespace storage
{
	class StorageValueImpl;
	class StorageFolderImpl;
	class StorageImpl;

	class SValue
	{
	public:
		SValue(const SValue& value);
		~SValue();

		SValue& operator= (const SValue& strValue);
		SValue& operator= (bool bValue);
		SValue& operator= (int32_t nValue);
		SValue& operator= (uint32_t unValue);
		SValue& operator= (int64_t nValue);
		SValue& operator= (uint64_t unValue);
		SValue& operator= (float fValue);
		SValue& operator= (double dValue);
		SValue& operator= (const std::string& strValue);
		SValue& operator= (const std::wstring& strValue);
		SValue& operator= (const char* szValue);
		SValue& operator= (const wchar_t* szValue);

		template<typename T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
		SValue& operator= (T value)
		{
			return operator=(static_cast<int32_t>(value));
		}

		template<typename T, typename = typename std::enable_if<!std::is_enum<T>::value>::type>
		SValue& operator= (const T& value)
		{
			toBinary(&value, sizeof(T));
			return *this;
		}

		void toBinary(const void* lpValue, size_t nLength);
		void toBinary(const std::vector<unsigned char>& value);

		operator bool() const;
		operator int32_t() const;
		operator uint32_t() const;
		operator int64_t() const;
		operator uint64_t() const;
		operator float() const;
		operator double() const;
		operator std::string() const;
		operator std::wstring() const;

		size_t asBinary(void* lpValue, size_t nLength) const;
		size_t asBinary(void* lpValue, size_t nLength, unsigned char ucDefault) const;

		void asBinary(std::vector<unsigned char>& value) const;
		void asBinary(std::vector<unsigned char>& value, const std::vector<unsigned char>& default_value) const;

		std::vector<unsigned char> asBinary() const;
		std::vector<unsigned char> asBinary(const std::vector<unsigned char>& default_value) const;

		bool operator()(bool bDefault) const;
		int32_t operator()(int32_t nDefault) const;
		uint32_t operator()(uint32_t unDefault) const;
		int64_t operator()(int64_t nDefault) const;
		uint64_t operator()(uint64_t unDefault) const;
		float operator()(float fDefault) const;
		double operator()(double dDefault) const;
		std::string operator()(const char* szDefault) const;
		std::string operator()(const std::string& sDefault) const;
		std::wstring operator()(const wchar_t* szDefault) const;
		std::wstring operator()(const std::wstring& wsDefault) const;

		template<typename T, typename = typename std::enable_if<std::is_enum<T>::value>::type>
		T operator()(T d) const
		{
			try
			{
				return static_cast<T>(static_cast<int32_t>(*this));
			}
			catch (Exception&)
			{
				return d;
			}
		}

		const std::wstring& getName() const;
		bool exist() const;
		void erase();

	private:
		friend class SFolder;
		explicit SValue(std::unique_ptr<StorageValueImpl> pImpl);

		const StorageValueImpl& getImplementation() const;
		StorageValueImpl& getImplementation();

	private:
		stdx::clone_ptr<StorageValueImpl> m_pStorageValueImpl;
	};

	class SFolder
	{
	public:
		SFolder(const SFolder& folder);
		~SFolder();

		SFolder& operator=(const SFolder& folder);

		SValue operator[] (const std::wstring& strValue);
		const SValue operator[] (const std::wstring& strValue) const;

		SFolder operator() (const std::wstring& strFolder);
		const SFolder operator() (const std::wstring& strFolder) const;

		const std::wstring& getName() const;
		bool exist() const;
		void erase();

	private:
		friend class Storage;
		explicit SFolder(std::unique_ptr<StorageFolderImpl> pImpl);

		const StorageFolderImpl& getImplementation() const;
		StorageFolderImpl& getImplementation();

	private:
		stdx::clone_ptr<StorageFolderImpl> m_pStorageFolderImpl;
	};

	class Storage
	{
	public:
		Storage();
		Storage(const Storage& storage);
		virtual ~Storage();

		Storage& operator=(const Storage& storage);

		void init(std::unique_ptr<StorageImpl> pImpl);
		void flush();
		void close();

		SFolder operator() (const std::wstring& strFolder);
		const SFolder operator() (const std::wstring& strFolder) const;

	protected:
		explicit Storage(std::unique_ptr<StorageImpl> pImpl);

		const StorageImpl& getImplementation() const;
		StorageImpl& getImplementation();

	private:
		stdx::clone_ptr<StorageImpl> m_pParamStorageImpl;
	};
}

#endif // !STORAGE_H_INCLUDED
