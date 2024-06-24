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
 * Purpose: Waitable thread-safe queue.
 */

#ifndef STDX_CONCURRENT_QUEUE_H
#define STDX_CONCURRENT_QUEUE_H

#include <deque>
#include <memory>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <chrono>

namespace stdx
{
	enum class cq_status
	{
		no_timeout,
		timeout
	};

	template<class T, class Container = std::deque<T>>
	class concurrent_queue
	{
	public:
		typedef typename Container::size_type size_type;

		concurrent_queue() : m_MaxSize(m_Container.max_size()) { }
		concurrent_queue(const concurrent_queue& cq) : m_Container(cq.m_Container), m_MaxSize(cq.m_MaxSize) { }
		concurrent_queue(concurrent_queue&& cq) : m_Container(std::move(cq.m_Container)), m_MaxSize(std::move(cq.m_MaxSize)) { }

		explicit concurrent_queue(size_type maxsize) : m_MaxSize(maxsize)
		{
			if (!m_MaxSize)
				m_MaxSize = m_Container.max_size();
		}

		~concurrent_queue()
		{
			clear();
		}

		concurrent_queue& operator= (const concurrent_queue& cq)
		{
			std::lock_guard<std::mutex> mlg(m_QueueMutex);
			if (&x != this)
			{
				m_Queue = cq.m_Queue;
				m_MaxSize = cq.m_MaxSize;
			}

			return *this;
		}

		concurrent_queue& operator= (concurrent_queue&& cq)
		{
			std::lock_guard<std::mutex> mlg(m_QueueMutex);
			if (&x != this)
			{
				clear();
				std::swap(m_Queue, cq.m_Queue);
				std::swap(m_MaxSize, cq.m_MaxSize);
			}

			return *this;
		}

		void push_front(const T& value, bool bWait = true)
		{
			std::unique_lock<std::mutex> mul(m_QueueMutex);
			while (m_Container.size() == m_MaxSize)
			{
				if (bWait)
					m_QueueCondition.wait(mul);
				else
					m_Container.pop_back();
			}

			m_Container.push_front(value);
			m_QueueCondition.notify_one();
		}

		void push_back(const T& value, bool bWait = true)
		{
			std::unique_lock<std::mutex> mul(m_QueueMutex);
			while (m_Container.size() == m_MaxSize)
			{
				if (bWait)
					m_QueueCondition.wait(mul);
				else
					m_Container.pop_front();
			}

			m_Container.push_back(value);
			m_QueueCondition.notify_one();
		}

		cq_status pop_front(T& value)
		{
			return pop_front(value, std::chrono::duration<size_t>::max());
		}

		template<class Rep, class Period>
		cq_status pop_front(T& value, const std::chrono::duration<Rep, Period>& rel_time)
		{
			std::unique_lock<std::mutex> mul(m_QueueMutex);
			while (m_Container.empty())
			{
				if (m_QueueCondition.wait_for(mul, rel_time) == std::cv_status::timeout)
					return cq_status::timeout;
			}

			value = std::move(m_Container.front());
			m_Container.pop_front();
			return cq_status::no_timeout;
		}

		cq_status pop_back(T& value)
		{
			return pop_back(value, std::chrono::duration<size_t>::max());
		}

		template<class Rep, class Period>
		cq_status pop_back(T& value, const std::chrono::duration<Rep, Period>& rel_time)
		{
			std::unique_lock<std::mutex> mul(m_QueueMutex);
			while (m_Container.empty())
			{
				if (m_QueueCondition.wait_for(mul, rel_time) == std::cv_status::timeout)
					return cq_status::timeout;
			}

			value = std::move(m_Container.back());
			m_Container.pop_back();
			return cq_status::no_timeout;
		}

		cq_status try_pop_front(T& value)
		{
			return pop_front(value, std::chrono::duration<size_t>::zero());
		}

		cq_status try_pop_back(T& value)
		{
			return pop_back(value, std::chrono::duration<size_t>::zero());
		}

		void clear()
		{
			std::lock_guard<std::mutex> mlg(m_QueueMutex);
			m_Container.clear();
		}

		bool empty() const
		{
			std::lock_guard<std::mutex> mlg(m_QueueMutex);
			return m_Container.empty();
		}

		bool full() const
		{
			std::lock_guard<std::mutex> mlg(m_QueueMutex);
			return (m_Container.size() == m_MaxSize);
		}

		size_type size() const
		{
			std::lock_guard<std::mutex> mlg(m_QueueMutex);
			return m_Container.size();
		}

		size_type max_size() const
		{
			std::lock_guard<std::mutex> mlg(m_QueueMutex);
			return m_MaxSize;
		}

		void resize(size_type maxsize)
		{
			if (!maxsize)
				maxsize = m_Container.max_size();

			std::lock_guard<std::mutex> mlg(m_QueueMutex);
			m_MaxSize = maxsize;

			while (m_MaxSize < m_Container.size())
				m_Container.pop_back();
		}

	private:
		mutable std::mutex m_QueueMutex;
		std::condition_variable m_QueueCondition;
		Container m_Container;
		size_type m_MaxSize;
	};
}
#endif // STDX_CONCURRENT_QUEUE_H
