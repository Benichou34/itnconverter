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

#ifndef STORAGE_IMPL_H_INCLUDED
#define STORAGE_IMPL_H_INCLUDED

#include <string>
#include <vector>
#include "stdx/prototype.h"

namespace storage
{
	class StorageValueImpl : public stdx::cloneable<StorageValueImpl>
	{
	public:
		typedef enum
		{
			E_TYPE_NONE,
			E_TYPE_BINARY,
			E_TYPE_UINT32,
			E_TYPE_UINT64,
			E_TYPE_SZ
		} type_t;

		StorageValueImpl(const std::wstring& strName) : m_strName(strName) {}
		virtual ~StorageValueImpl() = default;

		StorageValueImpl(const StorageValueImpl&) = default;
		StorageValueImpl& operator=(const StorageValueImpl&) = default;

		virtual void read(type_t eType, std::vector<unsigned char>& value) const = 0;
		virtual void write(type_t eType, const std::vector<unsigned char>& value) = 0;

		virtual bool exist() const = 0;
		virtual void erase() = 0;
		const std::wstring& getName() const { return m_strName; }

	private:
		std::wstring m_strName;
	};

	class StorageFolderImpl : public stdx::cloneable<StorageFolderImpl>
	{
	public:
		StorageFolderImpl(const std::wstring& strName) : m_strName(strName) {}
		virtual ~StorageFolderImpl() {}

		StorageFolderImpl(const StorageFolderImpl&) = default;
		StorageFolderImpl& operator=(const StorageFolderImpl&) = default;

		virtual std::unique_ptr<StorageValueImpl> getValue(const std::wstring& strValue) = 0;
		virtual std::unique_ptr<StorageValueImpl> getConstValue(const std::wstring& strValue) const = 0;

		virtual std::unique_ptr<StorageFolderImpl> getFolder(const std::wstring& strFolder) = 0;
		virtual std::unique_ptr<StorageFolderImpl> getConstFolder(const std::wstring& strFolder) const = 0;

		virtual bool exist() const = 0;
		virtual void erase() = 0;
		const std::wstring& getName() const { return m_strName; }

	private:
		std::wstring m_strName;
	};

	class StorageImpl : public stdx::cloneable<StorageImpl>
	{
	public:
		virtual ~StorageImpl() {}

		virtual std::unique_ptr<StorageFolderImpl> getFolder(const std::wstring& strFolder) = 0;
		virtual std::unique_ptr<StorageFolderImpl> getConstFolder(const std::wstring& strFolder) const = 0;

		virtual void flush() = 0;
		virtual void close() = 0;
	};
}
#endif // !STORAGE_IMPL_H_INCLUDED
