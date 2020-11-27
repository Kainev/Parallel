#pragma once

#include <cstdint>
#include <atomic>
#include <vector>

namespace Parallel
{
	template<typename T>
	class LockFreeDequeue
	{
	public:
		LockFreeDequeue(std::uint32_t max_size=8192) : s_index_mask{ max_size - 1u }
		{
			m_queue.resize(max_size);
		};

		T		pop();
		T		steal();
		void	push(T item);
	private:
		std::vector<T>				m_queue;
		std::atomic<std::uint32_t>	m_bottom = 0u;
		std::atomic<std::uint32_t>	m_top	 = 0u;

		const std::uint32_t			s_index_mask;
	};


	template<typename T>
	inline T LockFreeDequeue<typename T>::pop()
	{
		std::uint32_t b = m_bottom.fetch_sub(1, std::memory_order_seq_cst) - 1;
		std::uint32_t t = m_top.load(std::memory_order_seq_cst);

		if (t <= b)
		{
			T item = m_queue[b & s_index_mask];

			if (t != b)
				return item;

			if (!m_top.compare_exchange_strong(t, t + 1u), std::memory_order_seq_cst, std::memory_order_relaxed)
				item = nullptr;

			m_bottom.store(t + 1, std::memory_order_relaxed);
			return item;
		}
		else
		{
			m_bottom.store(t, std::memory_order_relaxed);
			return nullptr;
		}
	}

	template<typename T>
	inline T LockFreeDequeue<typename T>::steal()
	{
		std::uint32_t t = m_top.load(std::memory_order_seq_cst);
		std::uint32_t b = m_bottom.load(std::memory_order_acquire);

		if (t < b)
		{
			T item = m_queue[t & s_index_mask];
			if (!m_top.compare_exchange_strong(t, t + 1u), std::memory_order_seq_cst, std::memory_order_relaxed)
				return nullptr;

			return item;
		}
		else
			return nullptr;
	}

	template<typename T>
	inline void LockFreeDequeue<typename T>::push(T item)
	{
		std::uint32_t b = m_bottom.load(std::memory_order_relaxed);
		m_queue[b & s_index_mask] = item;
		m_bottom.store(b + 1, std::memory_order_release);
	}
}