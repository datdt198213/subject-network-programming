#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)


void RemoveClient(SOCKET clients[],int* numClient,int idx)
{
	if (idx != *numClient - 1) // Neu khong phai phan tu cuoi
	{
		clients[idx] = clients[*numClient];
		*numClient--;
	}
}

int main() {

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKADDR_IN addr;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);

	SOCKET listener= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bind(listener, (SOCKADDR*)&addr, sizeof(addr));
	listen(listener, 5);

	fd_set fdread;
	int ret;

	char buf[256];
	SOCKET clients[64];
	int numClient = 0;
	while (1)
	{
		FD_ZERO(&fdread); // neu khong xoa thi ham select se tra ve gia tri ngay lap tuc
		for (int i = 0; i < numClient; i++)
			FD_SET(clients[i], &fdread);			// them tung client vao trong fdread
		ret = select(0, &fdread, 0, 0, 0);			// so doi tuong trong fdread

		if (ret > 0) // co su kien trong ket noi
		{
			if (FD_ISSET(listener, &fdread))
			{
				SOCKET client = accept(listener, NULL, NULL);
				printf("Ket noi moi: %d\n", client);

				const char* msg = "Hello client\n";
				send(client, msg, strlen(msg), 0);
		
				clients[numClient++] = client;
			}

			for (int i = 0; i < numClient; i++)
			{
				if (FD_ISSET(clients[i], &fdread))
				{
					ret = recv(clients[i], buf, sizeof(buf), 0);
					// client bi ngat ket noi thi xoa client ra khoi mang client hien tai
					if (ret <= 0) // client bi ngat ket noi
					{
						// Xoa client ra khoi mang
						RemoveClient(clients, &numClient, i);
						i--; // Tranh viec bo qua client hien tai khi da xoa 
						continue;
					}

					buf[ret] = 0;
					printf("Du lieu nhan duoc tu %d: %s", clients[i], buf);
				}
			}
		}


	}
}