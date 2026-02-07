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