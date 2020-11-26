#include <cstdint>

typedef void(*TaskFunction)();


namespace Parallel
{
	typedef std::uint32_t TaskID;
	typedef std::uint32_t GroupID;
	
	GroupID create_group();
	TaskID  create_task(TaskFunction function);
	TaskID  create_task(TaskID, TaskFunction function);

	void	schedule(TaskID task);
	void	schedule(GroupID group);

	void    wait(TaskID task);
	void    wait(GroupID group);
}