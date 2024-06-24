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
 * Purpose: Provide read-write access to memory-mapped files on Windows and POSIX systems.
 */

#include "fmstream.h"

class filemapping_base
{
public:
	virtual ~filemapping_base() = default;

	virtual std::streamsize open(const char* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset, char*& pAddress, std::streamsize& map_length) = 0;
	virtual std::streamsize open(const wchar_t* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset, char*& pAddress, std::streamsize& map_length) = 0;
	virtual int sync(char* pAddress, std::streamsize map_length) = 0;
	virtual void close(char* pAddress, std::streamsize map_length) = 0;
};

#ifdef _WIN32
#include <windows.h>

namespace
{
	template<class T> HANDLE CreateFileT(const T*, DWORD, DWORD);

	template<>
	HANDLE CreateFileT<char>(const char* lpFileName, DWORD dwDesiredAccess, DWORD dwCreationDisposition)
	{
		return CreateFileA(lpFileName, dwDesiredAccess, 0, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
	}

	template<>
	HANDLE CreateFileT<wchar_t>(const wchar_t* lpFileName, DWORD dwDesiredAccess, DWORD dwCreationDisposition)
	{
		return CreateFileW(lpFileName, dwDesiredAccess, 0, nullptr, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
	}
}

class filemapping_windows : public filemapping_base
{
	template<class T>
	std::streamsize open(const T* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset, char*& pAddress, std::streamsize& map_length)
	{
		std::streamsize FileSize = 0;
		DWORD dwDesiredAccess = GENERIC_READ;
		DWORD flProtect = PAGE_READONLY;
		DWORD dwMapAccess = FILE_MAP_READ;
		DWORD dwCreationDisposition = OPEN_EXISTING;
		DWORD dwFileSizeHigh = 0;
		DWORD dwFileOffsetHigh = 0;

		if (mode & std::ios_base::out)
		{
			dwDesiredAccess |= GENERIC_WRITE;
			flProtect = PAGE_READWRITE;
			dwMapAccess |= FILE_MAP_WRITE;
			dwCreationDisposition = OPEN_ALWAYS;
		}

		m_pFile = CreateFileT(path_name, dwDesiredAccess, dwCreationDisposition);
		if (m_pFile == INVALID_HANDLE_VALUE)
			return FileSize;

		FileSize = GetFileSize(m_pFile, &dwFileSizeHigh);
#ifdef _WIN64
		FileSize |= static_cast<std::streamsize>(dwFileSizeHigh) << 32;
#endif

		if (max_length)
			map_length = max_length;
		else
			map_length = FileSize - static_cast<std::streamsize>(offset);

		if (!(mode & std::ios_base::out) && (static_cast<std::streamsize>(offset) + map_length) > FileSize)
			map_length = FileSize - static_cast<std::streamsize>(offset);

		if (map_length <= 0)
			return FileSize;

		std::streamsize NewFileSize = static_cast<std::streamsize>(offset) + map_length;

#ifdef _WIN64
		dwFileSizeHigh = static_cast<DWORD>(NewFileSize >> 32);
		dwFileOffsetHigh = static_cast<DWORD>(offset >> 32);
#else
		dwFileSizeHigh = 0;
#endif

		m_pFileMapping = CreateFileMappingA(m_pFile, nullptr, flProtect, dwFileSizeHigh, static_cast<DWORD>(NewFileSize), nullptr);
		if (!m_pFileMapping)
			return FileSize;

		pAddress = static_cast<char*>(MapViewOfFile(m_pFileMapping, dwMapAccess, dwFileOffsetHigh, static_cast<DWORD>(offset), static_cast<SIZE_T>(map_length)));

		return FileSize;
	}

public:
	filemapping_windows() : m_pFile(INVALID_HANDLE_VALUE), m_pFileMapping(nullptr) {}

	std::streamsize open(const char* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset, char*& pAddress, std::streamsize& map_length) override
	{
		return open<char>(path_name, mode, max_length, offset, pAddress, map_length);
	}

	std::streamsize open(const wchar_t* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset, char*& pAddress, std::streamsize& map_length) override
	{
		return open<wchar_t>(path_name, mode, max_length, offset, pAddress, map_length);
	}

	int sync(char* pAddress, std::streamsize map_length) override
	{
		return (FlushViewOfFile(pAddress, static_cast<SIZE_T>(map_length)) != FALSE) ? 0 : -1;
	}

	void close(char* pAddress, std::streamsize) override
	{
		if (pAddress)
			UnmapViewOfFile(pAddress);

		if (m_pFileMapping)
		{
			CloseHandle(m_pFileMapping);
			m_pFileMapping = nullptr;
		}

		if (m_pFile != INVALID_HANDLE_VALUE)
		{
			CloseHandle(m_pFile);
			m_pFile = INVALID_HANDLE_VALUE;
		}
	}

private:
	HANDLE m_pFile;              //!< Windows handle to the file mapping object
	HANDLE m_pFileMapping;       //!< Windows handle to the opened file
};
#else // If not Windows, this is a POSIX system !
#include <stddef.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <locale>
#include <vector>

class  filemapping_posix : public filemapping_base
{
public:
	filemapping_posix() : m_fd(-1) {}

	std::streamsize open(const char* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset, char*& pAddress, std::streamsize& map_length) override
	{
		std::streamsize FileSize = 0;
		int oflag = O_RDONLY;
		int flags = PROT_READ;
		mode_t ar = 0;

		if (mode & std::ios_base::out)
		{
			oflag = O_RDWR | O_CREAT;
			flags |= PROT_WRITE;
			ar = S_IRUSR | S_IWUSR;
		}

		m_fd = ::open(path_name, oflag, ar);
		if (m_fd == -1)
			return FileSize;

		struct stat statbuf;

		// Get the file size
		if (fstat(m_fd, &statbuf) != 0)
			return FileSize;

		FileSize = statbuf.st_size;

		if (max_length)
			map_length = max_length;
		else
			map_length = FileSize - static_cast<std::streamsize>(offset);

		if (static_cast<std::streamsize>(offset) + map_length > FileSize)
		{
			if (mode & std::ios_base::out)
			{
				/* Something needs to be written at the end of the file to
				* have the file actually have the new size.
				* Just writing an empty string at the current file position will do.
				*
				* Note:
				*  - The current position in the file is at the end of the stretched
				*    file due to the call to lseek().
				*  - An empty string is actually a single '\0' character, so a zero-byte
				*    will be written at the last byte of the file.
				*/
				if (lseek(m_fd, static_cast<std::streamsize>(offset) + map_length - 1, SEEK_SET) == -1)
					return FileSize;

				if (write(m_fd, "", 1) != 1)
					return FileSize;
			}
			else
			{
				map_length = FileSize - static_cast<std::streamsize>(offset);

				if (map_length <= 0)
					return FileSize;
			}
		}

		pAddress = static_cast<char*>(mmap(nullptr, static_cast<size_t>(map_length), flags, MAP_SHARED, m_fd, offset));
		if (pAddress == MAP_FAILED)
			pAddress = nullptr;

		return FileSize;
	}

	std::streamsize open(const wchar_t* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset, char*& pAddress, std::streamsize& map_length) override
	{
		typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
		std::setlocale(LC_ALL, "");

		const converter_type& converter = std::use_facet<converter_type>(std::locale());
		std::wstring ws(path_name);
		std::vector<char> to(ws.length() * converter.max_length());

		std::mbstate_t state;
		const wchar_t* from_next;
		char* to_next;

		converter_type::result result = converter.out(state, ws.data(), ws.data() + ws.length(), from_next, &to[0], &to[0] + to.size(), to_next);
		if (result != converter_type::ok && result != converter_type::noconv)
			return 0;

		return open(&to[0], mode, max_length, offset, pAddress, map_length);
	}

	int sync(char* pAddress, std::streamsize map_length) override
	{
		return msync(pAddress, static_cast<size_t>(map_length), MS_ASYNC);
	}

	void close(char* pAddress, std::streamsize map_length) override
	{
		if (pAddress && map_length)
			munmap(pAddress, static_cast<size_t>(map_length));

		if (m_fd != -1)
		{
			::close(m_fd);
			m_fd = -1;
		}
	}

private:
	int m_fd;                    //!< File descriptor to the opened file
};
#endif


std::streamoff filemapping::offset_granularity()
{
#ifdef _WIN32
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	return static_cast<std::streamoff>(SystemInfo.dwAllocationGranularity);
#else // If not Windows, this is a POSIX system !
	return static_cast<std::streamoff>(sysconf(_SC_PAGE_SIZE));
#endif
}

/* filemappingbuf class */
filemappingbuf::filemappingbuf() :
	m_pAddress(nullptr),
	m_MapLength(0),
#ifdef _WIN32
	m_pMapping(new filemapping_windows)
#else // If not Windows, this is a POSIX system !
	m_pMapping(new filemapping_posix)
#endif
{
}

filemappingbuf::~filemappingbuf()
{
	close();
}

#ifdef _HAS_CPP11_
filemappingbuf::filemappingbuf(filemappingbuf&& rhs_buf) :
	m_pAddress(nullptr),
	m_MapLength(0)
{
	swap(rhs_buf);
}

filemappingbuf& filemappingbuf::operator=(filemappingbuf&& rhs_buf)
{
	if (this != &rhs_buf)
	{
		close();
		swap(rhs_buf);
	}

	return *this;
}

void filemappingbuf::swap(filemappingbuf& buf)
{
	if (this != &buf)
	{
		std::streambuf::swap(buf);
		std::swap(m_pAddress, buf.m_pAddress);
		std::swap(m_MapLength, buf.m_MapLength);
		std::swap(m_pMapping, buf.m_pMapping);
	}
}

void swap(filemappingbuf& lhs, filemappingbuf& rhs)
{
	lhs.swap(rhs);
}
#endif // _HAS_CPP11_

bool filemappingbuf::is_open() const
{
	return (m_pAddress && m_MapLength);
}

void* filemappingbuf::pubseekptr(void* ptr, std::ios_base::openmode which)
{
	if (!is_open())
		return nullptr;

	return seekptr(ptr, which);
}

std::streampos filemappingbuf::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
{
	switch (way)
	{
	case std::ios_base::beg:
		break;

	case std::ios_base::cur:
		if (which & std::ios_base::in)
			off += static_cast<std::streamoff>(gptr() - m_pAddress);
		else
			off += static_cast<std::streamoff>(pptr() - m_pAddress);
		break;

	case std::ios_base::end:
		off = m_MapLength - off;
		break;

	default:
		return -1;
	}

	if (off < 0)
		return -1;

	return seekpos(off, which);
}

std::streampos filemappingbuf::seekpos(std::streampos sp, std::ios_base::openmode which)
{
	if (sp < 0 || !seekptr(m_pAddress + static_cast<ptrdiff_t>(sp), which))
		return -1;

	return sp;
}

void* filemappingbuf::seekptr(void* ptr, std::ios_base::openmode which)
{
	char* pcPtr = static_cast<char*>(ptr);

	if ((which & std::ios_base::in) && pcPtr != gptr())
	{
		if (pcPtr >= m_pAddress && pcPtr < egptr())
			setg(eback(), pcPtr, egptr());
		else
			return nullptr;
	}

	if ((which & std::ios_base::out) && pcPtr != pptr())
	{
		if (pcPtr >= m_pAddress && pcPtr < epptr())
			setp(pcPtr, epptr());
		else
			return nullptr;
	}

	return ptr;
}

filemappingbuf* filemappingbuf::open(const char* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset)
{
	if (is_open() || max_length < 0 || offset < 0) // Check if a file is already opened and parameters
		return nullptr;

	std::streamsize FileSize = m_pMapping->open(path_name, mode, max_length, offset, m_pAddress, m_MapLength);
	if (!m_pAddress)
		return nullptr;

	char* pEnd = m_pAddress + static_cast<ptrdiff_t>(m_MapLength);

	setg(m_pAddress, m_pAddress, pEnd);

	if (mode & std::ios_base::ate) // At end
		setp(m_pAddress + static_cast<ptrdiff_t>(FileSize - offset), pEnd);
	else
		setp(m_pAddress, pEnd);

	return this;
}

filemappingbuf* filemappingbuf::open(const wchar_t* path_name, std::ios_base::openmode mode, std::streamsize max_length, std::streamoff offset)
{
	if (is_open() || max_length < 0 || offset < 0) // Check if a file is already opened and parameters
		return nullptr;

	std::streamsize FileSize = m_pMapping->open(path_name, mode, max_length, offset, m_pAddress, m_MapLength);
	if (!m_pAddress)
		return nullptr;

	char* pEnd = m_pAddress + static_cast<ptrdiff_t>(m_MapLength);

	setg(m_pAddress, m_pAddress, pEnd);

	if (mode & std::ios_base::ate) // At end
		setp(m_pAddress + static_cast<ptrdiff_t>(FileSize - offset), pEnd);
	else
		setp(m_pAddress, pEnd);

	return this;
}

int filemappingbuf::sync()
{
	if (m_pAddress && m_MapLength)
		return m_pMapping->sync(m_pAddress, m_MapLength);

	return -1;
}

const void* filemappingbuf::data() const
{
	return m_pAddress;
}

void* filemappingbuf::data()
{
	return m_pAddress;
}

std::streamsize filemappingbuf::size() const
{
	return m_MapLength;
}

filemappingbuf* filemappingbuf::close()
{
	m_pMapping->close(m_pAddress, m_MapLength);

	if (!is_open())
		return nullptr;

	m_pAddress = nullptr;
	m_MapLength = 0;

	setg(nullptr, nullptr, nullptr);
	setp(nullptr, nullptr);

	return this;
}

/* ifmstream class */
ifmstream::ifmstream() : std::istream(&m_rdbuf)
{}

ifmstream::ifmstream(const char* path_name, std::streamsize max_length, std::streamoff offset) : std::istream(&m_rdbuf)
{
	open(path_name, max_length, offset);
}

ifmstream::ifmstream(const wchar_t* path_name, std::streamsize max_length, std::streamoff offset) : std::istream(&m_rdbuf)
{
	open(path_name, max_length, offset);
}

#ifdef _HAS_CPP11_
ifmstream::ifmstream(ifmstream&& rhs_stream) : std::istream(&m_rdbuf)
{
	swap(rhs_stream);
}

ifmstream& ifmstream::operator=(ifmstream&& rhs_stream)
{
	if (this != &rhs_stream)
	{
		m_rdbuf.close();
		swap(rhs_stream);
	}

	return *this;
}

void ifmstream::swap(ifmstream& stream)
{
	if (this != &stream)
	{
		std::istream::swap(stream);
		m_rdbuf.swap(stream.m_rdbuf);
	}
}

void swap(ifmstream& lhs, ifmstream& rhs)
{
	lhs.swap(rhs);
}
#endif // _HAS_CPP11_

filemappingbuf* ifmstream::rdbuf() const
{
	return const_cast<filemappingbuf*>(&m_rdbuf);
}

bool ifmstream::is_open() const
{
	return m_rdbuf.is_open();
}

void ifmstream::open(const char* path_name, std::streamsize max_length, std::streamoff offset)
{
	if (m_rdbuf.open(path_name, std::ios_base::in, max_length, offset) == nullptr)
		setstate(std::ios_base::failbit);
	else
		clear();
}

void ifmstream::open(const wchar_t* path_name, std::streamsize max_length, std::streamoff offset)
{
	if (m_rdbuf.open(path_name, std::ios_base::in, max_length, offset) == nullptr)
		setstate(std::ios_base::failbit);
	else
		clear();
}

void ifmstream::close()
{
	if (m_rdbuf.close() == nullptr)
		setstate(std::ios_base::failbit);
}

const void* ifmstream::ptellg()
{
	// Return input stream position
	if (!fail())
		return static_cast<char*>(m_rdbuf.data()) + static_cast<ptrdiff_t>(m_rdbuf.pubseekoff(0, std::ios_base::cur, std::ios_base::in));
	else
		return nullptr;
}

std::istream& ifmstream::pseekg(const void* ptr)
{
	// Set input stream position to ptr
	if (!fail() && !m_rdbuf.pubseekptr(const_cast<void*>(ptr), ios_base::in))
		setstate(std::ios_base::failbit);
	return *this;
}

const void* ifmstream::data() const
{
	return (!fail()) ? m_rdbuf.data() : nullptr;
}

std::streamsize ifmstream::size() const
{
	return (!fail()) ? m_rdbuf.size() : 0;
}

/* fmstream class */
fmstream::fmstream() : std::iostream(&m_rdbuf)
{}

fmstream::fmstream(const char* path_name, std::streamsize max_length, std::streamoff offset) : std::iostream(&m_rdbuf)
{
	open(path_name, max_length, offset);
}

fmstream::fmstream(const wchar_t* path_name, std::streamsize max_length, std::streamoff offset) : std::iostream(&m_rdbuf)
{
	open(path_name, max_length, offset);
}

#ifdef _HAS_CPP11_
fmstream::fmstream(fmstream&& rhs_stream) : std::iostream(&m_rdbuf)
{
	swap(rhs_stream);
}

fmstream& fmstream::operator=(fmstream&& rhs_stream)
{
	if (this != &rhs_stream)
	{
		m_rdbuf.close();
		swap(rhs_stream);
	}

	return *this;
}

void fmstream::swap(fmstream& stream)
{
	if (this != &stream)
	{
		std::iostream::swap(stream);
		m_rdbuf.swap(stream.m_rdbuf);
	}
}

void swap(fmstream& lhs, fmstream& rhs)
{
	lhs.swap(rhs);
}
#endif // _HAS_CPP11_

filemappingbuf* fmstream::rdbuf() const
{
	return const_cast<filemappingbuf*>(&m_rdbuf);
}

bool fmstream::is_open() const
{
	return m_rdbuf.is_open();
}

void fmstream::open(const char* path_name, std::streamsize max_length, std::streamoff offset)
{
	if (m_rdbuf.open(path_name, std::ios_base::in | std::ios_base::out, max_length, offset) == nullptr)
		setstate(std::ios_base::failbit);
	else
		clear();
}

void fmstream::open(const wchar_t* path_name, std::streamsize max_length, std::streamoff offset)
{
	if (m_rdbuf.open(path_name, std::ios_base::in | std::ios_base::out, max_length, offset) == nullptr)
		setstate(std::ios_base::failbit);
	else
		clear();
}

void fmstream::close()
{
	if (m_rdbuf.close() == nullptr)
		setstate(std::ios_base::failbit);
}

const void* fmstream::ptellg()
{
	// Return input stream position
	if (!fail())
		return static_cast<char*>(m_rdbuf.data()) + static_cast<ptrdiff_t>(m_rdbuf.pubseekoff(0, std::ios_base::cur, std::ios_base::in));
	else
		return nullptr;
}

void* fmstream::ptellp()
{
	// Return input stream position
	if (!fail())
		return static_cast<char*>(m_rdbuf.data()) + static_cast<ptrdiff_t>(m_rdbuf.pubseekoff(0, std::ios_base::cur, std::ios_base::out));
	else
		return nullptr;
}

std::istream& fmstream::pseekg(const void* ptr)
{
	// Set input stream position to ptr
	if (!fail() && !m_rdbuf.pubseekptr(const_cast<void*>(ptr), std::ios_base::in))
		setstate(std::ios_base::failbit);
	return *this;
}

std::ostream& fmstream::pseekp(void* ptr)
{
	// Set output stream position to ptr
	if (!fail() && !m_rdbuf.pubseekptr(ptr, std::ios_base::out))
		setstate(std::ios_base::failbit);
	return *this;
}

const void* fmstream::data() const
{
	return (!fail()) ? m_rdbuf.data() : nullptr;
}

void* fmstream::data()
{
	return (!fail()) ? m_rdbuf.data() : nullptr;
}

std::streamsize fmstream::size() const
{
	return (!fail()) ? m_rdbuf.size() : 0;
}
