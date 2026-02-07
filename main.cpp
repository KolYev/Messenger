#include <windows.h>
#include <iostream>
#include <conio.h> 
#include "tcp.hpp" 
#include "bluetooth.hpp"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    char choice;
    
    std::cout << "=== Chat Program ===\n";
    std::cout << "Select chat type:\n";
    std::cout << "1. TCP Chat\n";
    std::cout << "2. Bluetooth Chat\n";
    std::cout << "Enter choice (1 or 2): ";
    std::cin >> choice;
    
    if (choice == '1') {
        TCPSocketHandler tcp;
        tcp.run();
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