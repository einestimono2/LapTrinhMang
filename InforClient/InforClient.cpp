//Chương trình clientinfo thực hiện kết nối đến máy chủ và gửi thông tin về tên máy, danh sách các ổ đĩa có trong máy, kích thước các ổ đĩa.

#include <stdio.h>
#include <WinSock2.h>

#include <windows.h>
#include <iostream>
#include <lmcons.h>
#include <codecvt>
#include <sstream>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

using namespace std;

int main()
{
    printf("CLIENT\n\n");

    // Khoi tao thu vien
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Tao socket
    SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Khai bao dia chi server
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8000);

    system("pause");

    // Ket noi toi server
    int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));
    if (ret == SOCKET_ERROR)
    {
        ret = WSAGetLastError();
        printf("Ket noi khong thanh cong - %d\n", ret);
        return 1;
    }

    // Gửi thông tin tên máy tới server
    send(client, "Ten may: ", strlen("Ten may: "), 0);

    // Lấy tên máy
    TCHAR computerName[UNCLEN + 1];
    DWORD computer_len = UNCLEN + 1;
    GetComputerName((TCHAR*)computerName, &computer_len);

    // Convert TCHAR sang char
    char msg[500];
    wcstombs(msg, computerName, wcslen(computerName) + 1);

    //Gửi tên máy
    send(client, msg, strlen(msg), 0);

    system("pause");

    //Gửi danh sách ổ đĩa và dung lượng của nó
    send(client, "\n\nDanh sach o dia va kich thuoc cua tung o dia:\n", strlen("\n\nDanh sach o dia va kich thuoc cua tung o dia:\n"), 0);

    //===
    char* szDrives = new char[MAX_PATH]();
    string name = "C:\\\\";
    string size = "";
    ULARGE_INTEGER FreeBytesAvailable, TotalNumberOfBytes, TotalNumberOfFreeBytes;

    GetLogicalDriveStringsA(MAX_PATH, szDrives);

    for (int i = 0; i < 100; i += 4)
        if (szDrives[i] != (char)0)
        {
            name[0] = szDrives[i];
            send(client, name.substr(0, 2).c_str(), strlen(name.substr(0, 2).c_str()), 0);

            wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            wstring wide = converter.from_bytes(name);
            LPCWSTR result = wide.c_str();

            GetDiskFreeSpaceEx(result, &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes);

            string totalSize = "\nTong dung luong tren o dia: " + to_string(TotalNumberOfBytes.QuadPart / 1024.0 / 1024 / 1024) + " GB";
            send(client, totalSize.c_str(), strlen(totalSize.c_str()), 0);

            string freeAvailable = "\nTong dung luong trong co san: " + to_string(FreeBytesAvailable.QuadPart / 1024.0 / 1024 / 1024) + " GB";
            send(client, freeAvailable.c_str(), strlen(freeAvailable.c_str()), 0);

            string totalFree = "\nDung luong trong tren o dia: " + to_string(TotalNumberOfFreeBytes.QuadPart / 1024.0 / 1024 / 1024) + " GB\n\n";
            send(client, totalFree.c_str(), strlen(totalFree.c_str()), 0);

        }

    closesocket(client);
    WSACleanup();
}