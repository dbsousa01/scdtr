server:
	g++ -std=c++11 server.cpp arduino.cpp pig2i2c.cpp circular_buffer.cpp -pthread -lboost_thread -lboost_system -o server
comm:
	g++ -Wall -Wextra -ansi -pedantic serial_comm.cpp -std=c++11 -pthread -lboost_system -o serial_comm
circular_buffer:
	g++ -Wall -Wextra -ansi -pedantic circular_buffer.cpp -std=c++11 -o circular_buffer
pig2i2c:
	g++ -Wall -Wextra -ansi -pedantic pig2i2c.cpp -std=c++11 -o pig2i2c
arduino:
	g++ -Wall -Wextra -ansi -pedantic arduino.cpp circular_buffer.cpp -std=c++11 -pthread -lboost_system -o arduino
main:
	g++ -Wall -Wextra -ansi -pedantic server.cpp circular_buffer.cpp -std=c++11 -pthread -lboost_system -o main
clean:
	-rm -f *.o server serial_comm circular_buffer pig2i2c arduino main
