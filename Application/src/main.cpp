#include "Parallel.h"


void task_function()
{

}

int main()
{

	Parallel::TaskID task = Parallel::create_task(task_function);

	Parallel::GroupID group = Parallel::create_group();

	Parallel::create_task(group, task_function);

	Parallel::schedule(group);

	return 0;
}