#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_PORT "27015"
#define BUFFER_LENGTH 512

int main() {
	WSADATA data;

	int startup = WSAStartup(MAKEWORD(2, 2), &data);

	if (startup != 0) {
		cout << "Initialization failed" << endl;
		return -1;
	}


	struct addrinfo* result = NULL, * ptr = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the local address and port to be used by the server
	if (getaddrinfo(NULL, DEFAULT_PORT, &hints, &result) != 0) {
		cout << "getaddrinfo failed" << endl;
		WSACleanup();
		return -1;
	}

	//create a socket to listen
	SOCKET listenSocket = INVALID_SOCKET;
	listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		cout << "socket failed" << endl;
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}

	//Bind socket the server
	if (bind(listenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
		cout << "socket bind failed" << endl;
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}
	while (1) {
		cout << "listening for clients" << endl;
		
		//listen for client sockets
		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
			cout << "listening failed" << endl;
			closesocket(listenSocket);
			WSACleanup();
			return -1;
		}

		//accept the client socket
		SOCKET client = INVALID_SOCKET;
		client = accept(listenSocket, NULL, NULL);
		if (client == INVALID_SOCKET) {
			cout << "accept failed" << endl;
			closesocket(listenSocket);
			WSACleanup();
			return -1;
		}

		char recvBuffer[BUFFER_LENGTH] = "";
		string sendBuffer = "";

		//transmit data between client and server
		int bytesRecieved = 1;
		bool needsInput = false;
		while (bytesRecieved > 0) {
			cout << "Pending recieve" << endl;
			bytesRecieved = recv(client, recvBuffer, BUFFER_LENGTH, 0);

			if (bytesRecieved > 0) {
				cout << "recieved " << bytesRecieved << " bytes" << endl;
				needsInput = true;
			}
			else
				cout << "recieve failed" << endl;
			
			cout << "Client: " << recvBuffer << endl;

			if (needsInput) {
				cout << "waiting on user input" << endl;
				getline(cin, sendBuffer);

				//send to client
				int bytesSent = send(client, sendBuffer.c_str(), sendBuffer.size(), 0);
				if (bytesSent == SOCKET_ERROR)
					cout << "send failed" << endl;
				else {
					cout << "sent " << bytesSent << " bytes" << endl;
					needsInput = false;
				}
			}
		}
		closesocket(client);
	}
	freeaddrinfo(result);
	closesocket(listenSocket);
	WSACleanup();

	return 0;
}