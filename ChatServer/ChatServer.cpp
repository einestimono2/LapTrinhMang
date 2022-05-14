// ChatServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32")

int numClient = 0;
SOCKET clients[64];
char buf[256];

DWORD WINAPI ClientThread(LPVOID lpParam)
{
    int ret;
    char* mes;

    SOCKET client = *(SOCKET*)lpParam;
   
    do {
        // Gui thong bao yeu cau nhap dung cu phap de dang nhap
        char* notification = (char*) "\nCu phap dang nhap 'client_id: xxx': ";
        send(client, notification, strlen(notification), 0);

        // Nhan thong tin cu phap tu client
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;

        // Tach lenh va id tu thong tin cua client
        char command[256];
        int client_id;
        sscanf_s(buf, "%s %d", command, sizeof(command), &client_id);

        // Truong hop sai cu phap
        if (strcmp(command, "client_id:") != 0) mes = (char*)"Sai cau lenh 'client_id' !\n";
        // Truong hop sai id
        else if (client_id != client) mes = (char*)"Sai id !";
        // Dang nhap thanh cong ==> them vao mang clients
        else {
            mes = (char*)"Dang nhap thanh cong !\n";
            // Dang nhap thanh cong ==> them vao clients
            clients[numClient] = client;
            numClient++;
        }

        // Gui thong bao ket qua toi client
        send(client, mes, strlen(mes), 0);

    } while (strcmp(mes, "Dang nhap thanh cong !\n") != 0);

    // ====================================================
    // Nhan du lieu tu client va in ra man hinh
    while (1)
    {
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;
        printf("Client %d da gui: %s\n", client, buf);

        // Gui tin nhan toi nhung nguoi khac
        for (int i = 0; i < numClient; i++)
        {
            // Bo qua chinh minh
            if (clients[i] == client) continue;
            // Gui tin voi cu phap "client_id: thong_tin_gui"
            char data[1024];
            // Them cu phap "client_id:" vao dau cua data
            snprintf(data, 10, "%d: ", client);
            // Them du lieu cua client[i] vao sau cu phap "client_id:" cua data
            strcat_s(data, buf);
            // Gui data toi client i
            send(clients[i], data, strlen(data), 0);
        }
    }
    closesocket(client);
}

int main()
{
    // Khoi tao thu vien
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Tao socket
    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Khai bao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(8000);

    // Gan cau truc dia chi voi socket
    bind(listener, (SOCKADDR*)&addr, sizeof(addr));

    // Chuyen sang trang thai cho ket noi
    listen(listener, 5);

    while (1)
    {
        SOCKET client = accept(listener, NULL, NULL);
        printf("Client moi ket noi: %d\n", client);
        CreateThread(0, 0, ClientThread, &client, 0, 0);
    }
}





