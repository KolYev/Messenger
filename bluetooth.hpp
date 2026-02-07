#pragma once
#include <string>
#include <vector>

// Отдельный модуль для Bluetooth с правильным порядком включения заголовков
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <bluetoothapis.h>
#endif

// Структура для хранения информации о Bluetooth устройствах
struct BluetoothDevice
{
    std::string name;          // Имя устройства
    BLUETOOTH_ADDRESS address; // MAC-адрес Bluetooth
    bool isConnected;          // Подключено ли устройство
    bool isAuthenticated;      // Прошло ли аутентификацию
};

class BluetoothChat
{
public:
    void run(); // Главная функция для запуска чата
private:
    std::vector<BluetoothDevice> discoverDevices(); // Поиск и вывод списка доступных Bluetooth устройств
    SOCKET createServer();
    SOCKET connectToDevice(const BLUETOOTH_ADDRESS &addr, int port);
    void chatLoop(SOCKET sock, const std::string &username);
    std::string wstringToString(const std::wstring &wstr); // Конвертируем строку wstring (широкие символы) в string
};