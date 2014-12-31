#include <vector>
#include <string>
#include <stack>
#include <mutex>
#include <list>
#include <future>

class Conway
{
private:
	unsigned int cell_size;
	unsigned int width;
	unsigned int height;

	unsigned int x_threads;
	unsigned int y_threads;

	unsigned int x_cells_per_thread;
	unsigned int y_cells_per_thread;
	
	std::list< std::future<void> > threads;

	std::mutex mut;

	std::vector<unsigned int> cells;

	std::stack<unsigned int> kill_stack;
	std::stack<unsigned int> grow_stack;

	unsigned int get_index(unsigned int x, unsigned int y);
	unsigned int get_neighbours(unsigned int x, unsigned int y);


	void process_cell(unsigned int x, unsigned int y);
	void process_cell_range(unsigned int x_start, unsigned int x_end,
				unsigned int y_start, unsigned int y_end);
	void kill_cells();
	void grow_cells();
public:
	Conway();
	Conway(unsigned int w, unsigned int h);

	std::string get_grid();
	
	void randomize();
	void next_generation();
	void next_generation_n(unsigned int count);
};
