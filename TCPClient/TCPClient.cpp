// TCPClient.cpp : nhận dữ liệu từ bàn phím và gửi đến server
//

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

int main(int argc, char* argv[])
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

    // Nhận phản hồi từ server
    char message[256];
    ret = recv(client, message, sizeof(message), 0);
    message[ret] = 0;
    printf("\nServer: \n%s\n\n", message);

    // Liên tục đọc dữ liệu từ bàn phím rồi gửi tới server
    char buf[2048];
    while (1)
    {
        printf("Nhap du lieu muon gui: ");
        fgets(buf, sizeof(buf), stdin);
        
        if (buf[0] == '\0') break;

        if (strncmp(buf, "exit", 4) == 0)
            break;

        send(client, buf, strlen(buf), 0);
    }

    //// Nhan du lieu tu server va in ra man hinh
    //char buf[256];
    //while (1)
    //{
    //    ret = recv(client, buf, sizeof(buf), 0);
    //    if (ret <= 0)
    //        break;

    //    // Them ky tu ket thuc xau va in ra man hinh
    //    buf[ret] = 0;
    //    printf("Du lieu tu server: %s\n", buf);
    //}

    closesocket(client);
    WSACleanup();
}
