#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

class Server {
private:
	sockaddr_in address;
	SOCKET server_socket;
	SOCKET client_socket;

public:
	Server(int port)
	{
		WSAData wsaData;
		(void)WSAStartup(MAKEWORD(2, 2), &wsaData);

		server_socket = socket(AF_INET, SOCK_STREAM, 0);
		address.sin_family = AF_INET; // семейство адресов — IPv4
		address.sin_port = htons(port); // сетевой порт
		address.sin_addr.s_addr = INADDR_ANY; // слушать на всех сетевых интерфейсах
	}

	void bind_and_listen()
	{
		// привязка сокета к адресу
		bind(server_socket, (struct sockaddr*)&address, sizeof(address));

		// прослушка входящего соединения с максимальным размером очереди для входящих соединий 5
		listen(server_socket, 5);
	}

	void accept_client()
	{
		client_socket = accept(server_socket, nullptr, nullptr);
	}

	const char* receive_message()
	{
		char buffer[1024] = { 0 };
		int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
		if (bytes > 0)
			buffer[bytes] = '\0';

		return buffer;
	}

	~Server()
	{
		closesocket(server_socket);
		closesocket(client_socket);
		WSACleanup();
	}
};


int main()
{
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	
	Server server(4000);
	server.bind_and_listen();
	server.accept_client();
	

	cout << "Сообщение от клиента: " << server.receive_message() << endl;

	
}