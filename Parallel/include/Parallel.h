#include <cstdint>
#include <new>
#include <atomic>

namespace Parallel
{
	typedef void(*TaskFunction)(const void* data);

	struct alignas(std::hardware_destructive_interference_size) Task
	{
		TaskFunction function;
		Task* parent;
		std::atomic<std::uint32_t> children;
		void* data;
	};

	typedef std::uint32_t TaskID;
	typedef std::uint32_t GroupID;

	void	initialize(std::uint32_t task_limit=8192);
	void	deinitialize();
	
	GroupID create_group();
	TaskID  create_task(TaskFunction function);
	TaskID  create_task(GroupID, TaskFunction function);

	/*void	schedule(TaskID task);
	void	schedule(GroupID group);

	void    wait(TaskID task);
	void    wait(GroupID group);*/
}