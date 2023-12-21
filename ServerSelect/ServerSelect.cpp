#include<stdio.h>
#include<WinSock2.h>

int main()
{
	// Khoi tao phien ban
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// Khai bao dia chi ket noi
	SOCKADDR_IN addrIn;
	addrIn.sin_family = AF_INET;
	addrIn.sin_addr = htonl(INADDR_ANY);
	addrIn.sin_port = htons(8000);

	// Khai bao socket
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (SOCKADDR*)&addrIn, sizeof(addrIn));
	listen(listener, 5);

	fd_set fread;
	int ret;

	SOCKET clients[64];
	int numClients = 0;

}