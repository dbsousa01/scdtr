server:
	g++ -Wall -Wextra -ansi -pedantic server.cpp -std=c++11 -pthread -lboost_system -o server
comm:
	g++ -Wall -Wextra -ansi -pedantic serial_comm.cpp -std=c++11 -pthread -lboost_system -o serial_comm
clean:
	-rm -f *.o server serial_comm
