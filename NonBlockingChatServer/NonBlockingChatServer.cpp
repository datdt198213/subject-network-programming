#include<stdio.h>
#include<WinSock2.h>

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")

int main()
{
	// khoi tao phien ban
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// khoi tao dai dia chi
	SOCKADDR_IN sockaddr_in;
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
	sockaddr_in.sin_port = htons(8000);

	// khoi tao socket nghe client ket noi den
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// gan socket nghe do voi dai dia chi
	bind(listener, (sockaddr*)&sockaddr_in, sizeof(sockaddr_in));

	// cho server vao trong trang thai cho
	listen(listener, 5);

	// chuyen server sang trang thai non-blocking
	unsigned long ul = 1;
	ioctlsocket(listener, FIONBIO, &ul);

	// khoi tao mang socket de luu danh sach client ket noi den
	SOCKET clients[64];
	int numClient = 0;

	// khoi tao cac bien trung gian
	int ret;
	char buf[256];

	// chap nhan nhan nhieu client den
	while (1)
	{
		// tao client ket noi den server
		SOCKET client = accept(listener, NULL, NULL);

		if (client == INVALID_SOCKET)
		{
			ret = WSAGetLastError();
			if (ret == WSAEWOULDBLOCK)
			{

			}
		}
		else
		{
			printf("\nClient da ket noi den: %d", client);
			// them client hien tai vao trong mang clients
			const char* msgCurrent = "\nBan da ket noi vao he thong thanh cong!!!";
			const char* msgOther = "\nClient moi ket noi vao trong he thong";
			char target[256];
			sprintf(target, "%s: %d", msgOther, client);

			send(client, msgCurrent, strlen(msgCurrent), 0);
			for (int i = 0; i < numClient; i++)
				send(clients[i], target, strlen(target), 0);

			clients[numClient++] = client;
		}

		// nhan du lieu tu cac client da ket noi den
		for (int i = 0; i < numClient; i++) 
		{
			ret = recv(clients[i], buf, sizeof(buf), 0);
			if (ret == SOCKET_ERROR)
			{
				ret = WSAGetLastError();
				if (ret == WSAEWOULDBLOCK)
				{

				}
			}
			else
			{
				buf[ret] = 0;
				printf("\nDu lieu tu client %d: %s", clients[i], buf);
				for(int j = 0 ; j < numClient; j++)
					if (clients[i] != clients[j])
						send(clients[j], buf, strlen(buf), 0);
			}
		}
	}
}