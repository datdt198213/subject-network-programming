#include<stdio.h>
#include<WinSock2.h>

int main(int argc, char* args[])
{
	printf("\nNumber of argument: %d", argc);
	printf("\nElements in args[]");

	for (int i = 0; args[i]; i++)
	{
		printf("\narg[%d] = %s", i, args[i]);
	}

	unsigned short des_port = 0;

	for (int i = 0;args[1][i];i++) 
	{
		des_port = des_port * 10 + args[1][i] - '0';
	}

	printf("\nDestination port: %u", des_port);

	// Tạo phiên bản
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// Tạo địa chỉ ip
	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr_in.sin_port = htons(des_port);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(listener, (sockaddr*)&sockaddr_in, sizeof(sockaddr_in));
	listen(listener, 5);

	char buf[1024];

	SOCKET client = accept(listener, NULL, NULL);

	FILE* f_hello = fopen(args[2], "rb");

	while (!feof(f_hello))
	{
		int ret = fread(buf, 1, sizeof(buf) - 1, f_hello);
		if (ret > 0)
		{
			buf[ret] = 0;
			send(client, buf, sizeof(buf), 0);
		}
	}

	fclose(f_hello);

	FILE* f_client_content = fopen(args[3], "wb");

	while (1)
	{
		int ret = recv(client, buf, sizeof(buf) - 1, 0);

		if (ret < 0) 
		{
			printf("Closed connection!");
			break;
		}

		buf[ret] = 0;
		fwrite(buf, 1, ret, f_client_content);
	}

	fclose(f_client_content);
	closesocket(client);
	WSACleanup();
}