#include "Parallel.h"

#include <string>
#include <algorithm>
#include <iostream>


void task_function(const void* data)
{

}

int main()
{
	Parallel::initialize();


	//Parallel::TaskID task = Parallel::create_task(task_function);

	Parallel::GroupID group = Parallel::create_group();

	//Parallel::create_task(group, task_function);

	//Parallel::schedule(group);

	Parallel::deinitialize();
	return 0;
}