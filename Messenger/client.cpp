#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    WSAData wsaData;
    (void)WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(4000);
    InetPton(AF_INET, L"127.0.0.1", &address.sin_addr);

    connect(client_socket, (struct sockaddr*)&address,
        sizeof(address));

    char message[1024];
    cout << "Сообщение получателю: ";
    cin.getline(message, sizeof(message));
    send(client_socket, message, (int)strlen(message), 0);

    closesocket(client_socket);
    WSACleanup();

    return 0;
}