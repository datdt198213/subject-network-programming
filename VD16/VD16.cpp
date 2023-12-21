#include <stdio.h>
#include <WinSock2.h>

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")


DWORD WINAPI HandleThread(LPVOID lParam)
{
	SOCKET client = *(SOCKET	*)lParam;
	
	OVERLAPPED overlapped;
	WSAEVENT newEvent = WSACreateEvent();
	overlapped.hEvent = newEvent;

	WSABUF databuf;
	char buf[256];
	databuf.buf = buf;
	databuf.len = sizeof(buf);

	DWORD bytesReceive, flags = 0;
	int ret;

	while (1)
	{
		ret = WSARecv(client, &databuf, 1, &bytesReceive, &flags, &overlapped, NULL);

		if (ret == SOCKET_ERROR)
		{
			ret = WSAGetLastError();
			printf("Error code: %d\n", ret);
			if (ret == WSA_IO_PENDING)
			{
				printf("Dang cho du lieu: ...\n");
			}				
		}

		ret = WSAWaitForMultipleEvents(1, &newEvent, FALSE, WSA_INFINITE, FALSE);

		if (ret == WSA_WAIT_FAILED)
		{
			break;
		}

		WSAResetEvent(newEvent);
		WSAGetOverlappedResult(client, &overlapped, &bytesReceive, FALSE, &flags);

		if (bytesReceive == 0)
		{
			break;
		}

		buf[bytesReceive] = 0;
		printf("Du lieu nhan duoc: %s\n", buf);
	}
}
int main()
{
	// khoi tao phien ban
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// khoi tao dai dia chi
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8000);

	// khoi tao socket de nghe client ket noi den
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			
	// gan socket hien tai voi dai dia chi
	bind(listener, (sockaddr*)&addr, sizeof(addr));

	// chuyen server sang trang thai cho ket noi
	listen(listener, 5);

	while (1) {
		SOCKET client = accept(listener, NULL, NULL);
		printf("Client moi ket noi: %d\n", client);
		CreateThread(0, 0, HandleThread, &client, 0, 0);
	}
}