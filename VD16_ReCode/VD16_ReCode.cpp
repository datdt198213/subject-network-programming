#include<stdio.h>
#include<WinSock2.h>	

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")

DWORD WINAPI HandleThread(LPVOID lParam)
{
	SOCKET client = *(SOCKET*)lParam;

	return 0;
}

int main()
{
	// tao phien ban
	WSADATA wsa;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
	// check loi khi khoi tao phien ban

	// tao socket lang nghe tu server
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// tao dia chi
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8000);
	// gan dia chi cho socket lang nghe
	bind(listener, (sockaddr*)&addr, sizeof(addr));

	// chuyen server sang trang thai cho
	listen(listener, 5);

	OVERLAPPED overlapped;
	WSAEVENT newEvent = WSACreateEvent();
	overlapped.hEvent = newEvent;

	char buf[256];
	WSABUF dataBuf;
	dataBuf.len = sizeof(buf);
	dataBuf.buf = buf;

	DWORD bytesReceived, flags = 0;

	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		ret = WSARecv(client, &dataBuf, 1, &bytesReceived, &flags, &overlapped, NULL);

		if (ret == SOCKET_ERROR)
		{
			ret = WSAGetLastError();
			printf("Error code: %d\n", ret);
			if (ret == WSA_IO_PENDING)
			{
				printf("Dang cho du lieu....\n");
			}
		}

		ret = WSAWaitForMultipleEvents(1, &newEvent, FALSE, WSA_INFINITE, FALSE);

		if (ret == WSA_WAIT_FAILED)
		{
			break;
		}

		WSAResetEvent(newEvent);
		WSAGetOverlappedResult(client, &overlapped, &bytesReceived, false, &flags);

		if (bytesReceived == 0)
			break;

		buf[bytesReceived] = 0;
		printf("Du lieu nhan duoc: %s\n", buf);
	}

	// vong lap chap nhan nhieu ket noi
	// vong lap nhan nhieu du lieu tu client

}