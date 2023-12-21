#include<stdio.h>
#include<WinSock2.h>

#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")

int main()
{
	// tao phien ban
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	// tao mien dia chi
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(8000);
	
	// tao socket de nghe ket noi tu client
	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// gan dia chi vao voi socket
	bind(listener, (sockaddr*)&addr, sizeof(addr));

	// cho server trong trang thai cho
	listen(listener, 5);

	// chuyen server sang trang thai non-blocking
	unsigned long ul = 1;
	ioctlsocket(listener, FIONBIO, &ul);

	// tao mang client de luu danh sach client ket noi den
	SOCKET clients[64];
	int numClient = 0;

	// khoi tao bien trung gian nhan cac gia tri trong SOCKET
	int ret;
	char buf[256];

	// chap nhan nhieu ket noi den server
	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		if (client == INVALID_SOCKET)
		{
			// lay ra ma loi
			ret = WSAGetLastError(); 
			if (ret == WSAEWOULDBLOCK)
			{
				// server dang trong trang thai thuc hien cac hanh dong nhu
				// ket noi, nhan du lieu, ....
				// server se luon roi vao trang thai nay ke cac khi co 
				// ket noi den hay khong co ket noi den 
			}
		}
		else
		{
			// khi client hien tai khong trong trang thai loi socket
			printf("\nClient da ket noi: %d", client);

			// them client hien tai vao mang client
			clients[numClient++] = client;
		}

		// khi client van con trong ket noi voi server
		for (int i = 0; i < numClient; i++)
		{
			// nhan du lieu tu cac client hien tai dang ket noi den server
			ret = recv(clients[i], buf, sizeof(buf), 0);
		
			if (ret == SOCKET_ERROR)
			{
				ret = WSAGetLastError();
				if (ret = WSAEWOULDBLOCK)
				{

				}
			} 
			else
			{
				buf[ret - 1] = 0;
				printf("\nClient %d send: %s",clients[i], buf);
			}
		}
	}

}