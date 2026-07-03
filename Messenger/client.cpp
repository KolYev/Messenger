#include <iostream>
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
    WSAData wsaData;
    (void)WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(4000);
    InetPton(AF_INET, L"127.0.0.1", &serverAddress.sin_addr);

    connect(clientSocket, (struct sockaddr*)&serverAddress,
        sizeof(serverAddress));

    char message[1024];
    cin.getline(message, sizeof(message));
    send(clientSocket, message, (int)strlen(message), 0);

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}