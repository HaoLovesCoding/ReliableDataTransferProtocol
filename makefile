default:client.cpp Packet.h Packet.cpp RcvBuffer.cpp RcvBuffer.h TCPClient.cpp TCPClient.h 
	g++ -std=c++0x -pthread client.cpp Packet.cpp RcvBuffer.cpp TCPClient.cpp -o client
	g++ -std=c++0x -pthread Packet.cpp main.cpp SRServer.cpp SenderWindow.cpp FileReader.cpp MapQueue.h -o server

client:
	g++ -std=c++0x -pthread client.cpp Packet.cpp RcvBuffer.cpp TCPClient.cpp -o client

server:
	g++ -std=c++0x -pthread Packet.cpp main.cpp SRServer.cpp SenderWindow.cpp FileReader.cpp MapQueue.h -o server
clean:
	rm client server received.data

