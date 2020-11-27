#include <cstdint>
#include <new>
#include <atomic>

namespace Parallel
{
	typedef void(*TaskFunction)();

	struct alignas(std::hardware_destructive_interference_size) Task
	{
		TaskFunction function;
		std::int32_t remaining_tasks;
	};

	typedef std::uint32_t TaskID;

	void	initialize(std::uint32_t task_limit=8192);
	void	deinitialize();
	
	TaskID  create_task(TaskFunction function);
	void	schedule(TaskID task);
	void    wait(TaskID task);
}