#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <conio.h>
#include <vector>
#include <iomanip>

class TCPSocketHandler {
protected:
    sf::TcpSocket socket; // Сокет для сетевого соединения
    std::string username; // Получение имени пользователя для чата
public:
    void run(); // Главная функция для запуска чата
private:
    bool startServer(); // Функция для запуска сервера
    bool startClient(); // Функция для запуска клиента
    void chatLoop(); // Основной цикл для отправки и получения сообщений
};

bool TCPSocketHandler::startServer() {
    // Создаем сервер
    sf::TcpListener listener;

    if (listener.listen(2000) != sf::Socket::Status::Done)
        {
            std::cout << "Error listening on port!\n";
            return false;
        }
    
    // Принимаем входящее соединение
    if(listener.accept(socket) != sf::Socket::Status::Done)
    {
        std::cout << "Error!\n";
        return false;
    }

    return true;
}

bool TCPSocketHandler::startClient() {
    // Получаем локальный IP-адрес компьютера
    auto localIp = sf::IpAddress::getLocalAddress();
    // Проверяем, успешно ли получен IP
    if (!localIp.has_value())
    {
        std::cout << "Ошибка получения IP адреса\n";
        return false;
    }
    // Извлекаем значение IP-адреса
    sf::IpAddress ip = *localIp;

    // Подключаемся к серверу по полученному IP и порту 2000
    if(socket.connect(ip, 2000) != sf::Socket::Status::Done)
    {
        std::cout << "Ошибка подключения к серверу!\n";
        return false;
    }

    return true;
    
}