#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

SOCKET clients[64];
int numberClient = 0;


DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET*)lpParam;
	int ret;
	char buf[256];
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
		{
			break;
		}
		buf[ret] = 0;
		printf("Data receive from client %d: %s\n",client, buf);

		char* cmd = strtok(buf, " ");

		char* sentences = buf + strlen(cmd) + 1;

		if (strcmp(cmd, "all") == 0)    //chat nhom, neu du lieu nhan duoc la all... thi chuyen sang chat nhom 
		{
			// Gui cho cac client khac
			for (int i = 0; i < numberClient; i++)
				if (clients[i] != client) // tranh viec gui lai cho chinh client dang dui len
				{
					char* msg = buf + strlen(cmd) + 1; // message gui di bat dau tu ki tu tiep theo sau cmd
					send(clients[i], msg, strlen(msg), 0);
				}
		}
		else // Chat ca nhan
		{
			int id = atoi(cmd); // chuyen chuoi cmd sang so
			// Gui cho client id
			for (int i = 0; i < numberClient; i++)
				if (clients[i] == id)
				{
					char* msg = buf + strlen(cmd) + 1;
					send(clients[i], msg, strlen(msg), 0);
				}
		}
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
		clients[numberClient++] = client;
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
}