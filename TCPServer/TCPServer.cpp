#include <stdio.h>
#include <WinSock2.h>

//#pragma comment(lib, "ws2_32")
//#pragma warning(disable:4996)

int main(int argc, char* argv[]) {

	unsigned short target_port = 0;

	for (int i = 0; argv[1][i]; i++) {
		target_port = target_port * 10 + argv[1][i] - '0';
	}

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(target_port);

	char buf[1024];

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(listener, (sockaddr*)&addr, sizeof(addr));

	listen(listener, sizeof(buf) - 1);

	SOCKET client = accept(listener, NULL, NULL);

	FILE* fhello = fopen(argv[2], "rb");

	while (!feof(fhello)) {
		int ret = fread(buf, 1, sizeof(buf) - 1, fhello);
		if (ret > 0) {
			buf[ret] = 0;
			send(client, buf, ret, 0);
		}
	}

	fclose(fhello);

	FILE* fclient_content = fopen(argv[3], "wb");

	while (1) {
		int ret = recv(client, buf, sizeof(buf) - 1, 0);

		if (ret <= 0) {
			printf("Close connection!");
			break;
		}

		buf[ret] = 0;

		fwrite(buf, 1, ret, fclient_content);
	}

	fclose(fclient_content);

	closesocket(listener);
	WSACleanup();

}