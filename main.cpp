#include <iostream>
#include <fstream>
#include <chrono>

#include "conway.hpp"

std::chrono::high_resolution_clock::time_point now(void)
{
	return std::chrono::high_resolution_clock::now();
}

void print_time(std::chrono::high_resolution_clock::time_point t1,
		std::chrono::high_resolution_clock::time_point t2)
{
	std::cout << "time: "
		  << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
		  << " ms"
		  << std::endl;
}

int main()
{
	Conway conway(500, 500);

	const unsigned int gen = 10000;

	std::cout << "generating generation #" << gen << std::endl;

	auto start = now();
	conway.next_generation_n(gen);
	auto end = now();

	print_time(start, end);

	std::ofstream file_out("out.txt");
	
	file_out << conway.get_grid();
	
	return 0;
}
