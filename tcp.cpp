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

void TCPSocketHandler::chatLoop() {
    socket.setBlocking(false);

    std::string message = "";
    sf::Packet packet;

    std::cout << "\n=== TCP чат начался ===\n";
    std::cout << "Напишите ваше сообщение и нажмите Enter, чтобы отправить\n";
    std::cout << "Напишите 'exit' для выхода\n\n";

    while (true)
    {
        // Проверка, была ли нажата клавиша
        if (_kbhit())
        {
            // Считываем строку с сообщением
            std::getline(std::cin, message);
            
            // Если сообщение не пустое
            if (!message.empty())
            {
                // Проверка команду выхода
                if (message == "exit" || message == "quit")
                {
                    break;
                }

                // Очищаем пакет от предыдущих данных
                packet.clear();
                // Упаковываем имя и сообщение в пакет
                packet << username << message;

                // Отправляем пакет через сокет
                if (socket.send(packet) != sf::Socket::Status::Done)
                {
                    std::cout << "Ошибка отправки сообщения!\n";
                }
                else
                {
                    std::cout << "You: " << message << '\n';
                }

                // Очищаем переменную сообщения
                message = "";
            }
        }

        // Принимаем данные через сокет
        sf::Socket::Status status = socket.receive(packet);
        // Если получено сообщение
        if (status == sf::Socket::Status::Done)
        {
            // Переменные для распаковки данных
            std::string nameRec;
            std::string messageRec;

            // Распаковываем имя и сообщение из пакета
            if (packet >> nameRec >> messageRec)
            {
                // Выводим полученное сообщение
                std::cout << nameRec << ": " << messageRec << '\n';
            }
        }
        // Если соединение разорвано
        else if (status == sf::Socket::Status::Disconnected)
        {
            std::cout << "Соединение прервано!\n";
            break;
        }

        // Небольшая задержка
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "TCP чат закончился.\n";

}