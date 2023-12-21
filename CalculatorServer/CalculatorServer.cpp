#include <stdio.h>
#include <winsock2.h>
#include <map>
#include <string>
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

std::map<std::string, std::string> param_map;

void ProcessQuery(char* req)
{
    char* p1 = strstr(req, "?");
    char* p2 = strstr(p1 + 1, " ");

    int paramLength = p2 - (p1 + 1);

    char* query = (char*)malloc(paramLength + 1);
    memcpy(query, p1 + 1, paramLength);
    query[paramLength] = 0;

    char* param = strtok(query, "&");
    while (param != NULL)
    {
        char* p = strstr(param, "=");

        // Tach phan name
        int nameLength = p - param;
        char* name = (char*)malloc(nameLength + 1);
        memcpy(name, param, nameLength);
        name[nameLength] = 0;

        // Tach phan value
        int valueLength = strlen(param) - nameLength - 1;
        char* value = (char*)malloc(valueLength + 1);
        memcpy(value, param + nameLength + 1, valueLength);
        value[valueLength] = 0;

        param_map[name] = value;

        param = strtok(NULL, "&");
    }
}
int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));
    listen(listener, 5);
    char buf[2048];
    int ret;
    while (1)
    {
        // Chap nhan ket noi
        SOCKET client = accept(listener, NULL, NULL);
        // Nhan yeu cau
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
        {
            closesocket(client);
            continue;
        }
        // Xu ly yeu cau
        if (ret < sizeof(buf))
            buf[ret] = 0;
        printf("%s\n", buf);
        ProcessQuery(buf);
        printf("%s\n", param_map["param2"]);

        const char* response = param_map["param1"].c_str();

        const char* header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(client, header, strlen(header), 0);
        send(client, response, 2, 0);

        param_map.clear();
        // Tra lai ket qua
        // Dong ket noi
        closesocket(client);
    }
}