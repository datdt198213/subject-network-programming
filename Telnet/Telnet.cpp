#include<stdio.h>
#include<WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

DWORD WINAPI ThreadNew(LPVOID param)
{
	// ep kieu param ve kieu socket
	SOCKET client = *(SOCKET*)param;
	char buf[256], user[256], pass[256], tmp[256];
	int ret;

	while (1)
	{
		// nhan du lieu tu client gui
		ret = recv(client, buf, strlen(buf), 0);

		if (ret <= 0)
			return 0;

		buf[ret - 1] = 0;
		printf("\nDu lieu nhan duoc: %s", buf);
		ret = sscanf(buf, "%s %s %s", user, pass, tmp);
		
		if (ret != 2)
		{
			const char* message = "Sai cu phap. Hay nhap lai\n";
			send(client, message, strlen(message), 0);
		} 
		else
		{
			FILE* f = fopen("C:\\test\\data.txt", "r");
			// bien found = 1 khi user va pass dung trong database
			int found = 0;

			while (!feof(f))
			{
				char line[256], userf[256], passf[256];
				// doc du lieu theo dong trong file			
				fgets(line, strlen(line), f);
				// tach chuoi line thanh userf va passf
				sscanf(line, "%s %s", userf, passf);

				if (strcmp(user, userf) == 0 && strcmp(pass, passf) == 0)
				{
					found = 1;
					break;
				}
			}
			if (found == 1)
			{
				const char* message = "Dang nhap thanh cong! vui long nhap lenh\n";
				send(client, message, strlen(message), 0);
				break;
			}
			else
			{
				const char* message = "Dang nhap that bai! vui long thu lai\n";
				send(client, message, strlen(message), 0);
			}
		}
	}

	// xu ly du lieu sau dang nhap
	while (1)
	{
		// nhan du lieu tu client
		ret = recv(client, buf, strlen(buf), 0);
		
		if (ret <= 0)
		{
			return 0;
		}
		buf[ret - 1] = 0; // do khi client gui se kem theo ki tu 0
		printf("Du lieu nhan duoc: %s", buf);

		char cmdbuf[256];
		// ghi du lieu vao trong file
		sprintf(cmdbuf, "%s > C:\\test\\out.txt", buf);
		system(cmdbuf);

		// doc file out.txt
		FILE* f = fopen("C:\\test\\out.txt", "r");
		while (!feof(f))
		{
			char line[256];
			fgets(line, sizeof(line), f);
			send(client, line, strlen(line), 0);
		}
		fclose(f);
	}
	closesocket(client);
}

int main()
{
	// tao phien ban
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// tao dia chi
	SOCKADDR_IN addrIn;
	addrIn.sin_family = AF_INET;
	addrIn.sin_addr.s_addr = htonl(INADDR_ANY); // chuyen dau nho thanh dau to
	addrIn.sin_port = htons(8000); // chuyen dau to thanh dau nho

	// tao socket
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// gan client vao trong addrIn
	bind(listener, (SOCKADDR*)&addrIn, sizeof(addrIn));

	// chuyen sang trang thai cho
	listen(listener, 5);

	
	while (1)
	{
		// nhan ket noi tu client
		SOCKET client = accept(listener, NULL, NULL);
		printf("\nClient moi ket noi: %d", client);
		CreateThread(0, 0, ThreadNew, &client, 0, 0);
	}
}