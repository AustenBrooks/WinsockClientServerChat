#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_LENGTH 512
#define PORT 27015
#define SERVER "11.187.119.110"

using namespace std;

int main(void){
	struct sockaddr_in client;
	int slen = sizeof(client);
	WSADATA data;

	int startup = WSAStartup(MAKEWORD(2, 2), &data);

	if (startup != 0) {
		cout << "Initialization failed" << endl;
		return -1;
	}

	//create a socket to connect to the server
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (clientSocket == INVALID_SOCKET) {
		cout << "socket failed" << endl;
		WSACleanup();
		return -1;
	}

	//setup address structure
	memset((char*) &client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(PORT);
	client.sin_addr.S_un.S_addr = inet_addr(SERVER);

	//start communication
	while (1){
		string sendBuffer = "";
		bool needsInput = true;

		if (needsInput) {
			cout << "waiting on user input" << endl;
			getline(cin, sendBuffer);

			//send text to server
			int bytesSent = sendto(clientSocket, sendBuffer.c_str(), sendBuffer.size(), 0, (struct sockaddr*) &client, slen);
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
		int bytesRecieved = recvfrom(clientSocket, recvBuffer, BUFFER_LENGTH, 0, (struct sockaddr*) &client, &slen);

		if (bytesRecieved > 0) {
			cout << "recieved " << bytesRecieved << " bytes" << endl;
			needsInput = true;
		}
		else {
			cout << "recieve failed" << endl;
		}

		cout << "Server: " << recvBuffer << endl;
	}
	closesocket(clientSocket);
	WSACleanup();

	return 0;
}