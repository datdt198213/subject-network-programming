#include<stdio.h>
#include<string.h>
#include<WinSock2.h>

// An di loi tu visual studio
#pragma warning (disable:4996)	
#pragma comment(lib, "ws2_32")

// khoi tao mang de chua cac client da ket noi den
SOCKET clients[64];
int numClient = 0;

// khoi tao mang de chua cac client da dang nhap thanh cong
SOCKET clientsLogin[64];
int numClientLogin = 0;

// khoi tao mang chua cac gia tri id cua client trong database
char clientsId[64][64];
int numClientId = 0;

// khoi tao mang chua gia tri cua client duoc gui vao he thong
char clientsIdLogin[64][64];
int numClientIdLogin = 0;

DWORD WINAPI HandleThread(LPVOID param);
void Disconnect(SOCKET client);
int FindIndexId(char strList[][64], int size, char* id);
int FindIndexSocket(SOCKET clientsSocket[], SOCKET client);
int ExistDuplicateId(char * id);
void AddLoginId(char* id);
void AddLogin(SOCKET client);
char* Menu();
void DisplayAllClient(SOCKET client_list[],int size, SOCKET client);
void SendAll(SOCKET socket, char* id_source, char* content);
void SendOne(SOCKET socket, char* id_source, char* content, char* id_dest);
void Logout(SOCKET clientList[], char clientListId[][64], int* sizeClientList, int* sizeIdList, SOCKET client, char* id);
void RemoveFromListClient(SOCKET clientList[], int* size, SOCKET client);
void RemoveFromListId(char strList[][64], int* size, char* id);

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

	// chap nhan nhieu ket noi
	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		printf("Client moi ket noi den: %d\n", client);

		// gui thong bao den client da ket noi thanh cong
		const char* msg = "Chuc mung ban! Ban da ket noi thanh cong den server\nHay thuc hien dang nhap theo cu phap \'client_id: xxxxxx\'\n";
		send(client, msg, strlen(msg), 0);

		//them client hien tai vao danh sach cac client da ket noi den server
		clients[numClient++] = client;
		// Cac luong trong server
		// Moi client thuoc mot luong
		CreateThread(0, 0, HandleThread, &client, 0, 0);
	}
}

DWORD WINAPI HandleThread(LPVOID param)
{
	// ep kieu LPVOID ve SOCKET
	SOCKET client = *(SOCKET*)param;

	// khoi tao cac bien trung gian de chua du lieu nhan duoc tu client
	int ret;
	char buf[256];
	char choose[50];
	char idSource[50];

	// Neu client ket noi den khong ton tai
	if (client == INVALID_SOCKET)
	{
		int ret = WSAGetLastError();
		printf("Client khong ton tai\n");
		if (ret == WSAEWOULDBLOCK)
		{
			printf("Ma loi = %d\n", ret);
		}
		return 0;
	}

	// vong lap kiem tra dang nhap tu nhieu client
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);

		// Neu client ket noi den bi gian doan do duong truyen hoac do client thoat khoi he thong
		if (ret == SOCKET_ERROR)
		{
			ret = WSAGetLastError();
			Disconnect(client);
			printf("Ket noi bi gian doan - %d\n", ret);
			return 0;
		}

		buf[ret - 1] = 0;
		printf("Du lieu nhan duoc tu client %d: %s\n", client, buf);

		// kiem tra dieu kien dang nhap
		// tach chuoi thanh chuoi theo dang s1 = "client_id:" va s2 = "ma_client", dung bien tmp de biet xem co them mot chuoi nao sau chuoi "ma_client" khong
		char head[50], id[50], tmp[50];
		ret = sscanf(buf, "%s %s %s", head, id, tmp);
		
		/* test cac du lieu duoc tach ra tu trong buf
		printf("\nHead: %s", head);
		printf("\nId: %s", id);
		printf("\nTmp: %s", tmp);*/

		// luu tru lai id de con gui du lieu cho cac client khac
		strcpy(idSource, id);

		if (ret != 2)
		{
			const char* msg = "He thong: Khong dung dinh dang, hay nhap lai theo cu phap\'client_id: ma_client\'!\n";
			send(client, msg, strlen(msg), 0);
		}
		else
		{
			// khoi tao header mac dinh cua chuong trinh
			const char* headerDefault = "client_id:";

			// doc file de lay ra cac id
			FILE* f = fopen("data.txt", "r");
			while (!feof(f))
			{
				char line[256];
				fgets(line, sizeof(line), f);
				
				char *p;
				//Tách chuỗi con lần đầu tiên
				p = strtok(line, ", ");
							
				strcpy(clientsId[numClientId++], p);
				
				//Tách chuỗi con từ lần thứ 2 trở đi
				//Bằng cách sử dụng hàm strtok cho tới khi kết quả NULL được trả về.
				while (p != NULL)
				{
					//Chỉ dịnh đối số NULL trong hàm strtok để tiếp tục tách chuỗi ban đầu
					p = strtok(NULL, ", ");
					
					if (p != NULL)
					{
						strcpy(clientsId[numClientId++], p);
					}
				}
			}

			// so sanh phan header nhap vao co dung hay khong
			if (strcmp(head, headerDefault) != 0)
			{
				const char* msg = "Khong dung dinh dang, hay nhap lai theo cu phap\'client_id: ma_client\'!\n";
				send(client, msg, strlen(msg), 0);
			}
			else
			{
				// dung dinh dang client_id: ma_client tuy nhien can so sanh voi cac ki tu trong file
				int found = 0, duplicatedId = ExistDuplicateId(id);
				// kiem tra xem co id ma nguoi dung nhap vao co giong voi id trong database hay khong
				// kiem tra xem id da duoc dang nhap vao trong he thong hay chua
				if (duplicatedId == 1)
				{
					const char* msg = "He thong: Dang nhap that bai! Do co nguoi dung dang su dung id ma ban vua nhap\n";
					send(client, msg, strlen(msg), 0);
				} 
				else
				{
					for (int i = 0; i < numClientId; i++)
					{
						if (strcmp(clientsId[i], id) == 0)
						{
							found = 1;
							break;
						}
					}
				}
				
				// dang nhap thanh cong thoat ra khoi vong lap
				if (found == 1)
				{
					// Gui thong bao dang nhap thanh cong vao he thong va gui menu cho nguoi dung chon cac chuc nang muon thao tac
					const char* msg = "He thong: Dang nhap thanh cong vao he thong!\n";
					char msgMenu[1000], * tmpMenu = Menu();
					sprintf(msgMenu, "%s%s", msg, tmpMenu);
					send(client, msgMenu, strlen(msgMenu), 0);

					// Gui thong bao cho cac client khac da dang nhap trong he thong
					char msgOther[256];
					sprintf(msgOther, "%s%s%s", "\nHe thong: Client voi id ", id, " da dang nhap vao he thong\n");
					for (int i = 0; i < numClientLogin; i++)
						send(clientsLogin[i], msgOther, strlen(msgOther), 0);
					
					// them id hien tai vao danh sach id
					AddLoginId(id);

					// them client hien tai vao trong danh sach client login
					AddLogin(client);

					// thoat ra khoi yeu cau dang nhap cua client hien tai
					break;
				}
				else
				{
					const char* msg= "He thong: Hay thu lai voi id khac!\n";
					send(client, msg, strlen(msg), 0);
				}
			}
		}
	}

	// vong lap nhan lien tuc du lieu tu client da ket noi thanh cong
	while(1)
	{
		
		ret = recv(client, choose, sizeof(choose), 0);

		if (ret == SOCKET_ERROR)
		{
			ret = WSAGetLastError();
			printf("Ma loi - %d\n", ret);
			break;
		}

		choose[ret - 1] = 0; // bo di ki tu xuong dong
		printf("Du lieu nhan duoc tu client %d: %s\n", client, choose);

		if (strcmp(choose, "1") == 0) 
		{
			// Hien thi cac client dang dang nhap trong he thong
			DisplayAllClient(clientsLogin,numClientLogin, client);
		}
		else if (strcmp(choose, "2") == 0)
		{
			// gui den tat ca client da login trong he thong
			const char* msg = "Nhap noi dung muon chuyen tiep: \n";
			send(client, msg, strlen(msg), 0);
			ret = recv(client, buf, sizeof(buf), 0);
			buf[ret] = 0;
			SendAll(client, idSource, buf);
		}
		else if (strcmp(choose, "3") == 0)
		{
		// gui den 1 client login trong he thong
		char idDest[50], subbuf[256];
		const char* tmpbuf = "Nhap vao theo dinh dang: ID<khoang trang>Tin nhan muon gui\n";
		do {
				send(client, tmpbuf, strlen(tmpbuf), 0);
				ret = recv(client, buf, sizeof(buf), 0);
			} while (ret <= 0);
			
			buf[ret] = 0;
			printf("Du lieu nhan duoc la: %s\n", buf);
			sscanf(buf, "%s %s", idDest, subbuf);
			SendOne(client, idSource, subbuf, idDest);
		}
		else if (strcmp(choose, "4") == 0)
		{
			// dang xuat khoi he thong
			const char* msg = "He thong: Dang xuat khoi he thong thanh cong\nHe thong: Dang nhap lai theo dung cu phap\'client_id: xxxxxx\'\n";
			send(client, msg, strlen(msg), 0);
			Logout(clientsLogin,clientsIdLogin, &numClientLogin, &numClientIdLogin,client, idSource);
			// Dang nhap lai
		}
		else
		{
			const char* msg = "He thong canh bao: Vui long lua chon tu 1~4 de thuc hien cac thao tac\n";
			send(client, msg, strlen(msg), 0);
		}

	}
}

void AddLogin(SOCKET client) {
	clientsLogin[numClientLogin++] = client;
}

// them thanh cong thi tra ve 1, khong thanh cong thi tra ve 0
void AddLoginId(char *id) {
	strcpy(clientsIdLogin[numClientIdLogin++], id);
}

// tra ve gia tri 1 khi trung id, 0 khi khong trung id
int ExistDuplicateId(char *id) {
	int index = FindIndexId(clientsIdLogin, numClientIdLogin, id);
	// khong tim thay => bi trung
	if (index == -1)
		return 0;
	return 1;
}


void Disconnect(SOCKET client) {
	for (int i = 0; i < numClient; i++)
		if (clients[i] == client)
			if (i < numClient - 1) {
				clients[i] = clients[numClient - 1];
				break;
			}
	numClient--;
}

char* Menu() {
	char x[1000] = "\nHe thong: moi ban lua chon cac thao tac cua chuong trinh\n1. Xem cac tai khoan da dang nhap vao he thong\n2. Chat chung\n3. Chat rieng\n4. Dang xuat\nHay chon tu 1~4 de thao tac\n\nSu lua chon cua ban la: ";
	return x;
}

int FindIndexSocket(SOCKET clientsSocket[], SOCKET client) {
	for (int i = 0; i < sizeof(clientsSocket); i++) {
		if (client == clientsSocket[i])
			return i;
	}
	return -1;
}

int FindIndexId(char strList[][64], int size, char* id) {
	for (int i = 0; i < size; i++)
		if (strcmp(strList[i], id) == 0)
			return i;
	return -1;
}

void DisplayAllClient(SOCKET client_list[],int size, SOCKET client) {
	int index = FindIndexSocket(client_list, client);

	// neu nhu tai vi tri hien tai la client dang muon xem cac ket noi tu client khac
	char msg[256];
	sprintf(msg, "%s%s%s", "He thong: Client cua ban co id la: ", clientsIdLogin[index], "\n");
	// neu chi ton tai mot client trong he thong
	if (size == 1)
		strcat(msg, "He thong: Hien tai chi co minh ban trong he thong\n");
	send(client, msg, strlen(msg), 0);

	// do vi tri cua clientsLogin hien tai cung la vi tri cua clientsIdLogin nen co the dung 1 chi muc cho ca 2
	for (int i = 0; i < size; i++)
	{
		if (i != index)
		{
			char msg[256];
			sprintf(msg, "%s %s %s", "He thong: Client voi id", clientsIdLogin[i], "dang ket noi trong he thong\n");
			send(client, msg, strlen(msg), 0);
		}
	}
}



void SendAll(SOCKET client,char* id_source, char* content) {
	char buf[256];
	// noi chuoi
	sprintf(buf, "%s %s: %s","Client_id", id_source, content);
	for (int i = 0; i < numClientLogin; i++)
		if (clientsLogin[i] != client) 
			send(clientsLogin[i], buf, strlen(buf), 0);
}

void SendOne(SOCKET client, char* id_source, char* content, char* id_dest) {
	char buf[256];
	sprintf(buf, "%s %s: %s", "Client_id", id_source, content);
	for (int i = 0; i < numClientIdLogin; i++)
		if (strcmp(clientsIdLogin[i], id_dest) == 0)
			send(clientsLogin[i], buf, strlen(buf), 0);
}

void Logout(SOCKET clientList[], char clientListId[][64],int* sizeClientList, int* sizeIdList, SOCKET client, char * id) {
	RemoveFromListClient(clientList, sizeClientList, client);
	RemoveFromListId(clientListId, sizeIdList, id);
}

void RemoveFromListClient(SOCKET clientList[],int *size, SOCKET client) {
	int index = FindIndexSocket(clientList, client);
	if (index < *size - 1)
		clientList[index] = clientList[*size - 1];
	*size = *size - 1;
}

void RemoveFromListId(char strList[][64], int* size, char* id) {
	int index = FindIndexId(strList, *size, id);
	if (index < *size - 1)
		strcpy(strList[index], strList[*size - 1]);
	*size = *size - 1;
}
