#include <stdio.h>
#include <WinSock2.h>

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")

int main()
{
	// khoi tao phien ban
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// khoi tao dai dia chi
	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);

	// khoi tao socket de nghe client ket noi den
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// gan socket hien tai voi dai dia chi
	bind(listener, (sockaddr*)&addr, sizeof(addr));

	// chuyen server sang trang thai cho ket noi
	listen(listener, 5);

	WSAEVENT newEvent = WSACreateEvent();
	while (1)
	{

	}
}

