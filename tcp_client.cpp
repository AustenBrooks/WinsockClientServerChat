#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define BUFFER_LENGTH 512

using namespace std;

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

	//create a socket to connect to the server
	ptr = result;

	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (clientSocket == INVALID_SOCKET) {
		cout << "socket failed" << endl;
		freeaddrinfo(result);
		WSACleanup();
		return -1;
	}

	//connect socket to the server
	if (connect(clientSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
		cout << "server connection failed" << endl;
		freeaddrinfo(result);
		closesocket(clientSocket);
		WSACleanup();
		return -1;
	}

	string sendBuffer = "Test send";
	

	//transmit data between client and server

	int bytesRecieved = 1;
	bool needsInput = true;
	while (bytesRecieved > 0) {

		if (needsInput) {
			cout << "waiting on user input" << endl;
			getline(cin, sendBuffer);

			//send text to server
			int bytesSent = send(clientSocket, sendBuffer.c_str(), sendBuffer.size(), 0);
			if (bytesSent == SOCKET_ERROR) 
				cout << "send failed" << endl;
			else {
				cout << "sent " << bytesSent << " bytes" << endl;
				needsInput = false;
			}
		}

		//recieve server data
		cout << "Pending recieve" << endl;
		char recvBuffer[BUFFER_LENGTH] = "";
		bytesRecieved = recv(clientSocket, recvBuffer, BUFFER_LENGTH, 0);

		if (bytesRecieved > 0) {
			cout << "recieved " << bytesRecieved << " bytes" << endl;
			needsInput = true;
		}
		else {
			cout << "recieve failed" << endl;
		}

		cout << "Server: " << recvBuffer << endl;
	}

	freeaddrinfo(result);
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}
