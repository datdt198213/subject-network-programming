#include<stdio.h>	
#include<WinSock2.h>

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")

char* clientId[64];
SOCKET clientLogin[64];
int numLogin = 0, numId = 0;

// khoi tao ham ThreadNew de tao mot luong moi
DWORD WINAPI ThreadNew(LPVOID param);
int main()
{
	// Khoi tao phien ban
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// khoi tao dia chi
	sockaddr_in addrIn;
	addrIn.sin_family = AF_INET;
	addrIn.sin_addr.s_addr = htonl(INADDR_ANY);
	addrIn.sin_port = htons(8000);

	// tao socket listener
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// Gan dia chi addr vao trong socket listener
	bind(listener, (sockaddr*)&addrIn, sizeof(addrIn));

	// cho server trong che do cho
	listen(listener, 5);

	// thuc hien chap nhan ket noi tu cac client
	while (1)
	{
		// tao socket client chap nhan ket noi den listener
		SOCKET client = accept(listener, NULL, NULL);
		// Hien thi ra cac client da ket noi den server
		printf("Client da ket noi: %d\n", client);
		// tao luong moi de xu ly du lieu khi client gui den
		CreateThread(0, 0, ThreadNew, &client, 0, 0);
	}
}

DWORD WINAPI ThreadNew(LPVOID param)
{
	// ep kieu LPVOID -> SOCKET
	SOCKET client = *(SOCKET*)param;
	
	char buf[256], header[256], content[256], tmp[256];
	const char* headerPattern = "client_id:";
	int ret;

	// Kiem tra dang nhap tu client
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

		// bo di ki tu xuong dong khi client gui len
		buf[ret - 1] = 0;
		printf("\nDu lieu nhan duoc tu client %d: %s\n", client, buf);
		// kiem tra tinh trang dang nhap
		// cat chuoi thanh cac chuoi nho hon duoc chia boi dau cach "client_id: xxxxxx"
		ret = sscanf(buf, "%s %s %s", header, content, tmp); // ki tu tmp duoc hieu la ki tu dung de lay ra cac ki tu sau xxxxxx

		// neu chuoi nhan vao khong phai la 2 ki tu thi yeu cau nhap lai
		if (ret != 2)
		{
			const char* msg = "\nDang nhap that bai! Hay dang nhap theo dinh dang \"client_id: xxxxxx\" voi xxxxxx la ten cua ban\n";
			send(client, msg, strlen(msg), 0);
		}
		else
		{
			// chuoi nhan duoc la 2 ki tu
			// dung dinh dang theo "client_id:"
			// dang nhap thanh cong thi khong kien tra nua: thoat vong lap
			if (strcmp(header, headerPattern) == 0)
			{
				const char* msg = "Dang nhap thanh cong, hay gui mot tin nhan gi do!\n";
				send(client, msg, strlen(msg), 0);

				// them client hien tai vao trong clientLogin
				clientLogin[numLogin++] = client;
				// them id hien tai vao trong danh sach id
				clientId[numId++] = content;

				printf("NumId: %d, NumLogin: %d\n", numLogin, numId);
				break;
			} 
			else
			{
				const char* msg = "\nDang nhap that bai! Hay dang nhap theo dinh dang \"client_id: xxxxxx\" voi xxxxxx la ten cua ban\n";
				send(client, msg, strlen(msg), 0);
			}
		}
	}

	// xu ly du lieu duoc client gui den
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;
		// 
		buf[ret - 1] = 0;
		printf("\nDu lieu nhan duoc tu client %d: %s", client, buf);
	
		// noi chuoi hien tai voi clientId
		int idx = 0;
		for (; idx < numLogin; idx++)
			if (clientLogin[idx] == client)
				break;

		char sendBuf[256];
		sprintf(sendBuf, "%s: %s", clientId[idx], buf);
		// neu du lieu nhan duoc hop le thi gui cho tat ca cac server da login vao he thong
		
		for (int i = 0; i < numLogin; i++)
			if (clientLogin[i] != client)
				send(clientLogin[i], sendBuf, strlen(sendBuf), 0);
	}
	closesocket(client);
}
