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
	char user[256], pass[20], tmp[20];
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
		{
			break;
		}
		buf[ret] = 0;
		printf("Data receive from client %d: %s\n", client, buf);
		

		ret = sscanf(tmp, "%s %s", user, pass);
		
		if (ret != 2)
		{

		}
		else
		{
			FILE* f = fopen("C:\\des", "r");
			while (!feof(f))
			{
				char line[256];
				char userf[32], passf[32];
				sscanf(line, "%s %s", userf, passf);

			}
		}
	}

	while (1)
	{

		buf[ret - 1] = 0;

		char cmdbuf[256];
		sprintf(cmdbuf, "%s > c:\\test\\out.txt", buf);

		system(cmdbuf);
		FILE* f = fopen("c:\\test\\out.txt", "r");

		while (!feof(f))
		{
			char line[256];

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
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
}