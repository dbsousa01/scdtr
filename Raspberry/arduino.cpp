#include "arduino.h"
#include "globals.h"
#include "circular_buffer.h"

#define SAMPLE_TIME 15 //ms - change accordingly

arduino::arduino()//io_service $io)
	:
	lux_fifo((1000*60)/SAMPLE_TIME),
	duty_fifo((1000*60)/SAMPLE_TIME)
{
	//begin_time = std::chrono::system_clock::now();
	occupancy = 0;
	lux_lido = 0;
	duty_cycle = 0;
	bound = 15;
	o = 0;
	ref = 0;
	energy = 0;
	comfort_error = 0;
	variance = 0;
	N = 0;
	lux[0] = 0; lux[1] = 0; lux[2] = 0;
	sum = 0;
	begin_time = std::chrono::system_clock::now();
}

void arduino::read_val(float duty_lido, float lux_lido){
	N++;

	duty_fifo.insert(duty_lido);
	lux_fifo.insert(lux_lido);

	lux[2] = lux[1];
	lux[1] = lux[0];
	lux[0] = lux_lido;

	//power returns duty cycle;

	energy += duty_lido * SAMPLE_TIME;

	//Compute the comfort error
	if( ref > lux_lido){
		comfort_error += (ref-lux_lido);
	}
	//Calculate variance
	if(N>=3){
		sum += abs(lux[0] - 2*lux[1] + lux[2]);
		float aux = pow(SAMPLE_TIME, 2);
		variance = sum/aux;
	}

}

void arduino::set_o(float ex_o){
	o = ex_o;
}

void arduino::set_occupancy(bool state){
	if(state == 0)
		occupancy = 0;
	else if(state == 1)
		occupancy = 1;

	bound = 25*occupancy + 15;
}

void arduino::set_ref(float new_ref){
	ref = new_ref;
}

float arduino::get_current_lux(){
	return lux_fifo.read_actual_pos();
}

int arduino::get_lower_bound(){
	return bound;
}

float arduino::get_o(){
	return o;
}

float arduino::get_current_duty(){
	return duty_fifo.read_actual_pos();
}

bool arduino::get_occupancy(){
	return occupancy;
}

float arduino::get_ref(){
	return ref;
}

float arduino::get_energy(){
	return energy;
}

float arduino::get_comfort(){
	return comfort_error;
}

float arduino::get_variance(){
	return variance;
}

std::string arduino::get_time(){
	std::ostringstream out;
	std::chrono::time_point<std::chrono::system_clock> t1 = std::chrono::system_clock::now();
	out << std::chrono::duration<double>(t1 - begin_time).count() << 's';
	std::string s = out.str();
	return s;
}

void arduino::parseData(char * msg){
	
	char* strtok_aux = strtok(msg,";"); //reads the address, irrelevant
	if(strtok_aux == NULL)
		return;

	strtok_aux = strtok(NULL,";");
	if(strtok_aux == NULL)
		return;
	arduino::set_ref(atof(strtok_aux)*100); // Sets the new ref

	strtok_aux = strtok(NULL,";"); //Saves the read lux
	if(strtok_aux == NULL)
		return;
	float lux_lido = atof(strtok_aux)*100;

	strtok_aux = strtok(NULL,";"); //Saves the read duty cycle
	if(strtok_aux == NULL)
		return;
	float duty_lido = atof(strtok_aux);

	strtok_aux = strtok(NULL,";");
	if(strtok_aux == NULL)
		return;
	arduino::set_o(atof(strtok_aux));

	arduino::read_val(duty_lido,lux_lido);	
	
}

void arduino::arduino_run(){
	char * msg;

	while(1){
		boost::unique_lock<boost::shared_mutex> lock(mutex_);
		if(!comms.empty()){
			msg = comms.front();
			comms.pop();
			//boost::unique_lock<boost::shared_mutex> unlock(mutex_);
			arduino::parseData(msg);
		}else{
			//boost::unique_lock<boost::shared_mutex> unlock(mutex_);
		}
	}
}
std::vector<float> arduino::get_lastminute_duty(){
      return duty_fifo.read_all_values();
}

std::vector<float> arduino::get_lastminute_lux(){
      return lux_fifo.read_all_values();
}
