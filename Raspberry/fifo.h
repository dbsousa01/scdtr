#ifndef FIFO_H
#define FIFO_H


class fifo{
private:
	float * buffer;
	int actual_pos;
	int max_pos;
	int loop_count;
public:
	fifo(int size);
	void delete_fifo();
	float read_actual_pos();
	void insert(float value);
};





#endif