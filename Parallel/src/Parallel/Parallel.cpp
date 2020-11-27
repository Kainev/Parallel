#include "Parallel.h"

#include <thread>
#include <vector>
#include <atomic>
#include <cassert>

using ThreadPool = std::vector<std::thread>;
using TaskPool = std::vector<Parallel::Task>;
using GroupFlags = std::vector<bool>;


namespace Parallel
{
	void thread_execution_loop();
	TaskID get_available_task();

	ThreadPool			g_thread_pool;
	TaskPool			g_task_pool;
	std::uint32_t		g_task_pool_size;
	std::atomic<TaskID>	g_lifetime_allocated_tasks;
	GroupFlags			g_initialized_groups;
	


	void Parallel::initialize(std::uint32_t task_limit)
	{
		assert((task_limit > 0 && (task_limit & (task_limit - 1)) == 0 && "Parameter task_limit MUST be a power of 2"));

		g_task_pool_size = task_limit;
		g_lifetime_allocated_tasks = 0u;

		g_task_pool.resize(g_task_pool_size);
		g_initialized_groups.resize(g_task_pool_size);

		for (std::size_t i = 0; i < std::thread::hardware_concurrency() - 1u; i++)
			g_thread_pool.emplace_back(thread_execution_loop);
	}

	void Parallel::deinitialize()
	{
		for (auto& thread : g_thread_pool)
			thread.join();
		g_thread_pool.clear();
		g_lifetime_allocated_tasks = 0u;
	}

	GroupID Parallel::create_group()
	{
		GroupID id = static_cast<GroupID>(get_available_task());
		g_initialized_groups[id] = false;

		return id;
	}

	TaskID create_task(TaskFunction function)
	{
		TaskID task_id = get_available_task();
		g_task_pool[task_id].function = function;
		return task_id;
	}

	TaskID Parallel::create_task(GroupID group, TaskFunction function)
	{
		if (g_initialized_groups[group])
		{
			TaskID task_id = get_available_task();
			Task* task = &g_task_pool[task_id];
			task->function = function;
			task->parent = &g_task_pool[group];
			task->children = 0u;
			return task_id;
		}
		else
		{
			Task* task = &g_task_pool[group];
			task->function = function;
			g_initialized_groups[group] = true;
			return group;
		}
	}

	void thread_execution_loop()
	{
	}

	TaskID get_available_task()
	{
		return g_lifetime_allocated_tasks++ & (g_task_pool_size - 1u);
	}
}
