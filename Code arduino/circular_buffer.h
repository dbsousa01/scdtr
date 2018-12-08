#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <vector>

class circular_buffer{
private:
	float * buffer;
	int actual_pos;
	int max_pos;
	int loop_count;
public:
	circular_buffer(int size);
	void delete_circular_buffer();
	float read_actual_pos();
	void insert(float value);
	std::vector<float> read_all_values();
};

#endif