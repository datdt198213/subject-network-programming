#include <stdio.h>
#include <winsock.h>
#include <utility>

#pragma comment(lib, "ws2_32")
#pragma warning( disable : 4996)

int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	unsigned long ul = 1;
	ioctlsocket(listener, FIONBIO, &ul);	// chuyen socket sang che do non-blocking

	bind(listener, (sockaddr*)&addr, sizeof(addr));

	char buf[1024];
	std::pair<SOCKET, char*> clients[64];
	int numClients = 0;

	listen(listener, 5);

	while (1) {
		SOCKET client = accept(listener, NULL, NULL);
		if (client == INVALID_SOCKET) {
			int ret = WSAGetLastError();
			if (ret = WSAEWOULDBLOCK) {

			}
		}
		else {
			printf("Client moi ket noi: %d\n", client);
			clients[numClients].first = client;
			clients[numClients].second = NULL;
			numClients++;
			send(client, "Nhap ten (client_id: xxxx):\n", 28, 0);
		}

		for (int i = 0; i < numClients; i++) {
			int ret = recv(clients[i].first, buf, sizeof(buf) - 1, 0);

			if (ret == SOCKET_ERROR) {
				ret = WSAGetLastError();
				if (ret != WSAEWOULDBLOCK) {
					printf("Error with %d, error code: %d | Close connection\n", clients[i].first, ret);
					closesocket(clients[i].first);
					numClients--;
					for (int j = i; j < numClients; j++) clients[j] = clients[j + 1];
				}
			}
			else {

				buf[ret] = 0;

				if (clients[i].second == NULL) {
					if (strncmp(buf, "client_id: ", 11) == 0) {
						char* name = new char[100];
						memcpy(name, buf + 11, strlen(buf) - 12);
						name[strlen(buf) - 12] = 0;
						clients[i].second = name;
					}
				}
				else {
					char mess[1000];
					strcpy(mess, clients[i].second);
					strcat(mess, ": ");
					strcat(mess, buf);

					for (int j = 0; j < numClients; j++)
						if (j != i) send(clients[j].first, mess, strlen(mess), 0);
				}
			}
		}
	}

	closesocket(listener);
	WSACleanup();
}