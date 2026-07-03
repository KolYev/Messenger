#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	
	WSAData wsaData;
	(void)WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
	sockaddr_in address;
	address.sin_family = AF_INET; // текущий IP адрес
	address.sin_port = htons(4000); // сетевой порт
	address.sin_addr.s_addr = INADDR_ANY; // IP адрес для сетевого соединения ко всем доступным сетевым картам устройства

	// привязка сокета к адресу
	bind(server_socket, (struct sockaddr*)&address, sizeof(address));

	// прослушка входящего соединения с максимальным размером очереди для входящих соединий 5
	listen(server_socket, 5);

	SOCKET client_socket = accept(server_socket, nullptr, nullptr);

	// подключение к клиенту
	char buffer[1024] = { 0 };
	recv(client_socket, buffer, sizeof(buffer), 0);
	cout << "Сообщение от клиента: " << buffer << endl;

	closesocket(server_socket);
	closesocket(client_socket);
	WSACleanup();
}