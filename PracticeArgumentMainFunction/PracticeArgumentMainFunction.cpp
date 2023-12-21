#include<stdio.h>
#include<WinSock2.h>

int main(int argc, char* argv[]) 
{
	printf("argc = %d\n", argc);
	printf("let's see what is in argv[]\n");

	for (int i = 0; i < argc; i++) 
	{
		printf("argv[%d] = %s\n", i, argv[i]);
	}
	
	char* destination_ip = argv[1];
	unsigned short destination_port = 0;

	for (int i = 0; argv[2][i]; i++) 
	{
		destination_port = destination_port * 10 + argv[2][i] - '0';
	}

	printf("Ip: %s, Port: %u\n", destination_ip, destination_port);

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(destination_ip);
	addr.sin_port = htons(destination_port);

	int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));

	if(ret == SOCKET_ERROR) 
	{
		ret = WSAGetLastError();
		printf("\nConnection fail! - %d", ret);
		return 1;
	}

	char buf[256];

	ret = recv(client, buf, sizeof(buf) - 1 , 0);
	if(ret > 0) 
	{
		buf[ret] = 0;
		printf("\nServer: %s", buf);
	}

	while (1)
	{
		fgets(buf, sizeof(buf) - 1, stdin);

		if (strncmp(buf, "exit", 4) == 0)
		{
			printf("\nEnd. Closed connection!");
			break;
		}
		
		int length = sizeof(buf);
		buf[length] = 0;

		send(client, buf, sizeof(buf), 0);
	}

	closesocket(client);
	WSACleanup();
	return 0;
}