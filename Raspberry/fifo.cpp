#include "fifo.h"

using namespace std;
//Creates a fifo of size int
fifo::fifo(int size){
	buffer = new float[size];
	actual_pos = 0;
	max_pos = size;
	loop_count = 0;
}

//deletes the fifo
void fifo::delete_fifo(){
	delete [] buffer;
}

//reads the current posifion
float fifo::read_actual_pos(){
	return buffer[actual_pos - 1];
}

//inserts a new value
void fifo::insert(float value){
	if(actual_pos < max_pos){ //normal insert
		buffer[actual_pos] = value;
		actual_pos++;
	}else{ //if the end of the buffer is reached, loop it and add the value
		loop_count++;
		actual_pos = 0;
		buffer[actual_pos] = value;
		actual_pos++;
	}
}

int main(){
	return 0;

	//Usar isto depois, já tá separado em .cpp e .h, apagar isto depois
}