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

namespace storage
{
	Storage::Storage()
	{
	}

	Storage::Storage(std::unique_ptr<StorageImpl> pImpl) :
		m_pParamStorageImpl(std::move(pImpl))
	{
	}

	Storage::Storage(const Storage& storage) :
		m_pParamStorageImpl(storage.m_pParamStorageImpl)
	{
	}

	Storage::~Storage()
	{
		close();
	}

	Storage& Storage::operator=(const Storage& storage)
	{
		if (&storage != this)
		{
			close();
			m_pParamStorageImpl = storage.m_pParamStorageImpl;
		}

		return *this;
	}

	void Storage::init(std::unique_ptr<StorageImpl> pImpl)
	{
		close();
		m_pParamStorageImpl = std::move(pImpl);
	}

	void Storage::flush()
	{
		getImplementation().flush();
	}

	void Storage::close()
	{
		if (m_pParamStorageImpl)
		{
			m_pParamStorageImpl->flush();
			m_pParamStorageImpl->close();
		}
	}

	SFolder Storage::operator() (const std::wstring& strFolder)
	{
		return SFolder(getImplementation().getFolder(strFolder));
	}

	const SFolder Storage::operator() (const std::wstring& strFolder) const
	{
		return SFolder(getImplementation().getConstFolder(strFolder));
	}

	const StorageImpl& Storage::getImplementation() const
	{
		if (!m_pParamStorageImpl)
			throw Exception("Storage", Exception::err_missing_impl);

		return *m_pParamStorageImpl;
	}

	StorageImpl& Storage::getImplementation()
	{
		if (!m_pParamStorageImpl)
			throw Exception("Storage", Exception::err_missing_impl);

		return *m_pParamStorageImpl;
	}
}
