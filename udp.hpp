#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <optional>

class UDPSocketHandler
{
protected:
    sf::UdpSocket socket; // Сокет для сетевого соединения с другими хостами
    std::optional<sf::IpAddress> remoteIp; // IP собеседника
    unsigned short remotePort{0}; // Порт собеседника
    std::string username; // Получение имени пользователя для чата
public:
    void run(); // Главная функция для запуска чата
private:
    bool initServer(); // Функция для открытия порта на чтение
    bool initClient(); // Функция для нахождения сервера и запоминание его адреса
    void chatLoop();    // Основной цикл для отправки и получения сообщений
};