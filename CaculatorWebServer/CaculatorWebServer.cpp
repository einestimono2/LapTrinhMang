#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

void ProcessHeaders(char* req, SOCKET client)
{
    /*char cmd[8], path[256];
    int ret = sscanf(req, "%s %s", cmd, path);*/

    char* p1 = strstr(req, " ");
    char* p2 = strstr((p1+1), " ");

    int headerLength = p2 - (p1 + 1);
    char* headers = (char*)malloc(headerLength + 1);
    memcpy(headers, p1 + 1, headerLength);
    headers[headerLength] = 0;

    printf("Headers: %s\n", headers);

    // Op, param1, param2
    char op[16], param1[512], param2[512];
    op[0] = 0;
    param1[0] = 0;
    param2[0] = 0;

    // Tach tung header (bo qua )
    char* header = strtok(headers + 2, "&");

    while (header != NULL)
    {
        char* p = strstr(header, "=");

        // Tach phan name
        int nameLength = p - header;
        char* name = (char*)malloc(nameLength + 1);
        memcpy(name, header, nameLength);
        name[nameLength] = 0;

        // Tach phan value
        int valueLength = strlen(header) - nameLength - 1;
        char* value = (char*)malloc(valueLength + 1);
        memcpy(value, header + nameLength + 1, valueLength);
        value[valueLength] = 0;

        if (strcmp(name, "op") == 0)
        {
            sprintf(op, "%s", value);
        }
        else if (strcmp(name, "param1") == 0)
        {
            sprintf(param1, "%s", value);
        }
        else if (strcmp(name, "param2") == 0)
        {
            sprintf(param2, "%s", value);
        }
        else {
            const char* response = "HTTP/1.1 404 Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Tham so khong hop le!</h1></body></html>";
            send(client, response, strlen(response), 0);
            return;
        }

        header = strtok(NULL, "&");
    }

    if (op[0] == 0 || param1[0] == 0 || param2[0] == 0)
    {
        const char* response = "HTTP/1.1 404 Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Thieu tham so!</h1></body></html>";
        send(client, response, strlen(response), 0);
    }
    else {
        char response[1024];
        if (strcmp(op, "add") == 0)
        {
            int num1 = atoi(param1);
            int num2 = atoi(param2);
            int result = num1 + num2;
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>%d + %d = %d</h1></body></html>", num1, num2, result);
            send(client, response, strlen(response), 0);
        }
        else if (strcmp(op, "sub") == 0)
        {
            int num1 = atoi(param1);
            int num2 = atoi(param2);
            int result = num1 - num2;
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>%d - %d = %d</h1></body></html>", num1, num2, result);
            send(client, response, strlen(response), 0);
        }
        else if (strcmp(op, "mul") == 0)
        {
            int num1 = atoi(param1);
            int num2 = atoi(param2);
            int result = num1 * num2;
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>%d x %d = %d</h1></body></html>", num1, num2, result);
            send(client, response, strlen(response), 0);
        }
        else if (strcmp(op, "div") == 0)
        {
            int num1 = atoi(param1);
            int num2 = atoi(param2);

            if (num2 == 0)
            {
                const char* response = "HTTP/1.1 404 Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Tham so thu 2 phai khac 0!</h1></body></html>";
                send(client, response, strlen(response), 0);
                return;
            }

            int result = num1 / num2;
            sprintf(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>%d / %d = %d</h1></body></html>", num1, num2, result);
            send(client, response, strlen(response), 0);
        }
        else {
            const char* response = "HTTP/1.1 404 Error\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Phep tinh khong hop le!</h1></body></html>";
            send(client, response, strlen(response), 0);
        }
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
        //printf("%s\n", buf);

         ProcessHeaders(buf, client);

        // Tra lai ket qua
        // Dong ket noi
        //closesocket(client);
    }
}