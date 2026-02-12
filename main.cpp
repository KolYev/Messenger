#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>

#include <iostream>
#include <conio.h>
#include "udp.hpp"
#include "bluetooth.hpp"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    char choice;
    
    std::cout << "=== Chat Program ===\n";
    std::cout << "Select chat type:\n";
    std::cout << "1. UDP Chat\n";
    std::cout << "2. Bluetooth Chat\n";
    std::cout << "Enter choice (1 or 2): ";
    std::cin >> choice;
    
    if (choice == '1') {
        UDPSocketHandler udp;
        udp.run();
    }
    else if (choice == '2') {
        BluetoothChat bt; 
        bt.run();
    }
    else {
        std::cout << "Invalid choice!\n";
    }
    
    std::cout << "Program ended.\n";
    
    std::cout << "Press any key to exit...";
    _getch();
    
    return 0;
}