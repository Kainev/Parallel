#include "Parallel.h"

#include <iostream>
#include <thread>
#include <vector>
#include <cassert>
#include <chrono>
#include <memory>

#include "Parallel/LockFreeDequeue.h"

using TaskQueues	= std::vector<std::unique_ptr<Parallel::LockFreeDequeue<Parallel::Task*>>>;
using TaskPool		= std::vector<std::unique_ptr<Parallel::Task>>;
using ThreadPool	= std::vector<std::thread>;


namespace Parallel
{
	void	thread_execution_loop(std::uint32_t thread_index);
	Task*	get_task(std::uint32_t thread_index);
	TaskID	next_task_id();

	bool				g_running;
	TaskQueues			g_task_queues;
	ThreadPool			g_thread_pool;
	std::uint32_t		g_worker_thread_count;
	TaskPool			g_task_pool;					// TODO: thread_local
	std::uint32_t		g_lifetime_allocated_tasks;		// TODO: thread_local
	std::uint32_t		g_task_pool_size;
	

	void Parallel::initialize(std::uint32_t task_limit)
	{
		assert((task_limit > 0 && (task_limit & (task_limit - 1)) == 0 && "Parameter task_limit MUST be a power of 2"));

		g_worker_thread_count = 0u;
		g_task_pool_size = task_limit;
		g_lifetime_allocated_tasks = 0u;
		g_task_pool.resize(task_limit);

		for(std::uint32_t i = 0u; i < task_limit; i++)
			g_task_pool[i] = std::make_unique<Parallel::Task>();


		g_task_queues.reserve(std::thread::hardware_concurrency());
		g_task_queues.emplace_back(std::make_unique<LockFreeDequeue<Task*>>());
		g_worker_thread_count++;
		g_running = true;
		for (std::size_t i = 1; i < std::thread::hardware_concurrency(); i++)
		{
			g_task_queues.emplace_back(std::make_unique<LockFreeDequeue<Task*>>());
			g_thread_pool.emplace_back(thread_execution_loop, i);
			g_worker_thread_count++;
		}	
	}

	void Parallel::deinitialize()
	{
		g_running = false;
		for (auto& thread : g_thread_pool)
			thread.join();
		g_thread_pool.clear();
		g_worker_thread_count = 0u;
		g_lifetime_allocated_tasks = 0u;
	}

	TaskID Parallel::create_task(TaskFunction function)
	{
		TaskID task_id = next_task_id();
		g_task_pool[task_id]->function = function;
		g_task_pool[task_id]->remaining_tasks = 1;
		g_task_pool[task_id]->task_id = task_id;
		return task_id;
	}

	void Parallel::schedule(TaskID task_id)
	{
		g_task_queues[0]->push(g_task_pool[task_id].get());
	}

	void Parallel::wait(TaskID task_id)
	{
		using namespace std::literals::chrono_literals;

		Task* task = g_task_pool[task_id].get();

		while(task->remaining_tasks > 0)
		{ 
		}
	}

	void thread_execution_loop(std::uint32_t thread_index)
	{
		thread_local int no = 1;
		while (g_running)
		{
			Task* task = get_task(thread_index);

			if (task)
			{
				task->function();
				task->remaining_tasks--;
			    std::cout << thread_index << " Executed Task : " << task->task_id << "\n";
			}
		}
	}

	Task* get_task(std::uint32_t thread_index)
	{
		auto queue = g_task_queues[thread_index].get();

		Task* task = g_task_queues[thread_index]->pop();
		if (!task)
		{
			queue = g_task_queues[0].get();
			task = queue->steal();
		}
		return task;
	}

	TaskID next_task_id()
	{
		return g_lifetime_allocated_tasks++ & (g_task_pool_size - 1u);
	}
}
