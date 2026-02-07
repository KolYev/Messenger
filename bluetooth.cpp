#include "bluetooth.hpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <conio.h>

// Конвертируем строку wstring (широкие символы) в string
std::string BluetoothChat::wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

// Поиск и вывод списка доступных Bluetooth устройств
std::vector<BluetoothDevice> BluetoothChat::discoverDevices() {
    std::vector<BluetoothDevice> devices; // Вектор для хранения устройств
    
    // Параметры для поиска Bluetooth радио-адаптеров
    BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) }; // Дескриптор радио-адаптера
    HANDLE hRadio = NULL; // Дескриптор радио-адаптера
    // Поиск Bluetooth радио-адаптеров
    HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&btfrp, &hRadio);
    
    // Если найден хотя бы один адаптер
    if (hFind != NULL) {
        do {
            // Получаем информацию о радио-адаптере
            BLUETOOTH_RADIO_INFO radioInfo = { sizeof(BLUETOOTH_RADIO_INFO), 0 };
            if (BluetoothGetRadioInfo(hRadio, &radioInfo) == ERROR_SUCCESS) {
                // Выводим имя радио-адаптера
                std::wcout << L"Found radio: " << radioInfo.szName << std::endl;
                
                // Параметры поиска устройств
                BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = {
                    sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
                    1,  // Возвращаем аутентифицированные устройства
                    0,  // Возвращаем запомненные устройства
                    1,  // Возвращаем неизвестные устройства
                    1,  // Возвращаем подключенные устройства
                    1,  // Выполняем поиск
                    15, // Таймаут поиска (чуть больше 15 секунд)
                    hRadio // Дескриптор радио-адаптера
                };
                
                // Информация о найденном устройстве
                BLUETOOTH_DEVICE_INFO deviceInfo = { sizeof(BLUETOOTH_DEVICE_INFO), 0 };
                // Начинаем поиск устройств
                HBLUETOOTH_DEVICE_FIND hDeviceFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
                
                // Если найдено хотя бы одно устройство
                if (hDeviceFind != NULL) {
                    do {
                        BluetoothDevice device;
                        // Конвертируем имя устройства из wstring в string
                        device.name = wstringToString(deviceInfo.szName);
                        // Копируем адрес устройства
                        device.address = deviceInfo.Address;
                        // Проверяем флаги подключения и аутентификации
                        device.isConnected = deviceInfo.fConnected != 0;
                        device.isAuthenticated = deviceInfo.fAuthenticated != 0;
                        
                        // Добавляем устройство в вектор
                        devices.push_back(device);
                        
                        // Выводим информацию об устройстве
                        std::cout << "Device: " << device.name << std::endl;
                        std::cout << "Address: ";
                        for (int i = 5; i >= 0; i--) {
                            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                                     << (int)deviceInfo.Address.rgBytes[i];
                            if (i > 0) std::cout << ":";
                        }
                        std::cout << std::dec << std::endl;
                        std::cout << "Connected: " << (device.isConnected ? "Yes" : "No") << std::endl;
                        std::cout << "Authenticated: " << (device.isAuthenticated ? "Yes" : "No") << std::endl;
                        std::cout << "------------------------" << std::endl;
                        
                    } while (BluetoothFindNextDevice(hDeviceFind, &deviceInfo));
                    
                    // Закрываем дескриптор поиска устройств
                    BluetoothFindDeviceClose(hDeviceFind);
                }
            }
            
            // Закрываем дескриптор радио-адаптера
            CloseHandle(hRadio);
        } while (BluetoothFindNextRadio(hFind, &hRadio)); // Ищем следующий радио-адаптер
        
        // Закрываем дескриптор поиска радио-адаптеров
        BluetoothFindRadioClose(hFind);
    }
    
    return devices; // Возвращаем список найденных устройств
}

// Функция для создания Bluetooth сервера (слушающего сокета)
SOCKET BluetoothChat::createServer() {
    WSADATA wsaData;
    // Инициализируем WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return INVALID_SOCKET;
    }
    
    // Создаем Bluetooth сокет
    // AF_BTH - семейство протоколов Bluetooth
    // SOCK_STREAM - потоковый сокет (TCP-подобный)
    // BTHPROTO_RFCOMM - протокол RFCOMM (последовательный порт поверх Bluetooth)
    SOCKET serverSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    // Настраиваем адрес для привязки
    SOCKADDR_BTH sa = { 0 }; // Обнуляем структуру
    sa.addressFamily = AF_BTH; // Семейство Bluetooth
    sa.port = BT_PORT_ANY; // Автоматический выбор порта
    
    // Привязываем сокет к адресу
    if (bind(serverSocket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        std::cout << "Bind failed\n";
        closesocket(serverSocket); // Закрываем сокет
        WSACleanup(); // Очищаем WinSock
        return INVALID_SOCKET;
    }
    
    // Получаем информацию о порте
    int len = sizeof(sa);
    getsockname(serverSocket, (SOCKADDR*)&sa, &len);
    
    std::cout << "Bluetooth server started on port: " << sa.port << std::endl;
    
    // Начинаем прослушивание входящих соединений
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    std::cout << "Waiting for Bluetooth connection...\n";
    return serverSocket; // Возвращаем дескриптор серверного сокета
}