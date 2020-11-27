#include <iostream>
#include <array>
#include <cmath>

#include "Parallel.h"


void heavy_function()
{
	double d = 0.0;
	for (int i = 0; i < 1000000; i++)
		d = std::exp2(std::sqrt(d));

	std::cout << "Function!\n";
}


void light_function()
{

}


int main()
{
	Parallel::initialize();

	const std::size_t task_count = 4096;
	std::array<Parallel::TaskID, task_count> tasks;


	for (int i = 0; i < task_count; i++)
	{
		tasks[i] = Parallel::create_task(light_function);
		Parallel::schedule(tasks[i]);
	}


	for (int i = 0; i < task_count; ++i)
		Parallel::wait(tasks[i]);


	Parallel::deinitialize();


	return 0;
}