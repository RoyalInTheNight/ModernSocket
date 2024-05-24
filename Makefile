CPP=g++
STAND=-std=gnu++17

all:
	${CPP} source/logging/logging.cpp source/socket/unix_client.cpp source/socket/unix_socket_client.cpp source/socket/unix_socket_server.cpp socket.cpp -o server ${STAND}

clean:
	rm server