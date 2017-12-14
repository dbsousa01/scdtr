#include <unistd.h>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>

#ifndef LENGTH
#define LENGTH 1024
#endif

using namespace boost; 
using namespace boost::asio; 
using boost::asio::ip::tcp;

class conn 
	:  public enable_shared_from_this<conn>{
private:
   	tcp::socket sock_;
   	std::string msg_;
   	boost::asio::streambuf input_buffer_;
   	boost::system::error_code error;
   	int bytes_Recv;
   	conn(io_service& io):sock_(io){}
   	void h_write(){
   		//Heartbeat sent 
   		async_write(sock_,buffer("  "),
    		boost::bind(&conn::h_write, shared_from_this()));


   		boost::asio::async_read_until(sock_, input_buffer_, '\n',
        	boost::bind(&conn::parse_read, this, _1));
   		//do something to perpetuate connection
   		//or close it cleanly
   	}
   	void parse_read(const boost::system::error_code& ec){
		if (!ec)
	    {
	      // Extract the newline-delimited message from the buffer.
			std::string line;
			std::istream is(&input_buffer_);
			std::getline(is, line);

			char buffer[1024];
			memset(buffer, '\0',1024);
			std::string value;


	      // Empty messages are heartbeats and so ignored.
	    	if (!line.empty())
	    	{	
	    		strcpy(buffer, line.c_str());
	    		if(buffer[0] == 'g'){
	    			switch(buffer[2]){
	      			case 'l' :
	      				std::cout << "Get the current measured illuminance at desk: " << buffer[4] << "\n";
	      				// value << something;
	      				break;
	      			case 'd' :
	      				std::cout << "Get current duty cycle at luminaire: " << buffer[4] << "\n";
	      				// value << something;
	      				break;	
	      			case 'o' :
	      				std::cout << "Get current occupancy state at desk: " << buffer[4] << "\n";
	      				break;
	      			case 'L' :
	      				std::cout << "Get current illuminance lower bound at desk: " << buffer[4] << "\n";	
	      				break;
	      			case 'O' :
	      				std::cout << "Get current external illumninance at desk: " << buffer[4] << "\n";
	      				break;
	      			case 'r' :
	      				std::cout << "Get current illumninance control reference at desk: " << buffer[4] << "\n";
	      				break;
	      			case 'p' :
	      				if(buffer[4] == 'T'){
	      					std::cout << "Get instantaneous total power consumption in the system\n"; 
	      				}else{
	      					std::cout << "Get instantaneous power consumption at desk: " << buffer[4] << "\n";
	      				}
	      				break;
	      			case 'e' :
	      				if(buffer[4] == 'T'){
	      					std::cout << "Get total accumulated energy consumption since last system restart\n";
	      				}else{
	      					std::cout << "Get accumulated energy consumption at desk " << buffer[4] << " since the last system restart\n";
	      				}
	      				break;
	      			case 'c' :
	      				if(buffer[4] == 'T'){
	      					std::cout << "Get total comfort error since last system restart\n";
	      				}else{
	      					std::cout << "Get accumulated comfort error at desk " << buffer[4] << " since last system restart\n";
	      				}
	      				break;
	      			case 'v' :
	      				if(buffer[4] == 'T'){
	      					std::cout << "Get total comfort variance since last system restart\n";
	      				}else{
	      					std::cout << "Get accumulated comfort variance at desk " << buffer[4] << " since last system restart\n";
	      				}
	      				break;
	      			default :
	      				std::cout << "Unexpected data received: " << line << "\n";
	      			}
	    		}else if (buffer[0] == 's'){

	    		}else if (buffer[0] == 'r'){

	    		}else if (buffer[0] == 'b'){

	    		}else if(buffer[0] == 'c'){

	    		}else if(buffer[0] == 'd'){

	    		}else{
	    			std::cout << "Unexpected message: " << line << "\n";
	    		}




	    		//Send value to the client;       
	    	}

	      h_write();

	      //process the data received with a switch case




	    }
   	}
public:
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
     : acceptor_(io, tcp::endpoint(tcp::v4(), 17000)){
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
int main()  {
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

	return 0;
}