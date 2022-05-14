// TelnetServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
    char *mes, *notification;
    char user[1024], pass[1024];

    SOCKET client = *(SOCKET*)lpParam;

    do {
        // Thong bao nhap tai khoan
        notification = (char*)"\nNhap user: ";
        send(client, notification, strlen(notification), 0);
        // Nhan thong user do client nhap
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;
        // Luu user vua nhap vao user
        sscanf_s(buf, "%s", user, sizeof(user));

        // ======================================================================
        
        // Thong bao nhap map khau
        notification = (char*)"Nhap password: ";
        send(client, notification, strlen(notification), 0);
        // Nhan thong user do client nhap
        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;
        // Luu user vua nhap vao user
        sscanf_s(buf, "%s", pass, sizeof(pass));

        // ======================================================================

        int matched = 1;

        // Doc file
        FILE* f; 
        fopen_s(&f, "user-password.txt", "rt");
        while (!feof(f))
        {
            char line[1024], _user[1024], _pass[1024];
            // Doc tung dong 1
            fgets(line, sizeof(line), f);
            // Loai bo ki tu \n o cuoi neu co
            if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = 0;
            // Doc user va pass trong moi dong (ngan cach boi khoang trang)
            sscanf_s(line, "%s %s", _user, sizeof(_user), _pass, sizeof(_pass));
            
            // Kiem tra thong tin client nhap        
            if (strcmp(user, _user) != 0) matched = 1;
            else
            {
                if (strcmp(pass, _pass) != 0) matched = 2;
                else matched = 3;
                break;
            }
        }
        fclose(f);
        
        if(matched == 1) mes = (char*)"\nUser khong ton tai !\n";
        else if (matched == 2) mes = (char*)"\nPassword khong dung !\n";
        else
        {
            mes = (char*)"\nDang nhap thanh cong !\n";
            clients[numClient] = client;
            numClient++;
        }

        // Gui thong bao ket qua toi client
        send(client, mes, strlen(mes), 0);

    } while (strcmp(mes, "\nDang nhap thanh cong !\n") != 0);

    // ====================================================
    // Nhan lenh tu client va thuc thi
    while (1)
    {
        notification = (char*)"\nNhap lenh: ";
        send(client, notification, strlen(notification), 0);

        ret = recv(client, buf, sizeof(buf), 0);
        if (ret <= 0)
            break;
        buf[ret] = 0;

        printf("Client %d da nhap lenh: %s\n", client, buf);

        // Loai bo ki tu '\n' o cuoi cung cua buf
        if (buf[strlen(buf) - 1] == '\n') buf[strlen(buf) - 1] = 0;

        // Thuc hien lenh va dinh huong ket qua vao file
        sprintf_s(buf + strlen(buf), sizeof(buf), "%s", ">out_put.txt");
        system(buf);

        // Doc file de lay ket qua thuc thi lenh cua nguoi dung
        FILE* f;
        fopen_s(&f, "out_put.txt", "rb");
        fseek(f, 0, SEEK_END);  // Dua con tro ve cuoi file
        int flen = ftell(f);    // Lay vi tri con tro cuoi file
        fseek(f, 0, SEEK_SET);  // Dua con tro ve lai dau file
        char* fdata = (char*)calloc(flen, 1);
        fread(fdata, 1, flen, f);
        fclose(f);

        // Gui ket qua toi client khac
        // Gui tieu de
        notification = (char*)"\n===== Ket qua sau khi thuc hien lenh =====\n\n";
        send(client, notification, strlen(notification), 0);
        // Gui ket qua lai cho nguoi dung
        send(client, fdata, flen, 0);
        //for (int i = 0; i < numClient; i++)
        //{
        //    // Bo qua chinh minh
        //    if (clients[i] == client) continue;
        //    // Gui tieu de
        //    notification = (char*)"\n===== Ket qua sau khi thuc hien lenh =====\n\n";
        //    send(client, notification, strlen(notification), 0);
        //    // Gui ket qua lai cho nguoi dung
        //    send(client, fdata, flen, 0);
        //}

        // Giai phong
        free(fdata);
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
