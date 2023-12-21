//#include <stdio.h>
//#include <WinSock2.h>
//#include <iostream>
//using namespace std;
//
//#pragma comment(lib, "ws2_32")
//#pragma warning(disable:4996)
//#pragma warning(disable:4700)
//
//// Xóa kí tự trắng đầu chuỗi và cuối chuỗi
//char* trimString(char* str) {
//    char* end; /*Khai con trỏ của ký tự cuối cùng*/
//
//    /*Vòng lặp đầu tiên để xóa khoảng trắng từ đầu chuỗi*/
//    while (isspace((unsigned char)*str)) str++;
//
//    /*Nếu vòng lặp thực hiện hết cả chuỗi thì trả về chuỗi str*/
//    if (*str == 0) return str;
//
//
//    /*end là vị trí cuối chuỗi, cũng là vị trí bắt đầu vòng lặp thứ 2*/
//    end = str + strlen(str) - 1;
//    /*Vòng lặp thứ hai để xóa khoảng trắng từ cuối chuỗi*/
//    while (end > str && isspace((unsigned char)*end)) end--;
//
//    end[1] = '\0';
//
//    return str;
//}
//
//// Tách chuỗi ra thành 2 chuỗi được con ngăn cách bởi dấu cách
//void CutString(char* str, char* dest_1, char* dest_2)
//{
//    // Trim str when it begins and ends with space
//    char* strTrim;
//    strTrim = strdup(trimString(str));
//
//    int pos = 0;
//
//    for (int i = strlen(strTrim) - 1; i >=0 ; i--) {
//        if (strTrim[i] == ' ')
//        {
//            pos = i;
//            break;
//        }
//    }
//
//    int strLen = strlen(strTrim) - 1;
//    memcpy(dest_1, &strTrim[0], pos);
//    dest_1[pos] = '\0';
//    memcpy(dest_2, &strTrim[pos + 1], strLen);
//    dest_2[strLen] = '\0';
//}
//
//int main()
//{
//    char stringContent[256], ipAddress[256], port[256];
//    cout << "String content: ";
//    fgets(stringContent, sizeof(stringContent) + 1, stdin);
//    CutString(stringContent, ipAddress, port);
//    unsigned short port_number = (unsigned short)strtoul(port, NULL, 0);
//    // cout << "Type id: " << typeid(port_number).name();           check data type
//    /*cout << "Ip Address: " << ipAddress;
//    cout << "\nPort: " << port;*/
//
//    WSADATA wsa;
//    WORD version = MAKEWORD(2, 2);
//    if (WSAStartup(version, &wsa)) {
//        printf("Version does not supported");
//    }
//    
//    SOCKET TCPClient;
//    TCPClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//
//    SOCKADDR_IN addr;
//    addr.sin_family = AF_INET;
//    addr.sin_addr.s_addr = inet_addr(ipAddress);
//    addr.sin_port = htons(port_number);
//
//    // Buffer chua du lieu can gui
//    char sendBuf[256];
//    int pos = 0;
//
//    cout << "Nhap vao noi dung can chuyen cho server: ";
//    fgets(sendBuf, sizeof(sendBuf) + 1, stdin);
//    
//    connect(TCPClient, (SOCKADDR*)&addr, sizeof(addr));
//    send(TCPClient, sendBuf, pos, 0);
//
//    /*Trách nhiệm giải phóng bộ nhớ thuộc về bên gọi*/
//    closesocket(TCPClient);
//    WSACleanup();
//    exit(EXIT_SUCCESS);
//}

#include <stdio.h>
#include <WinSock2.h>

//#pragma comment(lib, "ws2_32")
//#pragma warning(disable:4996)

int main(int argc, char* argv[])
{
    char* target_ip = argv[1];
    unsigned short target_port = 0;

    for (int i = 0; argv[2][i]; i++) {
        target_port = target_port * 10 + argv[2][i] - '0';
    }

    printf("%s %hu\n", target_ip, target_port);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(target_ip);
    addr.sin_port = htons(target_port);

    system("pause");

    int ret = connect(client_sock, (SOCKADDR*)&addr, sizeof(addr));

    if (ret == SOCKET_ERROR)
    {
        ret = WSAGetLastError();
        printf("Connection fail - %d\n", ret);
        return 1;
    }

    char buf[256];

    ret = recv(client_sock, buf, sizeof(buf) - 1, 0);
    if (ret > 0) {
        buf[ret] = 0;
        printf("%s\n", buf);
    }

    while (1)
    {
        fgets(buf, sizeof(buf), stdin);

        if (strncmp(buf, "exit", 4) == 0) {
            printf("End. Close connection!");
            break;
        }

        send(client_sock, buf, strlen(buf), 0);
    }

    closesocket(client_sock);
    WSACleanup();
}