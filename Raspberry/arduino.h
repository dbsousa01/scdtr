#ifndef ARDUINO_H
#define	ARDUINO_H
#include <iostream>
#include <boost/asio.hpp>
#include <string>
#include <sstream>
#include "circular_buffer.h"
#include <boost/bind.hpp>
#include <chrono>
#include <mutex>
#include "globals.h"
#include <queue>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

using namespace boost::asio;
using namespace boost::system;
using boost::asio::ip::tcp;

class arduino{
private:
	bool occupancy;
	circular_buffer lux_fifo;
	circular_buffer duty_fifo;
	std::chrono::time_point<std::chrono::system_clock> begin_time;
	int lux_lido;
	int duty_cycle;
	int bound;
	float o;
	float ref;
	float energy;
	float comfort_error;
	float variance;
	unsigned long int N;
	float lux[3];
	float sum;
public:
	boost::shared_mutex mutex_;
	std::queue<char *> comms;
	arduino();
	void read_val(float duty_lido, float lux_lido);
	void set_occupancy(bool state);
	void set_ref(float new_ref);
	float get_current_lux();
	int get_lower_bound();
	float get_o();
	float get_current_duty();
	bool get_occupancy();
	float get_ref();
	float get_energy();
	float get_comfort();
	float get_variance();
	std::string get_time();
	void arduino_run();
	void set_o(float ex_o);
	void parseData(char *msg);
	std::vector<float> get_lastminute_duty();
	std::vector<float> get_lastminute_lux();
};

#endif