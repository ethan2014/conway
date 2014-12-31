#include "conway.hpp"

#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>
#include <future>
#include <list>

#define ALIVE 1
#define DEAD 0

/*
 * Constructors
 */

Conway::Conway() :
	Conway(100, 100)
{
}

Conway::Conway(unsigned int w, unsigned int h) :
	cell_size(0),
	width(w),
	height(h),
	x_threads(2),
	y_threads(2),
	x_cells_per_thread(width / x_threads),
	y_cells_per_thread(height / y_threads),
	threads(x_threads * y_threads),
	cells(w * h)
{
	randomize();
}

/*
 * private member functions
 */

unsigned int Conway::get_index(unsigned int x, unsigned int y)
{
	return (y * width) + x;
}

unsigned int Conway::get_neighbours(unsigned int x, unsigned int y)
{
	unsigned int ret = 0;
	
	const int start_x = (x == 0 ? 0 : -1);
	const int start_y = (y == 0 ? 0 : -1);
	const int end_x = (x == width - 1 ? 0 : 1);
	const int end_y = (y == height - 1 ? 0 : 1);

	for (int yt = start_y; yt <= end_y; yt++) {
		for (int xt = start_x; xt <= end_x; xt++) {
			if (yt == 0 && xt == 0) {
				continue;
			}
			
			if (cells[get_index(x + xt, y + yt)]) {
				ret++;
			}
		}
	}

	return ret;
}

void Conway::process_cell(unsigned int x, unsigned int y)
{
	const unsigned int neighbours = get_neighbours(x, y);
	const unsigned int index = get_index(x, y);

	const unsigned int alive = cells[index];

	if (alive && (neighbours < 2 || neighbours > 3)) {
		// cell is alive and should die

		std::unique_lock<std::mutex> lock(mut);
		kill_stack.push(index);
	} else if (neighbours == 3) {
		// cell is dead but should become alive

		std::unique_lock<std::mutex> lock(mut);
		grow_stack.push(index);
	}
}

void Conway::process_cell_range(unsigned int x_start, unsigned int x_end,
				unsigned int y_start, unsigned int y_end)
{
	for (unsigned int y = y_start; y < y_end; y++) {
		for (unsigned int x = x_start; x < x_end; x++) {
			process_cell(x, y);
		}
	}
}

void Conway::kill_cells()
{
	while (!kill_stack.empty()) {
		const unsigned int pos = kill_stack.top();
		kill_stack.pop();
		cells[pos] = DEAD;
	}
}

void Conway::grow_cells()
{
	while (!grow_stack.empty()) {
		const unsigned int pos = grow_stack.top();
		grow_stack.pop();
		cells[pos] = ALIVE;
	}
}

/*
 * public member functions
 */

void Conway::randomize()
{
	std::fill(std::begin(cells), std::end(cells), DEAD);

	std::random_device rd;
	std::default_random_engine rand(rd());

	std::generate(std::begin(cells), std::end(cells), [&] { return rand() % 2; });
}

std::string Conway::get_grid()
{
	std::stringstream ret;

	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			ret << cells[get_index(x, y)];
		}

		ret << std::endl;
	}

	return ret.str();
}

void Conway::next_generation()
{
	threads.clear();

	for (unsigned int y = 0; y < y_threads; y++) {
		for (unsigned int x = 0; x < x_threads; x++) {
			threads.push_back(std::async(std::launch::async,
						     &Conway::process_cell_range, this,
						     x * x_cells_per_thread,
						     x * x_cells_per_thread + x_cells_per_thread,
						     y * y_cells_per_thread,
						     y * y_cells_per_thread + y_cells_per_thread));
		}
	}

	for (std::future<void> &t : threads)	{
		if (t.valid())
		    t.get();
	}
	
	kill_cells();
	grow_cells();
}

void Conway::next_generation_n(unsigned int count)
{
	for (unsigned int i = 0; i < count; i++) {
		next_generation();
	}
}
