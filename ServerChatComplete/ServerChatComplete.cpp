#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

SOCKET clients[64];
int numberClient = 0;

void RemoveClient(SOCKET client)
{
	int idx = 0;
	// tim vi tri client trong mang
	for (; idx < numberClient; idx++)
	{
		if (clients[idx] = client) break;
	}
	if (idx < numberClient - 1) // Neu khong phai phan tu cuoi
	{
		clients[idx] = clients[numberClient];
		numberClient--;
	}
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET*)lpParam;
	int ret;
	char buf[256], cmd[20], id[20];
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
		{
			RemoveClient(client);
			return 0;
		}
		buf[ret] = 0;
		printf("Data receive from client %d: %s\n", client, buf);

			
	
	}
	closesocket(client);
}

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr_in.sin_port = htons(8000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (sockaddr*)&sockaddr_in, sizeof(sockaddr_in));
	listen(listener, 2);

	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		printf("New client connection: %d\n", client);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
}