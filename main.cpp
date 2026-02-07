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

class TCP {
public:
    void runSfmlChat() {
    // Сокет для сетевого соединения
    sf::TcpSocket socket;

    // Получаем локальный IP-адрес компьютера
    auto localIp = sf::IpAddress::getLocalAddress();
    // Проверяем, успешно ли получен IP
    if (!localIp.has_value())
    {
        std::cout << "Failed to get local IP address\n";
        return;
    }
    // Извлекаем значение IP-адреса
    sf::IpAddress ip = *localIp;

    char type;

    std::cout << "Enter type connecting: [c] - client, [s] - server\n";
    std::cin  >> type;

    if(type == 's')
    {
        // Создаем сервер
        sf::TcpListener listener;
        // Попытка прослушать порт 2000
        if (listener.listen(2000) != sf::Socket::Status::Done)
        {
            std::cout << "Error listening on port!\n";
            return;
        }

        std::cout << "Соединение...\n";

        // Принимаем входящее соединение
        if(listener.accept(socket) != sf::Socket::Status::Done)
        {
            std::cout << "Error!\n";
        }
    }
    else if(type == 'c')
    {
        // Подключаемся к серверу по полученному IP и порту 2000
        if(socket.connect(ip, 2000) != sf::Socket::Status::Done)
        {
            std::cout << "Error!\n";
        }
    }

    // Получение имя пользователя для чата
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
                packet << name << message;

                // Отправляем пакет через сокет
                if (socket.send(packet) != sf::Socket::Status::Done)
                {
                    std::cout << "Error sending message!\n";
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
            std::cout << "Connection lost!\n";
            break;
        }

        // Небольшая задержка
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "SFML Chat ended.\n";
}       
};


// BLUETOOTH чат (только для Windows)

#ifdef _WIN32
// Структура для хранения информации о Bluetooth устройствах
struct BluetoothDevice {
    std::string name; // Имя устройства
    BLUETOOTH_ADDRESS address; // MAC-адрес Bluetooth
    bool isConnected; // Подключено ли устройство
    bool isAuthenticated; // Прошло ли аутентификацию
};

// Конвертируем строку wstring (широкие символы) в string
std::string wstringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

// Поиск и вывод списка доступных Bluetooth устройств
std::vector<BluetoothDevice> discoverBluetoothDevices() {
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
SOCKET createBluetoothServer() {
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

// Функция для подключения к Bluetooth устройству
SOCKET connectToBluetoothDevice(const BLUETOOTH_ADDRESS& address, int port = 1) {
    WSADATA wsaData;
    // Инициализируем WinSock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "WSAStartup failed\n";
        return INVALID_SOCKET;
    }
    
    // Создаем клиентский сокет
    SOCKET clientSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Socket creation failed\n";
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    // Настраиваем адрес для подключения
    SOCKADDR_BTH sa = { 0 };
    sa.addressFamily = AF_BTH; // Семейство Bluetooth
    sa.btAddr = address.ullLong; // MAC-адрес целевого устройства
    sa.port = port; // Порт для подключения
    
    // Подключаемся к устройству
    std::cout << "Connecting to Bluetooth device...\n";
    if (connect(clientSocket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        int error = WSAGetLastError(); // Получаем код ошибки
        std::cout << "Connection failed with error: " << error << "\n";
        closesocket(clientSocket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    
    std::cout << "Connected successfully!\n";
    return clientSocket; // Возвращаем дескриптор подключенного сокета
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
        TCP tcp;
        tcp.runSfmlChat();
    }
    else if (choice == '2') {
        runBluetoothChat();
    }
    else {
        std::cout << "Invalid choice!\n";
    }
    
    std::cout << "Program ended.\n";
    
    std::cout << "Press any key to exit...";
    _getch();
    
    return 0;
}