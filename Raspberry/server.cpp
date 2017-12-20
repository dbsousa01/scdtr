#include <unistd.h>
#include <iostream>
#include <thread>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

#include "pig2i2c.h"
#include "circular_buffer.h"
#include "globals.h"
#include "arduino.h"

#ifndef LENGTH
#define LENGTH 1024
#endif

using namespace boost; 
using namespace boost::asio; 
using boost::asio::ip::tcp;

std::vector<std::shared_ptr <arduino>> ard;
io_service ard1_service;
io_service ard2_service;

io_service io;

class conn 
	:  public enable_shared_from_this<conn>{
public:
   	tcp::socket sock_;
   	std::string msg_;
   	boost::asio::streambuf input_buffer_;
   	boost::system::error_code error;
   	int bytes_Recv;
   	conn(io_service& io):sock_(io){}
   	int flag = 0;
   	char X,I;
   	void h_write(){
   		//Heartbeat sent 
   		async_write(sock_,buffer("\n",1),
    		boost::bind(&conn::h_write, shared_from_this()));

   		boost::asio::async_read_until(sock_, input_buffer_, '\n',
        	boost::bind(&conn::parse_read, this, _1));
   		//do something to perpetuate connection
   		//or close it cleanly
   	}
   	void handle_send(const boost::system::error_code& ec, std::size_t length)
	  {
	    
	    if (!ec)
	    {
	      std::cout << "Sent " << length << " bytes" << std::endl;
	    }
	    else
	    {
	      std::cout << "Error on handle_send: " << ec.message() << "\n";
	    }
	  }	
   	void parse_read(const boost::system::error_code& ec){
		if (!ec)
	    {	
	      // Extract the newline-delimited message from the buffer.
			std::string line;
			std::istream is(&input_buffer_);
			std::getline(is, line);
			char com_buffer[1024];
			memset(com_buffer, '\0',1024);
			char send_message[1024];
			memset(send_message, '\0',1024);
			std::string value;
			int desk=0;

	      // Empty messages are heartbeats and so ignored.
	    	if (!line.empty())
	    	{	
	    		strcpy(com_buffer, line.c_str());
	    		if(flag == 1){
	    			if(com_buffer[0] == 'd' && com_buffer[2]== X && com_buffer[4]==I){
	    				flag = 0;
	    				async_write(sock_,buffer("ack\n"),
    						boost::bind(&conn::h_write, shared_from_this()));
	    				h_write();
	    			}else{
	    				if(X == 'd'){
		      				sprintf(send_message, "c <%c> <%d> <%.3f> <%s>",X, (int)I, ard.at((int)I-1)->get_current_duty(),ard.at(desk-1)->get_time().c_str);
	    				}else if(X == 'l'){
		      				sprintf(send_message, "c <%c> <%d> <%.3f> <%s>",X, (int)I, ard.at((int)I-1)->get_current_lux(),ard.at(desk-1)->get_time().c_str);
	    				}
	    			}
	    		}else{

		    		if(com_buffer[0] == 'g'){
		    			switch(com_buffer[2]){
		      			case 'l' :
		      				std::cout << "Get the current measured illuminance at desk: " << com_buffer[4] << "\n";
		      				desk = boost::lexical_cast<int>(com_buffer[4]);
		      				printf("%d\n", desk);
		      				sprintf(send_message, "l <%d> <%.3f>", desk, ard.at(desk-1)->get_current_lux());
		      				break;
		      			case 'd' :
		      				std::cout << "Get current duty cycle at luminaire: " << com_buffer[4] << "\n";
		      				desk = boost::lexical_cast<int>(com_buffer[4]);
		      				printf("%d\n", desk);
		      				sprintf(send_message, "d <%d> <%.3f>", desk, ard.at(desk-1)->get_current_duty());
		      				break;	
		      			case 'o' :
		      				std::cout << "Get current occupancy state at desk: " << com_buffer[4] << "\n";
		      				desk = boost::lexical_cast<int>(com_buffer[4]);
		      				printf("%d\n", desk);
		      				sprintf(send_message, "o <%d> <%d>", desk,ard.at(desk-1)->get_occupancy());
		      				break;
		      			case 'L' :
		      				std::cout << "Get current illuminance lower bound at desk: " << com_buffer[4] << "\n";
		      				desk = boost::lexical_cast<int>(com_buffer[4]);
		      				printf("%d\n", desk);
		      				sprintf(send_message, "L <%d> <%i>", desk,ard.at(desk-1)->get_lower_bound());	
		      				break;
		      			case 'O' :
		      				std::cout << "Get current external illumninance at desk: " << com_buffer[4] << "\n";
		      				desk = boost::lexical_cast<int>(com_buffer[4]);
		      				printf("%d\n", desk);
		      				sprintf(send_message, "O <%d> <%.3f>", desk,ard.at(desk-1)->get_o());
		      				break;
		      			case 'r' :
		      				std::cout << "Get current illumninance control reference at desk: " << com_buffer[4] << "\n";
		      				desk = boost::lexical_cast<int>(com_buffer[4]);
		      				printf("%d\n", desk);
		      				sprintf(send_message, "r <%d> <%.3f>", desk,ard.at(desk-1)->get_ref());
		      				break;
		      			case 'p' :
		      				if(com_buffer[4] == 'T'){
		      					std::cout << "Get instantaneous total power consumption in the system\n";
		      					float soma = ard.at(0)->get_current_duty() + ard.at(1)->get_current_duty();
		      					sprintf(send_message, "p T <%.3f>",soma);
		      				}else{
		      					std::cout << "Get instantaneous power consumption at desk: " << com_buffer[4] << "\n";
		      					desk = boost::lexical_cast<int>(com_buffer[4]);
		      					printf("%d\n", desk);
		      					sprintf(send_message, "p <%d> <%.3f>", desk,ard.at(desk-1)->get_current_duty());
		      				}
		      				break;
		      			case 'e' :
		      				if(com_buffer[4] == 'T'){
		      					std::cout << "Get total accumulated energy consumption since last system restart\n";
		      					float soma = ard.at(0)->get_energy() + ard.at(1)->get_energy();
		      					sprintf(send_message, "e T <%.3f>",soma);
		      				}else{
		      					std::cout << "Get accumulated energy consumption at desk " << com_buffer[4] << " since the last system restart\n";
		      					desk = boost::lexical_cast<int>(com_buffer[4]);
		      					printf("%d\n", desk);
		      					sprintf(send_message, "e <%d> <%.3f>", desk,ard.at(desk-1)->get_energy());
		      					}
		      				break;
		      			case 'c' :
		      				if(com_buffer[4] == 'T'){
		      					std::cout << "Get total comfort error since last system restart\n";
		      					float soma = ard.at(0)->get_comfort() + ard.at(1)->get_comfort();
		      					sprintf(send_message, "c T <%.3f>",soma);
		      				}else{
		      					std::cout << "Get accumulated comfort error at desk " << com_buffer[4] << " since last system restart\n";
		      					desk = boost::lexical_cast<int>(com_buffer[4]);
		      					printf("%d\n", desk);
		      					sprintf(send_message, "c <%d> <%.3f>", desk,ard.at(desk-1)->get_comfort());
		      				}
		      				break;
		      			case 'v' :
		      				if(com_buffer[4] == 'T'){
		      					std::cout << "Get total comfort variance since last system restart\n";
		      					float soma = ard.at(0)->get_variance() + ard.at(1)->get_variance();
		      					sprintf(send_message, "v T <%.3f>",soma);
		      				}else{
		      					std::cout << "Get accumulated comfort variance at desk " << com_buffer[4] << " since last system restart\n";
		      					desk = boost::lexical_cast<int>(com_buffer[4]);
		      					printf("%d\n", desk);
		      					sprintf(send_message, "v <%d> <%.3f>", desk,ard.at(desk-1)->get_variance());
		      				}
		      				break;
		      			case 't' :
		      				std::cout << "Get elapsed time since the last restart at desk: " << com_buffer[4] << "\n";
		      				desk = boost::lexical_cast<int>(com_buffer[4]);
		      				printf("%d\n", desk);
		      				sprintf(send_message, "t <%d> <%s>", desk,ard.at(desk-1)->get_time().c_str());
		      				break; 
		      			default :
		      				std::cout << "Unexpected data received: " << line << "\n";
		      			}
		    		}else if (com_buffer[0] == 's'){
		    			//Set occupancy state at desk i
		    		}else if (com_buffer[0] == 'r'){
		    			//Restart the system and put the timers to zero
		    		}else if (com_buffer[0] == 'b'){
		    			//Last minute buffer
		    			std::vector<float> values;
		    			desk = boost::lexical_cast<int>(com_buffer[4]);
		    			if(com_buffer[2] == 'l'){
		    				printf("Entrei\n");
		    				values = ard.at(desk-1)->get_lastminute_lux();
		    				std::cout << "Tamanho: "<< values.size() << "\n";
		    			}else if(com_buffer[2] == 'd'){
		    				values = ard.at(desk-1)->get_lastminute_duty();
		    			}else{
		    				std::cout << "Error" << "\n";
		    			}
		    			value += " ";
		    			value += std::to_string(values.at(0));
		    			for(int i = 1; i < values.size(); i++){
		    				value += ", ";
		    				value += std::to_string(values.at(i));
		    			}
		    			sprintf(send_message, "b <%s> <%d> <%s>", com_buffer[2], desk, value.c_str());
		    		}else if(com_buffer[0] == 'c'){
		    			flag = 1;
		    			X = com_buffer[2];
		    			I = com_buffer[4];
		    		}else if(com_buffer[0] == 'd'){
		    			flag = 0;
		    		}else if(line == "quit"){
		    			std::cout << "Exiting program\n";
		    			sock_.close();
		    			exit(1);
		    		}else{
		    			std::cout << "Unexpected message: " << line << "\n";
		    			h_write();
		    		}
		    	}
	    		async_write(sock_,buffer(send_message,strlen(send_message)),
    						boost::bind(&conn::h_write, shared_from_this()));
				//Send value to the client;    
				printf("Mensagem: %s\n",send_message );   
	    	}
	    	h_write();
	      //process the data received with a switch case

	    }
   	}
//public:
	typedef boost::shared_ptr<conn> pointer; 
	static pointer create(io_service& io) {
         return pointer(new conn(io));
    }
    tcp::socket& socket() {
    	return sock_;
    }

    void start() {
    	async_write(sock_,buffer("You are now connected!\n"),
    		boost::bind(&conn::h_write, shared_from_this())); 
    }
};

class tcp_server{
private:  
    tcp::acceptor acceptor_;
public:
	//Acceptor that listens a TCP connection on port x
    tcp_server(io_service& io)
     : acceptor_(io, tcp::endpoint(tcp::v4(), 17001)){
        start_accept(); 
     }
private:  
   void start_accept(){ // Creates a socket
       shared_ptr<conn> new_conn =
            conn::create(acceptor_.get_io_service());

       acceptor_.async_accept(new_conn->socket(),
       boost::bind(&tcp_server::h_accept, this, new_conn));
   }
  void h_accept(shared_ptr<conn> new_conn){ //Accepts incoming calls
         new_conn->start();
         start_accept();
     }
};

void tcp_run(){
	try{
		// IO Services such as sockets and servers
	    io_service io;
	    tcp_server server(io);
	    //An object that performs asynch operations on our behalf
	    io.run();	
	}
	catch(std::exception& e){
		//Error condition
		std::cerr << e.what() << std::endl;
	}
}

void concurrent_function(){ //function used just to check if multithreading is working
	int counter =	0;	
	while(++counter<10){
	std::cout<<counter<<":Hello from Child Thread #" << std::this_thread::get_id()<<std::endl;
		std::this_thread::yield();	//pass	time	slot	to	other	thread
	}
}

void ard_thread_1(){
  try
  {
    ard1_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}
/**
 * [ard_thread_1 thread function of io_service 2]
 */
void ard_thread_2(){
  try
  {
    ard2_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }
}

int main( )  {

	std::shared_ptr <arduino> ard1(new arduino());
    std::shared_ptr <arduino> ard2(new arduino());
    //std::shared_ptr<arduino> ard1 = std::make_shared <arduino>(ard1_service);
    //std::shared_ptr<arduino> ard2 = std::make_shared <arduino>(ard2_service);

    ard.push_back(ard1);
    ard.push_back(ard2);

	std::thread t1{tcp_run};
	std::thread t2{read_sniffer};
	std::thread t3{&arduino::arduino_run, ard1};
	std::thread t4{&arduino::arduino_run, ard2};
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	return 0;
}