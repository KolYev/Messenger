#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <conio.h>
#include <vector>
#include <iomanip>

// Отдельный модуль для Bluetooth с правильным порядком включения заголовков
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2bth.h>
#include <bluetoothapis.h>
#endif

// Функция для запуска SFML чата (ваш исходный код)
void runSfmlChat() {
    sf::TcpSocket socket;

    auto localIp = sf::IpAddress::getLocalAddress();
    if (!localIp.has_value())
    {
        std::cout << "Failed to get local IP address\n";
        return;
    }
    sf::IpAddress ip = *localIp;

    char type;

    std::cout << "Enter type connecting: [c] - client, [s] - server\n";
    std::cin  >> type;

    if(type == 's')
    {
        sf::TcpListener listener;
        if (listener.listen(2000) != sf::Socket::Status::Done)
        {
            std::cout << "Error listening on port!\n";
            return;
        }

        if(listener.accept(socket) != sf::Socket::Status::Done)
        {
            std::cout << "Error!\n";
        }
    }
    else if(type == 'c')
    {
        if(socket.connect(ip, 2000) != sf::Socket::Status::Done)
        {
            std::cout << "Error!\n";
        }
    }

    std::string name;
    std::cout << "Enter your name:\n";
    std::cin  >> name;

    socket.setBlocking(false);

    std::string message = "";
    sf::Packet  packet;

    std::cout << "\n=== SFML Chat started ===\n";
    std::cout << "Type your message and press Enter to send\n";
    std::cout << "Type 'exit' to quit\n\n";

    while (true)
    {
        if (_kbhit())
        {
            std::getline(std::cin, message);
            
            if (!message.empty())
            {
                if (message == "exit" || message == "quit")
                {
                    break;
                }

                packet.clear();
                packet << name << message;

                if (socket.send(packet) != sf::Socket::Status::Done)
                {
                    std::cout << "Error sending message!\n";
                }
                else
                {
                    std::cout << "You: " << message << '\n';
                }

                message = "";
            }
        }

        sf::Socket::Status status = socket.receive(packet);
        if (status == sf::Socket::Status::Done)
        {
            std::string nameRec;
            std::string messageRec;

            if (packet >> nameRec >> messageRec)
            {
                std::cout << nameRec << ": " << messageRec << '\n';
            }
        }
        else if (status == sf::Socket::Status::Disconnected)
        {
            std::cout << "Connection lost!\n";
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "SFML Chat ended.\n";
}

#ifdef _WIN32
// Структура для хранения информации о Bluetooth устройствах
struct BluetoothDevice {
    std::string name;
    BLUETOOTH_ADDRESS address;
    bool isConnected;
    bool isAuthenticated;
};

// Вспомогательная функция для конвертации широкосимвольной строки в std::string
std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

// Функция для вывода списка Bluetooth устройств
std::vector<BluetoothDevice> discoverBluetoothDevices() {
    std::vector<BluetoothDevice> devices;
    
    BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
    HANDLE hRadio = NULL;
    HBLUETOOTH_RADIO_FIND hFind = BluetoothFindFirstRadio(&btfrp, &hRadio);
    
    if (hFind != NULL) {
        do {
            BLUETOOTH_RADIO_INFO radioInfo = { sizeof(BLUETOOTH_RADIO_INFO), 0 };
            if (BluetoothGetRadioInfo(hRadio, &radioInfo) == ERROR_SUCCESS) {
                std::wcout << L"Found radio: " << radioInfo.szName << std::endl;
                
                BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = {
                    sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS),
                    1,  // return authenticated
                    0,  // return remembered
                    1,  // return unknown
                    1,  // return connected
                    1,  // issue inquiry
                    15, // timeout multiplier
                    hRadio
                };
                
                BLUETOOTH_DEVICE_INFO deviceInfo = { sizeof(BLUETOOTH_DEVICE_INFO), 0 };
                HBLUETOOTH_DEVICE_FIND hDeviceFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
                
                if (hDeviceFind != NULL) {
                    do {
                        BluetoothDevice device;
                        device.name = wstringToString(deviceInfo.szName);
                        device.address = deviceInfo.Address;
                        device.isConnected = deviceInfo.fConnected != 0;
                        device.isAuthenticated = deviceInfo.fAuthenticated != 0;
                        
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
                    
                    BluetoothFindDeviceClose(hDeviceFind);
                }
            }
            
            CloseHandle(hRadio);
        } while (BluetoothFindNextRadio(hFind, &hRadio));
        
        BluetoothFindRadioClose(hFind);
    }
    
    return devices;
}

// Функция для создания Bluetooth сервера (слушающего сокета)
SOCKET createBluetoothServer() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return INVALID_SOCKET;
    }
    
    // Создаем Bluetooth сокет
    SOCKET serverSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    // Настраиваем адрес для привязки
    SOCKADDR_BTH sa = { 0 };
    sa.addressFamily = AF_BTH;
    sa.port = BT_PORT_ANY;
    
    // Привязываем сокет
    if (bind(serverSocket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        std::cout << "Bind failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    // Получаем информацию о порте
    int len = sizeof(sa);
    getsockname(serverSocket, (SOCKADDR*)&sa, &len);
    
    std::cout << "Bluetooth server started on port: " << sa.port << std::endl;
    
    // Начинаем прослушивание
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "Listen failed\n";
        closesocket(serverSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    std::cout << "Waiting for Bluetooth connection...\n";
    return serverSocket;
}

// Функция для подключения к Bluetooth устройству
SOCKET connectToBluetoothDevice(const BLUETOOTH_ADDRESS& address, int port = 1) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return INVALID_SOCKET;
    }
    
    // Создаем сокет
    SOCKET clientSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    // Настраиваем адрес для подключения
    SOCKADDR_BTH sa = { 0 };
    sa.addressFamily = AF_BTH;
    sa.btAddr = address.ullLong;
    sa.port = port;
    
    // Подключаемся
    std::cout << "Connecting to Bluetooth device...\n";
    if (connect(clientSocket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        std::cout << "Connection failed with error: " << error << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    std::cout << "Connected successfully!\n";
    return clientSocket;
}

// Функция для запуска Bluetooth чата
void runBluetoothChat() {
    std::cout << "\n=== Bluetooth Chat ===\n";
    
    char chatMode;
    std::cout << "Select mode: [s] - Server, [c] - Client\n";
    std::cin >> chatMode;
    
    SOCKET chatSocket = INVALID_SOCKET;
    std::string name;
    
    if (chatMode == 's' || chatMode == 'S') {
        // Режим сервера
        std::cout << "Starting Bluetooth server...\n";
        
        // Получаем список устройств для информации
        auto devices = discoverBluetoothDevices();
        
        // Создаем сервер
        SOCKET serverSocket = createBluetoothServer();
        if (serverSocket == INVALID_SOCKET) {
            return;
        }
        
        // Принимаем соединение
        SOCKADDR_BTH clientAddr;
        int addrLen = sizeof(clientAddr);
        chatSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &addrLen);
        
        if (chatSocket == INVALID_SOCKET) {
            std::cout << "Accept failed\n";
            closesocket(serverSocket);
            WSACleanup();
            return;
        }
        
        std::cout << "Client connected!\n";
        closesocket(serverSocket);
    }
    else if (chatMode == 'c' || chatMode == 'C') {
        // Режим клиента
        std::cout << "Discovering Bluetooth devices...\n";
        
        auto devices = discoverBluetoothDevices();
        
        if (devices.empty()) {
            std::cout << "No Bluetooth devices found!\n";
            return;
        }
        
        int choice;
        std::cout << "\nSelect device to connect (1-" << devices.size() << "): ";
        std::cin >> choice;
        
        if (choice < 1 || choice > devices.size()) {
            std::cout << "Invalid choice!\n";
            return;
        }
        
        // Подключаемся к выбранному устройству
        int port;
        std::cout << "Enter port number (usually 1-30): ";
        std::cin >> port;
        
        chatSocket = connectToBluetoothDevice(devices[choice - 1].address, port);
        
        if (chatSocket == INVALID_SOCKET) {
            return;
        }
    }
    else {
        std::cout << "Invalid mode!\n";
        return;
    }
    
    // Получаем имя пользователя
    std::cout << "Enter your name: ";
    std::cin >> name;
    
    // Устанавливаем неблокирующий режим
    u_long nonBlockingMode = 1;
    ioctlsocket(chatSocket, FIONBIO, &nonBlockingMode);
    
    std::cout << "\n=== Bluetooth Chat started ===\n";
    std::cout << "Type your message and press Enter to send\n";
    std::cout << "Type 'exit' to quit\n\n";
    
    char buffer[1024];
    std::string message;
    
    while (true) {
        // Проверяем ввод с клавиатуры
        if (_kbhit()) {
            std::getline(std::cin, message);
            
            if (!message.empty()) {
                if (message == "exit" || message == "quit") {
                    break;
                }
                
                // Формируем сообщение с именем
                std::string fullMessage = name + ": " + message;
                
                // Отправляем сообщение
                int bytesSent = send(chatSocket, fullMessage.c_str(), (int)fullMessage.length() + 1, 0);
                if (bytesSent == SOCKET_ERROR) {
                    if (WSAGetLastError() != WSAEWOULDBLOCK) {
                        std::cout << "Error sending message!\n";
                    }
                }
                else {
                    std::cout << "You: " << message << '\n';
                }
            }
        }
        
        // Проверяем входящие сообщения
        int bytesReceived = recv(chatSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            std::cout << buffer << '\n';
        }
        else if (bytesReceived == 0) {
            std::cout << "Connection closed by peer\n";
            break;
        }
        else if (WSAGetLastError() != WSAEWOULDBLOCK) {
            std::cout << "Receive error: " << WSAGetLastError() << "\n";
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Завершаем работу
    if (chatSocket != INVALID_SOCKET) {
        closesocket(chatSocket);
    }
    WSACleanup();
    
    std::cout << "Bluetooth Chat ended.\n";
}
#else
void runBluetoothChat() {
    std::cout << "Bluetooth chat is only supported on Windows.\n";
}
#endif

int main() {
    char choice;
    
    std::cout << "=== Chat Program ===\n";
    std::cout << "Select chat type:\n";
    std::cout << "1. SFML Chat (TCP/IP)\n";
    std::cout << "2. Bluetooth Chat\n";
    std::cout << "Enter choice (1 or 2): ";
    std::cin >> choice;
    
    if (choice == '1') {
        runSfmlChat();
    }
    else if (choice == '2') {
        runBluetoothChat();
    }
    else {
        std::cout << "Invalid choice!\n";
    }
    
    std::cout << "Program ended.\n";
    
    // Ожидание нажатия любой клавиши перед выходом
    std::cout << "Press any key to exit...";
    _getch();
    
    return 0;
}