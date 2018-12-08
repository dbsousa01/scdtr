#include "circular_buffer.h"
#include "globals.h"

using namespace std;
//Creates a fifo of size int
circular_buffer::circular_buffer(int size){
	buffer = new float[size];
	actual_pos = 0;
	max_pos = size;
	loop_count = 0;
}

//deletes the fifo
void circular_buffer::delete_circular_buffer(){
	delete [] buffer;
}

//reads the current posifion
float circular_buffer::read_actual_pos(){
	return buffer[actual_pos - 1];
}

//inserts a new value
void circular_buffer::insert(float value){
	if(actual_pos < max_pos){ //normal insert
		buffer[actual_pos] = value;
		actual_pos = actual_pos + 1;
	}else{ //if the end of the buffer is reached, loop it and add the value
		loop_count = loop_count + 1;
		actual_pos = 0;
		buffer[actual_pos] = value;
		actual_pos = actual_pos + 1;
	}
}

vector<float> circular_buffer::read_all_values(){
  if(actual_pos == 0){
    vector<float> fifth;
    return fifth;
  }

  if(loop_count == 0){
    float return_vector [actual_pos];
    for(int i =0;i<actual_pos;i++){
      return_vector[i]=buffer[i];
    }
    vector<float> fifth (return_vector, return_vector + sizeof(return_vector) / sizeof(float) );
    return fifth;
  }

    float return_vector [max_pos];
    int j = 0;
    for(int i=actual_pos;i<max_pos;i++){
      return_vector[j] = buffer[i];
      j=j+1;
    }
    for(int i=0;i<actual_pos;i++){
      return_vector[j] = buffer[i];
      j=j+1;
    }

    vector<float> fifth (return_vector, return_vector + sizeof(return_vector) / sizeof(float) );
    return fifth;
}