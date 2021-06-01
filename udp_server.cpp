#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define BUFFER_LENGTH 512
#define PORT 27015


int main(){
	WSADATA data;
	int startup = WSAStartup(MAKEWORD(2, 2), &data);

	if (startup != 0) {
		cout << "Initialization failed" << endl;
		return -1;
	}

	struct sockaddr_in client;
	int slen = sizeof(client);

	//create a socket
	SOCKET serverSocket = INVALID_SOCKET;
	if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
		cout << "socket failed" << endl;
		WSACleanup();
		return -1;
	}

	//set up the sockaddr_in for server
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);

	//bind the socket
	if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR){
		cout << "socket bind failed" << endl;
		closesocket(serverSocket);
		WSACleanup();
		return -1;
	}

	//transmit data between client and server
	while (1){
		string sendBuffer = "";
		char recvBuffer[BUFFER_LENGTH] = "";
		cout << "Pending recieve" << endl;
		fflush(stdout);

		//recieve data from client
		bool needsInput = false;
		int bytesRecieved = recvfrom(serverSocket, recvBuffer, BUFFER_LENGTH, 0, (struct sockaddr*) &client, &slen);
		if (bytesRecieved > 0) {
			cout << "recieved " << bytesRecieved << " bytes" << endl;
			needsInput = true;
		}
		else
			cout << "recieve failed" << endl;

		cout << "Client: " << recvBuffer << endl;
		//print details of the client/peer and the data received
		//printf("Received packet from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		//printf("Data: %s\n", recvBuffer);

		if (needsInput) {
			cout << "waiting on user input" << endl;
			getline(cin, sendBuffer);

			//send to client
			int bytesSent = sendto(serverSocket, sendBuffer.c_str(), sendBuffer.size(), 0, (struct sockaddr*) &client, slen);
			if (bytesSent == SOCKET_ERROR)
				cout << "send failed" << endl;
			else {
				cout << "sent " << bytesSent << " bytes" << endl;
				needsInput = false;
			}
		}
	}

	closesocket(serverSocket);
	WSACleanup();

	return 0;
}