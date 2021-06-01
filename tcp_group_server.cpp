#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define DEFAULT_PORT "27015"
#define BUFFER_LENGTH 512
#define NUM_CLIENTS 3

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

	//hashtable for all clients
	unordered_map<int, SOCKET> clients;

	//listen until you have all the clients
	for (int i = 0; i < NUM_CLIENTS; i++) {
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
		else {
			clients[i] = client;
		}
	}

	while (1) {
		//recieve from each client and send to other clients
		for (auto& client : clients) {
			char permission[BUFFER_LENGTH] = "~";
			send(client.second, permission, BUFFER_LENGTH, 0);

			char recvBuffer[BUFFER_LENGTH] = "";
			string sendBuffer = "";

			//transmit data between client and server
			int bytesRecieved = 1;
			cout << "Pending recieve" << endl;
			bytesRecieved = recv(client.second, recvBuffer, BUFFER_LENGTH, 0);

			if (bytesRecieved > 0)
				cout << "recieved " << bytesRecieved << " bytes" << endl;
			else
				cout << "recieve failed" << endl;

			sendBuffer = recvBuffer;

			for (auto& recipient : clients) {
				//send the message to all other clients
				if (recipient != client) {
					//send to client
					int bytesSent = send(recipient.second, sendBuffer.c_str(), sendBuffer.size(), 0);
					if (bytesSent == SOCKET_ERROR)
						cout << "send failed" << endl;
					else {
						cout << "sent " << bytesSent << " bytes" << endl;
					}
				}
			}
		}
	}
	for (auto& client : clients) {
		closesocket(client.second);
	}
	freeaddrinfo(result);
	closesocket(listenSocket);
	WSACleanup();

	return 0;
}