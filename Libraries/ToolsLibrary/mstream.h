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

#ifndef MEMORY_STREAM_H_
#define MEMORY_STREAM_H_

#include <istream>

 /**
  * At this time, move constructor and move assignment for streams are only implements in Microsoft Visual Studio 2010 and Intel C++ Compiler 12
  */
#if ((__cplusplus > 199711L) || (_HAS_CPP0X > 0)) && ((_MSC_VER >= 1600) || (__INTEL_COMPILER >= 1200))
#define _HAS_CPP11_ 1
#endif

namespace stdx
{
	/**
	 * File mapping stream buffer.
	 * This class applies the functionality of the std::streambuf class to read and write from/to memory-mapped files.
	 * By calling member open, a physical file is associated to the file buffer as its associated character sequence.
	 * Depending on the mode used in this operation, the access to the controlled input sequence or the controlled output sequence may be restricted.
	 * The state of the filemappingbuf object -i.e. whether a file is open or not- may be tested by calling member function is_open.
	 * Internally, filemappingbuf objects operate as defined in the std::streambuf class.
	 * The class overrides some virtual members inherited from streambuf to provide a specific functionality for memory-mapped files.
	 */
	template <class CharT, class Traits = std::char_traits<CharT>>
	class basic_memorybuf : public std::basic_streambuf < CharT, Traits >
	{
		typedef CharT char_type;
		typedef Traits traits_type;

	public:
		basic_memorybuf() : m_pAddress(0), m_Length(0) {}

		/**
		 * Construct object.
		 * A filemappingbuf object is constructed, initializing all its pointers to null pointers and initializing the object's locale.
		 */
		basic_memorybuf(void* ptr_data, std::streamsize data_size) : m_pAddress(0), m_Length(0) { pubsetptr(ptr_data, data_size); }

		/**
		 * Destructs the filemappingbuf object.
		 */
		virtual ~basic_memorybuf() {}

#ifdef _HAS_CPP11_
		/** @name C++11
		 * The following methods requires some features introduced by the latest revision of the C++ standard (2011). Older compilers may not support it.
		 */
		 ///@{ 
		 /**
		  * Move constructor (requires C++11).
		  * Acquires the contents of rhs_buf, by move-assigning its members and base classes.
		  * @param rhs_buf filemappingbuf to move. rhs_buf becomes of invalid state after the operation.
		  * @see swap()
		  * @see operator=()
		  */
		basic_memorybuf(basic_memorybuf&& rhs_buf) : m_pAddress(0), m_Length(0) { swap(rhs_buf); }

		/**
		 * Move assignment (requires C++11).
		 * Closes the source filemappingbuf (as if member close was called), and then acquires the contents of rhs_buf.
		 * @param rhs_buf filemappingbuf to move. rhs_buf becomes of invalid state after the operation.
		 * @return *this.
		 * @see swap()
		 */
		basic_memorybuf& operator=(basic_memorybuf&& rhs_buf)
		{
			swap(rhs_buf);
			return *this;
		}

		/**
		 * Swap internals (requires C++11).
		 * Exchanges the state of the filemappingbuf with those of other.
		 * @param buf filemappingbuf to exchange the state with.
		 * @see operator=()
		 */
		void swap(basic_memorybuf& buf)
		{
			if (this != &buf)
			{
				std::basic_streambuf<CharT, Traits>::swap(buf);
				std::swap(m_pAddress, buf.m_pAddress);
				std::swap(m_Length, buf.m_Length);
			}
		}
		///@}
#endif // _HAS_CPP11_

		/**
		 * Check if a file is open.
		 * The function returns true if a previous call to open succeeded and there have been no calls to the member close since,
		 * meaning that the filemappingbuf object is currently associated with a file.
		 * @return true if a file is open, i.e. associated to this stream buffer object. false otherwise.
		 * @see open()
		 * @see close()
		 */
		bool is_null() const { return (!m_pAddress || !m_Length); }

		basic_memorybuf<CharT, Traits>* pubsetptr(void* ptr, std::streamsize data_size) { return setptr(ptr, data_size); }

		/**
		 * Set internal position pointer to absolute position.
		 * Calls protected virtual member seekptr, which sets a new position value for one or both of the internal position pointers.
		 * The parameter which determines which of the position pointers is affected: either the get pointer or the put pointer, or both.
		 * The function fails if no file is currently open (associated) with this object.
		 * @param ptr New absolute position for the position pointer.
		 * @param which Determines which of the internal position pointers shall be modified: the input pointer, the output pointer, or both. This is an object of type std::ios_base::openmode.
		 * @return In case of success, return the new position value of the modified position pointer. In case of failure, a null pointer is returned.
		 * @see std::streambuf::pubseekpos()
		 * @see std::streambuf::pubseekoff()
		 */
		void* pubseekptr(void* ptr, std::ios_base::openmode which = std::ios_base::in | std::ios_base::out)
		{
			if (is_null())
				return 0;

			return seekptr(ptr, which);
		}

		/**
		 * Get the read-only base address of the mapping.
		 * The function fails if no file is currently open (associated) with this object.
		 * @return In case of success, returns the constant base address of the mapping. In case of failure, a null pointer is returned.
		 * @see size()
		 */
		const void* data() const { return m_pAddress; }

		/**
		 * Get the base address of the mapping.
		 * The function fails if no file is currently open (associated) with this object.
		 * @return In case of success, returns the base address of the mapping. In case of failure, a null pointer is returned.
		 * @see size()
		 */
		void* data() { return m_pAddress; }

		/**
		 * Get the maximum byte length of the mapping.
		 * The function fails if no file is currently open (associated) with this object.
		 * @return In case of success, returns the length of the mapping. In case of failure, 0 is returned.
		 * @see data()
		 */
		std::streamsize size() const { return m_Length; }

	protected:
		virtual std::basic_streambuf<CharT, Traits>* setbuf(char_type* s, std::streamsize n)
		{
			return setptr(s, n * sizeof(char_type));
		}

		virtual std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which)
		{
			switch (way)
			{
			case std::ios_base::beg:
				break;

			case std::ios_base::cur:
				if (which & std::ios_base::in)
					off += static_cast<std::streamoff>(gptr() - static_cast<char_type*>(m_pAddress));
				else
					off += static_cast<std::streamoff>(pptr() - static_cast<char_type*>(m_pAddress));
				break;

			case std::ios_base::end:
				off = m_Length - off;
				break;

			default:
				return -1;
			}

			if (off < 0)
				return -1;

			return seekpos(off, which);
		}

		virtual std::streampos seekpos(std::streampos sp, std::ios_base::openmode which)
		{
			if (sp < 0 || !seekptr(static_cast<char_type*>(m_pAddress) + static_cast<ptrdiff_t>(sp), which))
				return -1;

			return sp;
		}

		basic_memorybuf<CharT, Traits>* setptr(void* ptr, std::streamsize data_size)
		{
			m_pAddress = ptr;
			m_Length = data_size;

			char_type* pBegin = static_cast<char_type*>(m_pAddress);
			char_type* pEnd = reinterpret_cast<char_type*>(!m_pAddress ? 0 : (static_cast<char*>(m_pAddress) + static_cast<ptrdiff_t>(m_Length)));

			setg(pBegin, pBegin, pEnd);
			setp(pBegin, pEnd);

			return this;
		}

		virtual void* seekptr(void* ptr, std::ios_base::openmode which)
		{
			char_type* pcPtr = static_cast<char_type*>(ptr);

			if ((which & std::ios_base::in) && pcPtr != gptr())
			{
				if (pcPtr >= m_pAddress && pcPtr < egptr())
					setg(eback(), pcPtr, egptr());
				else
					return 0;
			}

			if ((which & std::ios_base::out) && pcPtr != pptr())
			{
				if (pcPtr >= m_pAddress && pcPtr < epptr())
					setp(pcPtr, epptr());
				else
					return 0;
			}

			return ptr;
		}

	private:
		/**
		 * Copy constructor is private: this class is not copyable.
		 */
		basic_memorybuf(const basic_memorybuf&);

		/**
		 * Copy operator is private: this class is not copyable.
		 */
		basic_memorybuf& operator=(const basic_memorybuf&);

	private:
		void* m_pAddress;         //!< Base address of the buffer
		std::streamsize m_Length; //!< Length of the buffer
	};

	/**
	 * ifmstream provides an interface to read data from memory-mapped files as input streams.
	 * The objects of this class maintain internally a pointer to a filemappingbuf object that can be obtained by calling member rdbuf.
	 * The file to be associated with the stream can be specified either as a parameter in the constructor or by calling member open.
	 * After all necessary operations on a file have been performed, it can be closed (or disassociated) by calling member close. Once closed, the same file stream object may be used to open another file.
	 * The member function is_open can be used to determine whether the stream object is currently associated with a file.
	 * ifmstream can be used in place of std::ifstream.
	 */
	template <class CharT, class Traits = std::char_traits<CharT>>
	class basic_imstream : public std::basic_istream < CharT, Traits >
	{
		typedef CharT char_type;
		typedef Traits traits_type;

	public:
		/**
		 * Construct object.
		 * Constructs an object of the ifstream class.
		 * This implies the initialization of the associated filemappingbuf object and the call to the constructor of its base class with the filemappingbuf object as parameter.
		 */
		basic_imstream() : std::basic_istream<CharT, Traits>(&m_rdbuf) {}

		/**
		 * Construct object and open a file.
		 * Constructs an object of the ifstream class.
		 * This implies the initialization of the associated filemappingbuf object and the call to the constructor of its base class with the filemappingbuf object as parameter.
		 * The stream is associated with a physical file as if a call to the member function open with the same parameters was made.
		 * If the constructor is not successful in opening the file, the object is still created although no file is associated to the stream buffer and the stream's failbit is set (which can be checked with inherited member fail).
		 * @param path_name C-string contains the name of the file to be opened.
		 * @param max_length Maximum length of the file mapping. If this parameter are 0, the maximum length of the file mapping object is equal to the current size of the file.
		 * @param offset File offset where the mapping begins. They must also match the memory allocation granularity of the system.
		 * That is, the offset must be a multiple of the allocation granularity. To obtain the offset granularity of the system, use filemapping::offset_granularity().
		 * @see open()
		 * @see is_open()
		 * @see close()
		 * @see filemapping::offset_granularity()
		 */
		basic_imstream(void* ptr_data, std::streamsize data_size) : std::basic_istream<CharT, Traits>(&m_rdbuf),
			m_rdbuf(ptr_data, data_size) {}

		/**
		 * Destructs the ifmstream object.
		 */
		virtual ~basic_imstream() {}

#ifdef _HAS_CPP11_
		/** @name C++11
		 * The following methods requires some features introduced by the latest revision of the C++ standard (2011). Older compilers may not support it.
		 */
		 ///@{ 
		 /**
		  * Move constructor (requires C++11).
		  * Acquires the contents of rhs_stream, by move-assigning its members and base classes.
		  * @param rhs_stream File stream to move. rhs_stream becomes of invalid state after the operation.
		  * @see swap()
		  * @see operator=()
		  */
		basic_imstream(basic_imstream&& rhs_stream) : std::basic_istream<CharT, Traits>(&m_rdbuf) { swap(rhs_stream); }

		/**
		 * Move assignment (requires C++11).
		 * Closes the source stream (as if member close was called), and then acquires the contents of rhs_stream.
		 * @param rhs_stream File stream to move. rhs_stream becomes of invalid state after the operation.
		 * @return *this.
		 * @see swap()
		 */
		basic_imstream& operator=(basic_imstream&& rhs_stream) { swap(rhs_stream); return *this; }

		/**
		 * Swap internals (requires C++11).
		 * Exchanges the state of the stream with those of other.
		 * @param stream ifmstream to exchange the state with.
		 * @see operator=()
		 */
		void swap(basic_imstream& stream)
		{
			if (this != &stream)
			{
				std::basic_istream<CharT, Traits>::swap(stream);
				m_rdbuf.swap(stream.m_rdbuf);
			}
		}
		///@}
#endif // _HAS_CPP11_

		basic_imstream& assign(void* ptr_data, std::streamsize data_size)
		{
			if (!rdbuf()->pubsetptr(ptr_data, data_size))
				setstate(std::ios_base::failbit);

			return *this;
		}

		/**
		 * Get the associated filemappingbuf object.
		 * Returns a pointer to the filemappingbuf object associated with the stream.
		 * @return A pointer to the filemappingbuf object associated with the stream.
		 * Notice that for any successfully constructed ifmstream object this pointer is never a null pointer, even if no files have been opened or if the stream is unbuffered.
		 */
		basic_memorybuf<CharT, Traits>* rdbuf() const
		{
			std::basic_streambuf<CharT, Traits>* pBuf = std::basic_istream<CharT, Traits>::rdbuf();
			if (!pBuf)
				return const_cast<basic_memorybuf<CharT, Traits>*>(&m_rdbuf);

			return reinterpret_cast<basic_memorybuf< CharT, Traits>*>(pBuf);
		}

		/**
		 * Check if a file is open.
		 * Returns true if the stream is currently associated with a file, and false otherwise.
		 * The stream is associated with a file if either a previous call to member open succeeded or if the object was successfully constructed using the parameterized constructor, and close has not been called since.
		 * @return true if a file is open, i.e. associated to this stream object. false otherwise.
		 * @see open()
		 * @see close()
		 */
		bool is_null() const { return rdbuf()->is_null(); }

		/**
		 * Get position of the get pointer.
		 * The get pointer determines the next location in the input sequence to be read by the next input operation.
		 * The function fails if no file is currently open (associated) with this object.
		 * @return Return the address of the get pointer. In case of failure, a null pointer is returned.
		 * @see pseekg()
		 * @see std::istream::tellg()
		 */
		const void* ptellg()
		{
			// Return input stream position
			if (fail())
				return 0;

			return static_cast<char*>(rdbuf()->data()) + static_cast<ptrdiff_t>(rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::in));
		}

		/**
		 * Sets the position of the get pointer.
		 * The get pointer determines the next location to be read in the source associated to the stream.
		 * The function fails if no file is currently open (associated) with this object.
		 * On failure, the failbit flag is set (which can be checked with member fail), and depending on the value set with exceptions an exception may be thrown.
		 * @param ptr New absolute position for the input pointer.
		 * @return The function returns *this.
		 * @see ptellg()
		 * @see std::istream::seekg()
		 */
		basic_imstream& pseekg(const void* ptr)
		{
			// Set input stream position to ptr
			if (!fail() && !rdbuf()->pubseekptr(const_cast<void*>(ptr), ios_base::in))
				setstate(std::ios_base::failbit);

			return *this;
		}

		/**
		 * Get the read-only base address of the mapping.
		 * The function fails if no file is currently open (associated) with this object.
		 * @return In case of success, returns the constant base address of the mapping. In case of failure, a null pointer is returned.
		 * @see size()
		 */
		const void* data() const { return (!fail()) ? rdbuf()->data() : 0; }

		/**
		 * Get the maximum byte length of the mapping.
		 * The function fails if no file is currently open (associated) with this object.
		 * @return In case of success, returns the length of the mapping. In case of failure, 0 is returned.
		 * @see data()
		 */
		std::streamsize size() const { return (!fail()) ? rdbuf()->size() : 0; }

	private:
		/**
		 * Copy constructor is private: this class is not copyable.
		 */
		basic_imstream(const basic_imstream&);

		/**
		 * Copy operator is private: this class is not copyable.
		 */
		basic_imstream& operator=(const basic_imstream&);

	private:
		basic_memorybuf<CharT, Traits> m_rdbuf; //!< memorybuf object
	};

	template <class CharT, class Traits = std::char_traits<CharT>>
	class basic_omstream : public std::basic_ostream < CharT, Traits >
	{
		typedef CharT char_type;
		typedef Traits traits_type;

	public:
		/**
		* Construct object.
		* Constructs an object of the ifstream class.
		* This implies the initialization of the associated filemappingbuf object and the call to the constructor of its base class with the filemappingbuf object as parameter.
		*/
		basic_omstream() : std::basic_ostream<CharT, Traits>(&m_rdbuf) {}

		/**
		* Construct object and open a file.
		* Constructs an object of the ifstream class.
		* This implies the initialization of the associated filemappingbuf object and the call to the constructor of its base class with the filemappingbuf object as parameter.
		* The stream is associated with a physical file as if a call to the member function open with the same parameters was made.
		* If the constructor is not successful in opening the file, the object is still created although no file is associated to the stream buffer and the stream's failbit is set (which can be checked with inherited member fail).
		* @param path_name C-string contains the name of the file to be opened.
		* @param max_length Maximum length of the file mapping. If this parameter are 0, the maximum length of the file mapping object is equal to the current size of the file.
		* @param offset File offset where the mapping begins. They must also match the memory allocation granularity of the system.
		* That is, the offset must be a multiple of the allocation granularity. To obtain the offset granularity of the system, use filemapping::offset_granularity().
		* @see open()
		* @see is_open()
		* @see close()
		* @see filemapping::offset_granularity()
		*/
		basic_omstream(void* ptr_data, std::streamsize data_size) : std::basic_ostream(&m_rdbuf),
			m_rdbuf(ptr_data, data_size) {}

		/**
		* Destructs the ifmstream object.
		*/
		virtual ~basic_omstream() {}

#ifdef _HAS_CPP11_
		/** @name C++11
		* The following methods requires some features introduced by the latest revision of the C++ standard (2011). Older compilers may not support it.
		*/
		///@{ 
		/**
		* Move constructor (requires C++11).
		* Acquires the contents of rhs_stream, by move-assigning its members and base classes.
		* @param rhs_stream File stream to move. rhs_stream becomes of invalid state after the operation.
		* @see swap()
		* @see operator=()
		*/
		basic_omstream(basic_omstream&& rhs_stream) : std::basic_ostream(&m_rdbuf) { swap(rhs_stream); }

		/**
		* Move assignment (requires C++11).
		* Closes the source stream (as if member close was called), and then acquires the contents of rhs_stream.
		* @param rhs_stream File stream to move. rhs_stream becomes of invalid state after the operation.
		* @return *this.
		* @see swap()
		*/
		basic_omstream& operator=(basic_omstream&& rhs_stream) { swap(rhs_stream); return *this; }

		/**
		* Swap internals (requires C++11).
		* Exchanges the state of the stream with those of other.
		* @param stream ifmstream to exchange the state with.
		* @see operator=()
		*/
		void swap(basic_omstream& stream)
		{
			if (this != &stream)
			{
				std::basic_ostream<CharT, Traits>::swap(stream);
				m_rdbuf.swap(stream.m_rdbuf);
			}
		}
		///@}
#endif // _HAS_CPP11_

		basic_omstream& assign(void* ptr_data, std::streamsize data_size)
		{
			if (!rdbuf()->pubsetptr(ptr_data, data_size))
				setstate(std::ios_base::failbit);

			return *this;
		}

		/**
		* Get the associated filemappingbuf object.
		* Returns a pointer to the filemappingbuf object associated with the stream.
		* @return A pointer to the filemappingbuf object associated with the stream.
		* Notice that for any successfully constructed ifmstream object this pointer is never a null pointer, even if no files have been opened or if the stream is unbuffered.
		*/
		basic_memorybuf<CharT, Traits>* rdbuf() const
		{
			std::basic_streambuf<CharT, Traits>* pBuf = std::basic_ostream<CharT, Traits>::rdbuf();
			if (!pBuf)
				return const_cast<basic_memorybuf<CharT, Traits>*>(&m_rdbuf);

			return reinterpret_cast<basic_memorybuf< CharT, Traits>*>(pBuf);
		}

		/**
		* Check if a file is open.
		* Returns true if the stream is currently associated with a file, and false otherwise.
		* The stream is associated with a file if either a previous call to member open succeeded or if the object was successfully constructed using the parameterized constructor, and close has not been called since.
		* @return true if a file is open, i.e. associated to this stream object. false otherwise.
		* @see open()
		* @see close()
		*/
		bool is_null() const { return rdbuf()->is_null(); }

		/**
		* Get position of the put pointer.
		* The put pointer determines the location in the output sequence where the next output operation is going to take place.
		* The function fails if no file is currently open (associated) with this object.
		* @return Return the address of the put pointer. In case of failure, a null pointer is returned.
		* @see ptellg()
		* @see pseekp()
		* @see std::istream::tellp()
		*/
		void* ptellp()
		{
			// Return input stream position
			if (fail())
				return 0;

			return static_cast<char*>(rdbuf()->data()) + static_cast<ptrdiff_t>(rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::out));
		}

		/**
		* Sets the position of the put pointer.
		* The put pointer determines the location in the output sequence where the next output operation is going to take place.
		* The function fails if no file is currently open (associated) with this object.
		* On failure, the failbit flag is set (which can be checked with member fail), and depending on the value set with exceptions an exception may be thrown.
		* @param ptr New absolute position for the output pointer.
		* @return The function returns *this.
		* @see pseekg()
		* @see ptellp()
		* @see std::istream::seekp()
		*/
		basic_omstream& pseekp(void* ptr)
		{
			// Set output stream position to ptr
			if (!fail() && !rdbuf()->pubseekptr(ptr, std::ios_base::out))
				setstate(std::ios_base::failbit);

			return *this;
		}

		/**
		* Get the read-only base address of the mapping.
		* The function fails if no file is currently open (associated) with this object.
		* @return In case of success, returns the constant base address of the mapping. In case of failure, a null pointer is returned.
		* @see size()
		*/
		void* data() { return (!fail()) ? rdbuf()->data() : 0; }

		/**
		* Get the maximum byte length of the mapping.
		* The function fails if no file is currently open (associated) with this object.
		* @return In case of success, returns the length of the mapping. In case of failure, 0 is returned.
		* @see data()
		*/
		std::streamsize size() const { return (!fail()) ? rdbuf()->size() : 0; }

	private:
		/**
		* Copy constructor is private: this class is not copyable.
		*/
		basic_omstream(const basic_omstream&);

		/**
		* Copy operator is private: this class is not copyable.
		*/
		basic_omstream& operator=(const basic_omstream&);

	private:
		basic_memorybuf<CharT, Traits> m_rdbuf; //!< memorybuf object
	};

	/**
	 * fmstream provides an interface to read/write data from/to memory-mapped files as input/output streams.
	 * The objects of this class maintain internally a pointer to a filemappingbuf object that can be obtained by calling member rdbuf.
	 * The file to be associated with the stream can be specified either as a parameter in the constructor or by calling member open.
	 * After all necessary operations on a file have been performed, it can be closed (or disassociated) by calling member close.
	 * Once closed, the same file stream object may be used to open another file.
	 * The member function is_open can be used to determine whether the stream object is currently associated with a file.
	 * fmstream can be used in place of std::fstream.
	 */
	template <class CharT, class Traits = std::char_traits<CharT>>
	class basic_mstream : public std::basic_iostream < CharT, Traits >
	{
		typedef CharT char_type;
		typedef Traits traits_type;

	public:
		/**
		* Construct object.
		* Constructs an object of the ifstream class.
		* This implies the initialization of the associated filemappingbuf object and the call to the constructor of its base class with the filemappingbuf object as parameter.
		*/
		basic_mstream() : std::basic_iostream<CharT, Traits>(&m_rdbuf) {}

		/**
		* Construct object and open a file.
		* Constructs an object of the ifstream class.
		* This implies the initialization of the associated filemappingbuf object and the call to the constructor of its base class with the filemappingbuf object as parameter.
		* The stream is associated with a physical file as if a call to the member function open with the same parameters was made.
		* If the constructor is not successful in opening the file, the object is still created although no file is associated to the stream buffer and the stream's failbit is set (which can be checked with inherited member fail).
		* @param path_name C-string contains the name of the file to be opened.
		* @param max_length Maximum length of the file mapping. If this parameter are 0, the maximum length of the file mapping object is equal to the current size of the file.
		* @param offset File offset where the mapping begins. They must also match the memory allocation granularity of the system.
		* That is, the offset must be a multiple of the allocation granularity. To obtain the offset granularity of the system, use filemapping::offset_granularity().
		* @see open()
		* @see is_open()
		* @see close()
		* @see filemapping::offset_granularity()
		*/
		basic_mstream(void* ptr_data, std::streamsize data_size) : std::basic_iostream<CharT, Traits>(&m_rdbuf),
			m_rdbuf(ptr_data, data_size) {}

		/**
		* Destructs the ifmstream object.
		*/
		virtual ~basic_mstream() {}

#ifdef _HAS_CPP11_
		/** @name C++11
		* The following methods requires some features introduced by the latest revision of the C++ standard (2011). Older compilers may not support it.
		*/
		///@{ 
		/**
		* Move constructor (requires C++11).
		* Acquires the contents of rhs_stream, by move-assigning its members and base classes.
		* @param rhs_stream File stream to move. rhs_stream becomes of invalid state after the operation.
		* @see swap()
		* @see operator=()
		*/
		basic_mstream(basic_mstream&& rhs_stream) : std::basic_iostream<CharT, Traits>(&m_rdbuf) { swap(rhs_stream); }

		/**
		* Move assignment (requires C++11).
		* Closes the source stream (as if member close was called), and then acquires the contents of rhs_stream.
		* @param rhs_stream File stream to move. rhs_stream becomes of invalid state after the operation.
		* @return *this.
		* @see swap()
		*/
		basic_mstream& operator=(basic_mstream&& rhs_stream) { swap(rhs_stream); return *this; }

		/**
		* Swap internals (requires C++11).
		* Exchanges the state of the stream with those of other.
		* @param stream ifmstream to exchange the state with.
		* @see operator=()
		*/
		void swap(basic_mstream& stream)
		{
			if (this != &stream)
			{
				std::basic_iostream<CharT, Traits>::swap(stream);
				m_rdbuf.swap(stream.m_rdbuf);
			}
		}
		///@}
#endif // _HAS_CPP11_

		basic_mstream& assign(void* ptr_data, std::streamsize data_size)
		{
			if (!rdbuf()->pubsetptr(ptr_data, data_size))
				setstate(std::ios_base::failbit);

			return *this;
		}

		/**
		* Get the associated filemappingbuf object.
		* Returns a pointer to the filemappingbuf object associated with the stream.
		* @return A pointer to the filemappingbuf object associated with the stream.
		* Notice that for any successfully constructed ifmstream object this pointer is never a null pointer, even if no files have been opened or if the stream is unbuffered.
		*/
		basic_memorybuf<CharT, Traits>* rdbuf() const
		{
			std::basic_streambuf<CharT, Traits>* pBuf = std::basic_iostream<CharT, Traits>::rdbuf();
			if (!pBuf)
				return const_cast<basic_memorybuf<CharT, Traits>*>(&m_rdbuf);

			return reinterpret_cast<basic_memorybuf< CharT, Traits>*>(pBuf);
		}

		/**
		* Check if a file is open.
		* Returns true if the stream is currently associated with a file, and false otherwise.
		* The stream is associated with a file if either a previous call to member open succeeded or if the object was successfully constructed using the parameterized constructor, and close has not been called since.
		* @return true if a file is open, i.e. associated to this stream object. false otherwise.
		* @see open()
		* @see close()
		*/
		bool is_null() const { return rdbuf()->is_null(); }

		/**
		* Get position of the put pointer.
		* The put pointer determines the location in the output sequence where the next output operation is going to take place.
		* The function fails if no file is currently open (associated) with this object.
		* @return Return the address of the put pointer. In case of failure, a null pointer is returned.
		* @see ptellg()
		* @see pseekp()
		* @see std::istream::tellp()
		*/
		void* ptellp()
		{
			// Return input stream position
			if (fail())
				return 0;

			return static_cast<char*>(rdbuf()->data()) + static_cast<ptrdiff_t>(rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::out));
		}

		/**
		* Get position of the get pointer.
		* The get pointer determines the next location in the input sequence to be read by the next input operation.
		* The function fails if no file is currently open (associated) with this object.
		* @return Return the address of the get pointer. In case of failure, a null pointer is returned.
		* @see pseekg()
		* @see std::istream::tellg()
		*/
		const void* ptellg()
		{
			// Return input stream position
			if (fail())
				return 0;

			return static_cast<char*>(rdbuf()->data()) + static_cast<ptrdiff_t>(rdbuf()->pubseekoff(0, std::ios_base::cur, std::ios_base::in));
		}

		/**
		* Sets the position of the put pointer.
		* The put pointer determines the location in the output sequence where the next output operation is going to take place.
		* The function fails if no file is currently open (associated) with this object.
		* On failure, the failbit flag is set (which can be checked with member fail), and depending on the value set with exceptions an exception may be thrown.
		* @param ptr New absolute position for the output pointer.
		* @return The function returns *this.
		* @see pseekg()
		* @see ptellp()
		* @see std::istream::seekp()
		*/
		basic_mstream& pseekp(void* ptr)
		{
			// Set output stream position to ptr
			if (!fail() && !rdbuf()->pubseekptr(ptr, std::ios_base::out))
				setstate(std::ios_base::failbit);

			return *this;
		}

		/**
		* Sets the position of the get pointer.
		* The get pointer determines the next location to be read in the source associated to the stream.
		* The function fails if no file is currently open (associated) with this object.
		* On failure, the failbit flag is set (which can be checked with member fail), and depending on the value set with exceptions an exception may be thrown.
		* @param ptr New absolute position for the input pointer.
		* @return The function returns *this.
		* @see ptellg()
		* @see std::istream::seekg()
		*/
		basic_mstream& pseekg(const void* ptr)
		{
			// Set input stream position to ptr
			if (!fail() && !rdbuf()->pubseekptr(const_cast<void*>(ptr), ios_base::in))
				setstate(std::ios_base::failbit);

			return *this;
		}

		/**
		* Get the read-only base address of the mapping.
		* The function fails if no file is currently open (associated) with this object.
		* @return In case of success, returns the constant base address of the mapping. In case of failure, a null pointer is returned.
		* @see size()
		*/
		const void* data() const { return (!fail()) ? rdbuf()->data() : 0; }

		/**
		* Get the read-only base address of the mapping.
		* The function fails if no file is currently open (associated) with this object.
		* @return In case of success, returns the constant base address of the mapping. In case of failure, a null pointer is returned.
		* @see size()
		*/
		void* data() { return (!fail()) ? rdbuf()->data() : 0; }

		/**
		* Get the maximum byte length of the mapping.
		* The function fails if no file is currently open (associated) with this object.
		* @return In case of success, returns the length of the mapping. In case of failure, 0 is returned.
		* @see data()
		*/
		std::streamsize size() const { return (!fail()) ? rdbuf()->size() : 0; }

	private:
		/**
		* Copy constructor is private: this class is not copyable.
		*/
		basic_mstream(const basic_mstream&);

		/**
		* Copy operator is private: this class is not copyable.
		*/
		basic_mstream& operator=(const basic_mstream&);

	private:
		basic_memorybuf<CharT, Traits> m_rdbuf; //!< memorybuf object
	};

#ifdef _HAS_CPP11_
	/** @name C++11
	 * The following methods requires some features introduced by the latest revision of the C++ standard (2011). Older compilers may not support it.
	 */
	 ///@{ 
	 /**
	  * Swap two basic_memorybuf (requires C++11).
	  * Overloads the std::swap algorithm for filemappingbuf. Exchanges the state of lhs with that of rhs.
	  * Effectively calls lhs.swap(rhs).
	  * @param lhs basic_memorybuf to exchange the state with.
	  * @param rhs basic_memorybuf to exchange the state with.
	  */
	template<class CharT, class Traits>
	void swap(basic_memorybuf<CharT, Traits>& lhs, basic_memorybuf<CharT, Traits>& rhs) { lhs.swap(rhs); }

	/**
	 * Swap two basic_imstream (requires C++11).
	 * Overloads the std::swap algorithm for ifmstream. Exchanges the state of lhs with that of rhs.
	 * Effectively calls lhs.swap(rhs).
	 * @param lhs basic_imstream to exchange the state with.
	 * @param rhs basic_imstream to exchange the state with.
	 */
	template<class CharT, class Traits>
	void swap(basic_imstream<CharT, Traits>& lhs, basic_imstream<CharT, Traits>& rhs) { lhs.swap(rhs); }

	/**
	 * Swap two basic_omstream (requires C++11).
	 * Overloads the std::swap algorithm for ifmstream. Exchanges the state of lhs with that of rhs.
	 * Effectively calls lhs.swap(rhs).
	 * @param lhs basic_omstream to exchange the state with.
	 * @param rhs basic_omstream to exchange the state with.
	 */
	template<class CharT, class Traits>
	void swap(basic_omstream<CharT, Traits>& lhs, basic_omstream<CharT, Traits>& rhs) { lhs.swap(rhs); }

	/**
	 * Swap two basic_mstream (requires C++11).
	 * Overloads the std::swap algorithm for fmstream. Exchanges the state of lhs with that of rhs.
	 * Effectively calls lhs.swap(rhs).
	 * @param lhs basic_mstream to exchange the state with.
	 * @param rhs basic_mstream to exchange the state with.
	 */
	template<class CharT, class Traits>
	void swap(basic_mstream<CharT, Traits>& lhs, basic_mstream<CharT, Traits>& rhs) { lhs.swap(rhs); }
	///@}
#endif // _HAS_CPP11_

	typedef basic_memorybuf<char> memorybuf;
	typedef basic_memorybuf<wchar_t> wmemorybuf;
	typedef basic_imstream<char> imstream;
	typedef basic_imstream<wchar_t> wimstream;
	typedef basic_omstream<char> omstream;
	typedef basic_omstream<wchar_t> womstream;
	typedef basic_mstream<char> mstream;
	typedef basic_mstream<wchar_t> wmstream;
}
#endif // MEMORY_STREAM_H_
