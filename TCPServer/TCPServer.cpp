// TCPServer.cpp : Mỗi khi có client kết nối đến, thì gửi xâu chào chứa trong một tệp tin, sau đó ghi toàn bộ nội dung client gửi đến vào một tệp tin khác. 
// Tên của tập tin chứa xâu chào và tập tin chứa nội dung client gửi đến được chỉ ra trong tham số dòng lệnh.
//

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)


int main()
{
    printf("SERVER\n\n");

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

    // Cho ket noi moi
    SOCKET client = accept(listener, NULL, NULL);

    // Gửi lời chào trong file tới client
    FILE* f = fopen("cau_chao.txt", "rb");
    char message[2048];
    int ret;

    while (!feof(f))
    {
        ret = fread(message, 1, sizeof(message), f);
        if (ret > 0)
        {
            send(client, message, ret, 0);
        }
    }

    fclose(f);


    // Nhận dữ liệu từ client và ghi vào file
    f = fopen("du_lieu_cua_client.txt", "wb");
    char buf[2048];

    while (1)
    {
        int ret = recv(client, buf, sizeof(buf), 0);

        if (ret <= 0)
        {
            if (ret == -1)
                ret = WSAGetLastError();
            break;
        }

        if (ret < sizeof(buf))
            buf[ret] = 0;
        fwrite(buf, 1, ret, f);
    }
    fclose(f);

    printf("\nKet thuc nhan du lieu\n\n");

    system("pause");

    closesocket(client);
    closesocket(listener);
    WSACleanup();
}

