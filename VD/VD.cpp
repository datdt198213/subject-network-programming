#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

int main() {

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);

	char buf[1024];

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	connect(client, (SOCKADDR*)&addr, sizeof(addr));

	fd_set fdread;
	int ret;
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	while (1)
	{
		FD_ZERO(&fdread);
		FD_SET(client, &fdread);
		ret = select(0, &fdread, 0, 0, 0);

		if (ret > 0)
		{
			ret = recv(client, buf, sizeof(buf), &tv);
			if (ret <= 0)
				break;

			buf[ret] = 0;
			printf("Du lieu nhan duoc %s", buf);
		}

		if (ret == 0)
		{
			printf("Timeout\n");
		}
	}
}