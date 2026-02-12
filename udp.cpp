#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "udp.hpp"
#include <iostream>
#include <thread>
#include <conio.h>

// Инициализация серверной части UDP-сокета
bool UDPSocketHandler::initServer() {
    // Привязываем сокет к порту 2000, проверяем успешность операци
    if (socket.bind(2000) != sf::Socket::Status::Done) { 
        std::cout << "Ошибка при привязке к порту 2000!\n";
        return false;
    }

    std::cout << "Сервер прослушивает порт 2000. Ожидает первого сообщения...\n";
    
    // Пакет для первого сообщения
    sf::Packet firstPacket;
    // Принимаем первое сообщение, сохраняем IP и порт отправителя
    if (socket.receive(firstPacket, remoteIp, remotePort) == sf::Socket::Status::Done) {
        // Строка для приветственного сообщения
        std::string hello;
        // Извлекаем строку из пакета
        if (firstPacket >> hello) {
            // Вывод информации о подключившемся клиенте
            std::cout << "Connected to: " << (remoteIp ? remoteIp->toString() : "unknown") 
                      << ":" << remotePort << " (" << hello << ")\n";
        }
    }
    // Возвращаем true, если инициализация успешна
    return true;
}

// Инициализация клиентской части UDP-сокета
bool UDPSocketHandler::initClient() {
    // Привязываем сокет к любому свободному порту
    if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Status::Done) {
        std::cerr << "Не удалось привязать клиентский сокет!\n";
        return false;
    }
    
    // Строка для ввода IP-адреса сервера
    std::string ipInput;
    // Запрашиваем IP сервера
    std::cout << "Enter Server IP: ";
    // Считываем IP из стандартного ввода
    std::cin >> ipInput;

    // Преобразовываем строку в IP-адрес
    auto result = sf::IpAddress::resolve(ipInput);
    // Проверяем успешность преобразования
    if (!result) {
        std::cerr << "Недопустимый IP-адрес!\n";
        return false;
    }
    // Сохраняем IP сервера
    remoteIp = result;
    // Устанавливаем порт сервера
    remotePort = 2000;

    // Создаем пакет с приветствием
    sf::Packet hello;
    // Записываем строку приветствия в пакет
    hello << "Приветствие от клиента";
    
    // Отправляем приветствие серверу
    if (socket.send(hello, *remoteIp, remotePort) != sf::Socket::Status::Done) {
        std::cerr << "Ошибка отправки приветствия!\n";
    }
    
    // Возвращаем true, если инициализация успешна
    return true;
}

// Основной цикл чата
void UDPSocketHandler::chatLoop() {
    // Устанавливаем неблокирующий режим сокета
    socket.setBlocking(false);
    // Создаем пакет для передачи данных
    sf::Packet packet;

    // Бесконечный цикл обмена сообщениями
    while (true) {
        // Проверяем нажатие клавиши в консоли
        if (_kbhit()) {
            // Строка для ввода сообщения
            std::string message;
            // Считываем строку из консоли
            std::getline(std::cin, message);
            // Выходим если введено "exit"
            if (message == "exit") break;

            // Если сообщение не пустое и известен удаленный IP
            if (!message.empty() && remoteIp) {
                // Очищаем пакет
                packet.clear();
                // Записываем имя пользователя и сообщение в пакет
                packet << username << message;
                
                // Отправляем пакет на удаленный IP и порт
                if (socket.send(packet, *remoteIp, remotePort) != sf::Socket::Status::Done) {
                    
                }
                // Выводим свое сообщение в консоль
                std::cout << "You: " << message << "\n";
            }
        }

        // Опциональный объект для IP отправителя
        std::optional<sf::IpAddress> senderIp;
        // Порт отправителя
        unsigned short senderPort;
        // Принимаем пакет от любого отправителя
        if (socket.receive(packet, senderIp, senderPort) == sf::Socket::Status::Done) {
            // Строки для имени и сообщения получателя
            std::string nameRec, msgRec;
            // Извлекаем имя и сообщение из пакета
            if (packet >> nameRec >> msgRec) {
                // Выводим полученное сообщение в консоль
                std::cout << nameRec << ": " << msgRec << "\n";
            }
        }
        // Небольшая задержка для снижения нагрузки на процессор
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void UDPSocketHandler::run() {
    std::cout << "Ваше имя: ";
    std::getline(std::cin, username);

    char type;
    std::cout << "Режим: [s] server, [c] client: ";
    std::cin >> type;
    std::cin.ignore();

    if (type == 's') {
        if (!initServer()) return;
    } else {
        if (!initClient()) return;
    }

    std::cout << "Ваше имя: ";
    std::getline(std::cin, username);
    chatLoop();
}