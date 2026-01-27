#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include <chrono>
#include <conio.h>

int main()
{
    sf::TcpSocket socket;

    auto localIp = sf::IpAddress::getLocalAddress();
    if (!localIp.has_value())
    {
        std::cout << "Failed to get local IP address\n";
        return 1;
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
            return 1;
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

    std::cout << "\n=== Chat started ===\n";
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

    std::cout << "Chat ended.\n";
    return 0;
}